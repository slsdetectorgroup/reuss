#pragma once
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/Receiver.h"
#include "reuss/utils.h"

#include <chrono>
#include <fmt/color.h>
#include <fmt/format.h>
#include <memory>
#include <thread>
#include <vector>

namespace reuss {

class FrameAssembler {
    std::vector<ImageFifo *> fifos_;
    std::atomic<bool> stopped_{false};
    std::atomic<bool> stop_requested_{false};
    // ImageFifo assembled_images_;
    std::vector<ImageFifo> assembled_images_;
    std::vector<int64_t> frame_numbers_; // For equality check
    size_t part_size_ = 0;

  public:
        FrameAssembler(const std::vector<std::unique_ptr<Receiver>> &rec, size_t n_fifos=1)
        : frame_numbers_(rec.size()) {
            assembled_images_.reserve(10); //TODO fix vectors of Fifos!!!
        for (size_t i = 0; i<n_fifos; i++){
            fmt::print("Creating assembled image fifo {}\n", i);
            assembled_images_.emplace_back(10, rec.size() * FRAME_SIZE);
        }

        for (int i = 0; i<assembled_images_.size(); i++){
            fmt::print("Assembled image fifo {} size: {}, filled {} free {}\n", i, assembled_images_[i].size(),
            assembled_images_[i].numFilledSlots(),assembled_images_[i].numFreeSlots());
        }
        
        for (auto &r : rec) {
            fifos_.push_back(r->fifo());
        }
        part_size_ = fifos_[0]->image_size();
    }
    ImageFifo *fifo(size_t i = 0) { return &assembled_images_[i]; }
    size_t n_fifos() { return assembled_images_.size(); };
    void stop() {
        fmt::print(fg(fmt::color::hot_pink),
                   "FrameAssembler::stop requested\n");
        stop_requested_ = true;
        while (!stopped_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void assemble(int cpu) {
        fmt::print("FrameAssembler::assemble started\n");
        pin_this_thread(cpu);
        // auto part_size = fifos_[0]->image_size();
        for (int i = 0; i<assembled_images_.size(); i++){
            fmt::print("Assembled image fifo {} size: {}, filled {} free {}\n", i, assembled_images_[i].size(),
            assembled_images_[i].numFilledSlots(),assembled_images_[i].numFreeSlots());
        }


        size_t fifo_index = 0;
        while (!stop_requested_) {
            if (fifo_index == assembled_images_.size()) {
                fifo_index = 0;
            }
            // fmt::print("Assemble trying pop free\n");
            auto full_image = assembled_images_[fifo_index].pop_free();
            // fmt::print("Assemble pop free\n");
            for (size_t i = 0; i < fifos_.size(); ++i) {
                pop_and_copy_from(i, full_image);
            }

            while (!allEqual(frame_numbers_)) {
                fmt::print(fg(fmt::color::hot_pink), "{}, {}, {}\n",
                           frame_numbers_[0], frame_numbers_[1],
                           frame_numbers_[0] - frame_numbers_[1]);
                fmt::print(fg(fmt::color::yellow_green), "{}, {}\n",
                           fifos_[0]->numFilledSlots(),
                           fifos_[1]->numFilledSlots());

                // find min pull one
                auto i = std::min_element(frame_numbers_.begin(),
                                          frame_numbers_.end()) -
                         frame_numbers_.begin();
                fmt::print("pop: {}\n", i);
                pop_and_copy_from(i, full_image);
            }

            // push to stream or writer
            full_image.frameNumber = frame_numbers_[0];
            
            assembled_images_[fifo_index].push_image(full_image);
            fifo_index += 1;
            // fmt::print("Assemple push image\n");
        }
        stopped_ = true;
        fmt::print(fg(fmt::color::hot_pink),
                   "FrameAssembler::assemble done!\n");
    }

  private:
    void pop_and_copy_from(size_t i, ImageView full_image) {
        // TODO! any performance implications from this?
        ImageView img = fifos_[i]->pop_image(DEFAULT_WAIT, stop_requested_);
        if(!stop_requested_){
            frame_numbers_[i] = img.frameNumber;
            std::copy_n(img.data, part_size_, full_image.data + i * part_size_);
            fifos_[i]->push_free(img);
        }
        
    }
};
} // namespace reuss
