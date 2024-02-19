#pragma once

#include "reuss/ImageData.h"
#include "reuss/ZmqReceiver.h"
#include "reuss/conversion_helper.h"

#include <chrono>
#include <fmt/core.h>
#include <fmt/color.h>
namespace reuss {

template <typename T> class FrameAccumulator {

    ImageData<T, 3> pedestal_{{3, 512, COL_MAX - COL_MIN}, 0};
    ImageData<T, 3> calibration_{{3, 512, COL_MAX - COL_MIN}, 1};
    ImageData<uint16_t> buffer_{{512, COL_MAX - COL_MIN}, 0};
    T threshold_{-5000}; // add everything
    ZmqReceiver r{DEFAULT_RECEIVE};

  public:
    FrameAccumulator() { 
        r.set_zmq_hwm(1);
        r.connect(); 
        
    }

    ~FrameAccumulator() { r.disconnect(); }
    void set_threshold(T th) { threshold_ = th; }
    T get_threshold() const noexcept { return threshold_; }

    void set_calibration(ImageData<T, 3> cal) { calibration_ = std::move(cal); }
    ImageData<T, 3> get_calibration() const { return calibration_; }

    void set_pedestal(ImageData<T, 3> pd) { pedestal_ = std::move(pd); }
    ImageData<T, 3> get_pedestal() const { return pedestal_; }

    ImageData<T, 2> accumulate(int n_frames) {
        ImageData<T, 2> image{{512, COL_MAX - COL_MIN}, 0};
        int64_t frame_number{};
        int64_t previous_frame_number{-1};
        // auto t0 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n_frames; ++i) {
            r.receive_into(1, &frame_number, buffer_.buffer());
            convert_and_add(buffer_.span(), image.span());
            int64_t diff = frame_number - previous_frame_number;
            if (previous_frame_number > 0 && diff != 1)
            fmt::print(fg(fmt::color::red),
                       "current: {} last: {} Lost {} frames\n", frame_number,
                       previous_frame_number, diff);
            previous_frame_number = frame_number;
        }
        // auto t1 = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double> t(t1 - t0);
        return image;
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