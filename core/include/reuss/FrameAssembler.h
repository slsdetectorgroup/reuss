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

  public:
    FrameAssembler(const std::vector<std::unique_ptr<Receiver>> &rec)
        : assembled_images_(10, rec.size() * FRAME_SIZE),
          frame_numbers_(rec.size()) {
        for (auto &r : rec) {
            fifos_.push_back(r->fifo());
        }
    }
    ImageFifo *fifo() { return &assembled_images_; }
    void stop() { stopped_ = true; }

    void assemble(int cpu) {
        pin_this_thread(cpu);
        auto part_size = fifos_[0]->image_size();

        while (!stopped_) {
            auto full_image = assembled_images_.pop_free();
            for (size_t i = 0; i < fifos_.size(); ++i) {
                ImageView img = fifos_[i]->pop_image(DEFAULT_WAIT, stopped_);
                if (stopped_)
                    break;
                frame_numbers_[i] = img.frameNumber;

                //copy full image 
                std::copy_n(img.data, part_size,
                            full_image.data + i * part_size);

                // //copy per row 
                // for (size_t j=0; j<NROW; ++j){
                //     std::copy_n(img.data+j*NCOL,NCOL, full_image.data +  );
                // }


                fifos_[i]->push_free(img);
            }

            if (!allEqual(frame_numbers_))
                throw std::runtime_error(
                    "Frame numbers in assembled image differ!");

            // push to stream or writer
            full_image.frameNumber = frame_numbers_[0];
            assembled_images_.push_image(full_image);

            if (full_image.frameNumber % PRINT_MOD == 0)
                fmt::print(fg(fmt::color::hot_pink),
                           "Assembled frame {}, frames in fifo: {}\n",
                           frame_numbers_[0], fifos_[0]->numFilledSlots());
        }
    }
};
} // namespace reuss
