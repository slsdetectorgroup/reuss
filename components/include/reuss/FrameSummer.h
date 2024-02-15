#pragma once

#include "reuss/ImageData.h"
#include "reuss/ImageFiFo.h"
#include "reuss/ZmqReceiver.h"
#include "reuss/conversion_helper.h"

#include <chrono>
#include <fmt/core.h>
#include <fmt/color.h>
namespace reuss {

template <typename T> class FrameSummer {

    ImageData<T, 3> pedestal_{};
    ImageData<T, 3> calibration_{};
    ImageData<uint16_t> buffer_{{512, COL_MAX - COL_MIN}, 0};

    ImageFifo summed_images_{100, 512*1024*sizeof(T)};
    ImageFifo* raw_fifo_;
    T threshold_{-5000}; // add everything


  public:
    FrameSummer(ImageFifo* raw):raw_fifo_(raw) {}
    ~FrameSummer() {}

    void set_threshold(T th) { threshold_ = th; }
    T get_threshold() const noexcept { return threshold_; }

    void set_calibration(ImageData<T, 3> cal) { calibration_ = std::move(cal); }
    ImageData<T, 3> get_calibration() const { return calibration_; }

    void set_pedestal(ImageData<T, 3> pd) { pedestal_ = std::move(pd); }
    ImageData<T, 3> get_pedestal() const { return pedestal_; }

    void sum(int n_frames) {
        ImageView image = summed_images_->pop_free(DEFAULT_WAIT, stopped_);
        int64_t previous_frame_number{-1};
        // auto t0 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n_frames; ++i) {
            // r.receive_into(1, &frame_number, buffer_.buffer());
            ImageView raw = raw_fifo_->pop_image(DEFAULT_WAIT, stopped_);
            // convert_and_add(buffer_.span(), image.span());
            int64_t diff = raw.frameNumber - previous_frame_number;
            if (previous_frame_number > 0 && diff != 1)
            fmt::print(fg(fmt::color::red),
                       "current: {} last: {} Lost {} frames\n", frame_number,
                       previous_frame_number, diff);
            previous_frame_number = raw.frameNumber;
            raw_fifo_->push_free(raw);
        }
        summed_images_->push_image(image);
        // auto t1 = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double> t(t1 - t0);
        // return image;
    }

  private:
    void convert_and_add(const DataSpan<uint16_t, 2> src,
                         DataSpan<T, 2> dst) {
        for (int row = 0; row != src.shape(0); ++row) {
            for (int col = 0; col != src.shape(1); ++col) {
                uint16_t value = src(row, col);
                auto gain = get_gain(value);
                value &= ADC_MASK;
                T e_val = (value - pedestal_(gain, row, col)) /
                          calibration_(gain, row, col);

                if (e_val > threshold_)
                    dst(row, col) += e_val;
            }
        }
    }
};

} // namespace reuss