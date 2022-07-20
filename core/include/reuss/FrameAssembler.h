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
    ImageFifo assembled_images_;
    std::vector<int64_t> frame_numbers_; // For equality check
    size_t part_size_ = 0;

  public:
    FrameAssembler(const std::vector<std::unique_ptr<Receiver>> &rec)
        : assembled_images_(10, rec.size() * FRAME_SIZE),
          frame_numbers_(rec.size()) {
        for (auto &r : rec) {
            fifos_.push_back(r->fifo());
        }
        part_size_ = fifos_[0]->image_size();
    }
    ImageFifo *fifo() { return &assembled_images_; }
    void stop() { stopped_ = true; }

    void assemble(int cpu) {
        pin_this_thread(cpu);
        // auto part_size = fifos_[0]->image_size();

        while (!stopped_) {
            auto full_image = assembled_images_.pop_free();


            for (size_t i = 0; i < fifos_.size(); ++i) {
                pop_and_copy_from(i, full_image);
                // ImageView img = fifos_[i]->pop_image(DEFAULT_WAIT, stopped_);
                // if (stopped_)
                //     break;
                // frame_numbers_[i] = img.frameNumber;

                // //copy full image 
                // std::copy_n(img.data, part_size,
                //             full_image.data + i * part_size);


                // fifos_[i]->push_free(img);
            }


            while(!allEqual(frame_numbers_)){
                fmt::print(fg(fmt::color::hot_pink),
                           "{}, {}, {}\n",
                           frame_numbers_[0], frame_numbers_[1], frame_numbers_[0]-frame_numbers_[1]);
                fmt::print(fg(fmt::color::yellow_green),
                           "{}, {}\n",
                           fifos_[0]->numFilledSlots(), fifos_[1]->numFilledSlots());

                //find min pull one
                auto i = std::min_element(frame_numbers_.begin(), frame_numbers_.end()) - frame_numbers_.begin();
                fmt::print("pop: {}\n", i);
                pop_and_copy_from(i, full_image);
                // ImageView img = fifos_[i]->pop_image(DEFAULT_WAIT, stopped_);
                // if (stopped_)
                //     break;
                // frame_numbers_[i] = img.frameNumber;

                // //copy full image 
                // std::copy_n(img.data, part_size,
                //             full_image.data + i * part_size);


                // fifos_[i]->push_free(img);
            }

            // push to stream or writer
            full_image.frameNumber = frame_numbers_[0];
            assembled_images_.push_image(full_image);

            // if (full_image.frameNumber % PRINT_MOD == 0)
            //     fmt::print(fg(fmt::color::hot_pink),
            //                "Assembled frame {}, frames in fifo: {}\n",
            //                frame_numbers_[0], fifos_[0]->numFilledSlots());
        }
    }

private:
    void pop_and_copy_from(size_t i, ImageView full_image){
        //TODO! any performance implications from this?
        ImageView img = fifos_[i]->pop_image(DEFAULT_WAIT, stopped_);
                frame_numbers_[i] = img.frameNumber;
                std::copy_n(img.data, part_size_,
                            full_image.data + i * part_size_);
                fifos_[i]->push_free(img);
    }
};
} // namespace reuss
