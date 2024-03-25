#pragma once
#include "reuss/ImageData.h"
#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"
#include "reuss/Receiver.h"
#include "reuss/project_defs.h"
#include "reuss/utils.h"
#include "reuss/DetectorInterface.h"

#include <atomic>
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
    std::vector<ImageFifo> assembled_images_;
    std::vector<int64_t> frame_numbers_; // For equality check
    size_t part_size_ = 0;
    std::atomic<bool> pedestal_mode_{false};
    std::atomic<size_t> frames_per_queue_{50};
    DetectorInterface *det_;

  public:
    FrameAssembler(const std::vector<std::unique_ptr<Receiver>> &rec, DetectorInterface *d,
                   size_t n_fifos = 1)
        : frame_numbers_(rec.size()), det_(d) {
        assembled_images_.reserve(50); // TODO fix vectors of Fifos!!!
        for (size_t i = 0; i < n_fifos; i++) {
            fmt::print("Creating assembled image fifo {}\n", i);
            assembled_images_.emplace_back(100, rec.size() * FRAME_SIZE); //Make this depend on actual number of frames to sum
        }

        for (size_t i = 0; i < assembled_images_.size(); i++) {
            fmt::print("Assembled image fifo {} size: {}, filled {} free {}\n",
                       i, assembled_images_[i].size(),
                       assembled_images_[i].numFilledSlots(),
                       assembled_images_[i].numFreeSlots());
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

    void set_pedestal_mode(bool mode) { pedestal_mode_ = mode; }
    bool get_pedestal_mode() { return pedestal_mode_; }

    void assemble(int cpu) {
        fmt::print("FrameAssembler::assemble started\n");
        pin_this_thread(cpu);

        size_t fifo_index = 0;
        size_t frames_sent = 0;
        while (!stop_requested_) {
            if (fifo_index == assembled_images_.size()) {
                fifo_index = 0;
            }

            if ((fifo_index == 0) && pedestal_mode_) {
                record_pedestal();
            }

            auto full_image = assembled_images_[fifo_index].pop_free();
            for (size_t i = 0; i < fifos_.size(); ++i) {
                pop_and_copy_from(i, full_image);
            }

            //if frame numbers are not equal we pop from the fifo with the lowest 
            //frame number until we can make a full image
            while (!allEqual(frame_numbers_)) {
                fmt::print(fg(fmt::color::hot_pink), "{}, {}, {}\n",
                           frame_numbers_[0], frame_numbers_[1],
                           frame_numbers_[0] - frame_numbers_[1]);
                fmt::print(fg(fmt::color::yellow_green), "{}, {}\n",
                           fifos_[0]->numFilledSlots(),
                           fifos_[1]->numFilledSlots());

                // find min pop one
                auto i = std::min_element(frame_numbers_.begin(),
                                          frame_numbers_.end()) -
                         frame_numbers_.begin();
                fmt::print("pop: {}\n", i);
                pop_and_copy_from(i, full_image);
            }

            // push to next step (streamer or summer)
            full_image.frameNumber = frame_numbers_[0];
            assembled_images_[fifo_index].push_image(full_image);

            frames_sent++;
            if(frames_sent == frames_per_queue_){
                frames_sent = 0;
                fifo_index++;
            }
        }
        stopped_ = true;
        fmt::print(fg(fmt::color::hot_pink),
                   "FrameAssembler::assemble done!\n");
    }

  private:
    void pop_and_copy_from(size_t i, ImageView full_image) {
        // TODO! any performance implications from this?
        ImageView img = fifos_[i]->pop_image(DEFAULT_WAIT, stop_requested_);
        if (!stop_requested_) {
            frame_numbers_[i] = img.frameNumber;
            std::copy_n(img.data, part_size_, full_image.data + i * part_size_);
            fifos_[i]->push_free(img);
        }
    }

    void record_pedestal() {
        fmt::print("Recording pedestal\n");

        ImageData<uint16_t> raw_image{{512, COL_MAX - COL_MIN}, 0};
        ImageData<float,3> pedestal{{3, 512, COL_MAX - COL_MIN}, 0};
        ImageView full_image{1, reinterpret_cast<char *>(raw_image.buffer())};
        ImageData<int64_t> accumulate{{512, COL_MAX - COL_MIN}, 0};
        constexpr std::array<double, 3> period{-1, 0.01, 0.01};
        const auto pervious_period = det_->get_period();
        const auto pervious_gain = det_->get_gain();

        for (int gain = 0; gain < 3; ++gain) {
            accumulate = 0;
            fmt::print("Gain {}\n", gain);
            det_->stop();
            if (period[gain] > 0)
                det_->set_period(period[gain]);
            det_->set_gain(gain);
            det_->start();
            //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            int frames_caught{0};
            int n_frames = 1000;
            while ((frames_caught < n_frames) && !stopped_) {
                //---------------------------------------------------------DUPLICATED
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
                //---------------------------------------------------------DUPLICATED

                full_image.frameNumber =
                    frame_numbers_[0]; // We know from above that they are all
                                       // the same
                frames_caught++;
                fmt::print("{}:{}:Pedestal frame {}\n", gain, frames_caught,
                           full_image.frameNumber);
                auto out = accumulate.data();
                for (auto it = raw_image.begin(); it != raw_image.end(); ++it) {
                    *out++ += *it & ADC_MASK;
                }
            }
            // divide and copy
            auto dst = &pedestal(gain, 0, 0);
            for (auto it = accumulate.begin(); it != accumulate.end(); ++it) {
                *dst++ = static_cast<float>(*it) / static_cast<float>(n_frames);
            }
        }

        det_->stop();
        det_->set_gain(pervious_gain);
        det_->set_period(pervious_period);
        det_->start();
        save(pedestal, "/dev/shm/reuss/pedestal.bin"); //TODO Fix hardcoded path!
        pedestal_mode_ = false;
    }
};
} // namespace reuss
