#pragma once

#include "reuss/ImageData.h"
#include "reuss/conversion_helper.h"

#include <chrono>
#include <fmt/color.h>
#include <fmt/core.h>

namespace reuss {

template <typename T> class FrameSummer {

    // {{512, COL_MAX - COL_MIN}, 0.0}
    ImageData<T, 3> pedestal_{{3, 512, COL_MAX - COL_MIN}, 0.0};
    ImageData<T, 3> calibration_{{3, 512, COL_MAX - COL_MIN}, 1.0};
    ImageData<uint16_t> buffer_{{512, COL_MAX - COL_MIN}, 0};
    T threshold_{-5000}; // add everything
    std::atomic<bool> stopped_{false};
    std::atomic<int> frames_to_sum_{5};

    ImageFifo *raw_fifo_; // Read frames from the receiver
    ImageFifo summed_fifo_{100, IMAGE_SIZE.rows *IMAGE_SIZE.cols * sizeof(T)};

  public:
    FrameSummer(ImageFifo *fifo) : raw_fifo_(fifo) {}
    ~FrameSummer() {}
    void set_threshold(T th) { threshold_ = th; }
    T get_threshold() const noexcept { return threshold_; }
    void set_calibration(ImageData<T, 3> cal) { calibration_ = std::move(cal); }
    void stop() { stopped_ = true; }
    ImageData<T, 3> get_calibration() const { return calibration_; }

    void set_pedestal(ImageData<T, 3> pd) { pedestal_ = std::move(pd); }
    ImageData<T, 3> get_pedestal() const { return pedestal_; }
    ImageFifo *fifo() { return &summed_fifo_; }

    void set_frames_to_sum(int n) { frames_to_sum_ = n; }
    int get_frames_to_sum() const { return frames_to_sum_; }

    void accumulate(int cpu) {
        fmt::print("Summed fifo size: {}\n", summed_fifo_.size());
        pin_this_thread(cpu);
        
        int summed_frames = 0;
        std::array<ssize_t, 2> shape{IMAGE_SIZE.rows, IMAGE_SIZE.cols};
        while (!stopped_) {
            ImageView summed_image = summed_fifo_.pop_free();
            DataSpan<T, 2> summed_span(reinterpret_cast<T *>(summed_image.data),
                                       shape);
            summed_span = 0;
            int n_frames = frames_to_sum_;
            while (summed_frames < n_frames) {
                ImageView raw_image;

                // fmt::print("Trying to pop image\n");
                if (raw_fifo_->try_pop_image(raw_image)) {
                    fmt::print("Summing receiver got frame {}\n",
                               raw_image.frameNumber);
                    DataSpan<uint16_t, 2> raw_span(
                        reinterpret_cast<uint16_t *>(raw_image.data), shape);
                    convert_and_add(raw_span, summed_span);

                    raw_fifo_->push_free(raw_image);
                    summed_image.frameNumber = raw_image.frameNumber;
                    summed_frames++;
                }
            }
            fmt::print("Pushing summed frame {}\n", summed_image.frameNumber);
            summed_fifo_.push_image(summed_image);
            summed_frames = 0;
            // last_frame_ = raw_image.frameNumber;
            // total_frames_++;
        }
        std::this_thread::sleep_for(DEFAULT_WAIT);
    }

  private:
    void convert_and_add(const DataSpan<uint16_t, 2> src, DataSpan<T, 2> dst) {
        for (int row = 0; row != src.shape(0); ++row) {
            for (int col = 0; col != src.shape(1); ++col) {
                uint16_t value = src(row, col);
                auto gain = get_gain(value);
                value &= ADC_MASK;

                T e_val = (value - pedestal_(gain, row, col)) /
                          calibration_(gain, row, col);

                // if (row == 0 && col == 0)
                    // fmt::print("value: {}, cal: {} pd: {} e_val: {}\n", value,
                    //            calibration_(gain, row, col),
                    //            pedestal_(gain, row, col), e_val);

                if (e_val > threshold_)
                    dst(row, col) += e_val;
            }
        }
    }
};

} // namespace reuss