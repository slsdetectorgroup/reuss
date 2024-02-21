#pragma once

#include "reuss/ImageData.h"
#include "reuss/conversion_helper.h"

#include "reuss/DetectorInterface.h"
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
    std::atomic<bool> stop_requested_{false};
    std::atomic<int> frames_to_sum_{5};
    std::atomic<bool> pedestal_mode_{false};

    ImageFifo *raw_fifo_; // Read frames from the receiver
    ImageFifo summed_fifo_{100, IMAGE_SIZE.rows *IMAGE_SIZE.cols * sizeof(T)};
    DetectorInterface *det_;

  public:
    FrameSummer(ImageFifo *fifo, DetectorInterface *d)
        : raw_fifo_(fifo), det_(d) {
            fmt::print("FrameSummer created\n");
        }
    ~FrameSummer() {}
    void set_threshold(T th) { threshold_ = th; }
    T get_threshold() const noexcept { return threshold_; }
    void set_calibration(ImageData<T, 3> cal) { calibration_ = std::move(cal); }
    void stop() {
        fmt::print(fg(fmt::color::hot_pink), "FrameSummer::stop requested\n");
        stop_requested_ = true;
        while (!stopped_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    ImageData<T, 3> get_calibration() const { return calibration_; }

    void set_pedestal(ImageData<T, 3> pd) { pedestal_ = std::move(pd); }
    ImageData<T, 3> get_pedestal() const { return pedestal_; }
    void set_pedestal_mode(bool mode) { pedestal_mode_ = mode; }
    bool get_pedestal_mode() const { return pedestal_mode_; }
    ImageFifo *fifo() { return &summed_fifo_; }

    void set_frames_to_sum(int n) { frames_to_sum_ = n; }
    int get_frames_to_sum() const { return frames_to_sum_; }

    void accumulate(int cpu) {
        fmt::print("Summed fifo size: {}\n", summed_fifo_.size());
        pin_this_thread(cpu);

        int summed_frames = 0;
        std::array<ssize_t, 2> shape{IMAGE_SIZE.rows, IMAGE_SIZE.cols};
        int64_t last_frame = -1;
        while (!stop_requested_) {
            if (pedestal_mode_) {
                record_pedestal();
            }

            ImageView summed_image = summed_fifo_.pop_free();
            // fmt::print("Summer popped free\n");
            DataSpan<T, 2> summed_span(reinterpret_cast<T *>(summed_image.data),
                                       shape);
            summed_span = 0;
            int n_frames = frames_to_sum_;
            while ((summed_frames < n_frames) && !stop_requested_) {
                ImageView raw_image;
                if (raw_fifo_->try_pop_image(raw_image)) {
                    // fmt::print("Summer popped raw\n");
                    if ((last_frame != -1) &&
                        (raw_image.frameNumber - last_frame != 1))
                        fmt::print(fg(fmt::color::red),
                                   "current: {} last: {} Lost {} frames\n",
                                   raw_image.frameNumber, last_frame,
                                   raw_image.frameNumber - last_frame);
                    DataSpan<uint16_t, 2> raw_span(
                        reinterpret_cast<uint16_t *>(raw_image.data), shape);
                    convert_and_add(raw_span, summed_span);

                    raw_fifo_->push_free(raw_image);
                    summed_image.frameNumber = raw_image.frameNumber;
                    summed_frames++;
                    last_frame = raw_image.frameNumber;
                }else{
                    std::this_thread::sleep_for(DEFAULT_WAIT);
                }
            }
            summed_fifo_.push_image(summed_image);
            summed_frames = 0;
            // last_frame_ = raw_image.frameNumber;
            // total_frames_++;
        }
        stopped_ = true;
        fmt::print(fg(fmt::color::hot_pink), "FrameSummer::accumulate done!\n");
    }

  private:
    void record_pedestal() {
        fmt::print("Recording pedestal\n");
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
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            int frames_caught{0};
            int64_t frame_number;
            int n_frames = 1000;
            std::array<ssize_t, 2> shape{IMAGE_SIZE.rows, IMAGE_SIZE.cols};
            while ((frames_caught < n_frames) && !stopped_) {
                ImageView raw_image;
                if (raw_fifo_->try_pop_image(raw_image)) {
                    // fmt::print("Pedestal frame {}\n", raw_image.frameNumber);
                    DataSpan<uint16_t, 2> raw_span(
                        reinterpret_cast<uint16_t *>(raw_image.data), shape);
                    frames_caught++;
                    auto out = accumulate.data();
                    for (auto it = raw_span.begin(); it != raw_span.end();
                         ++it) {
                        *out++ += *it & ADC_MASK;
                    }
                    raw_fifo_->push_free(raw_image);
                }
            }
            // divide and copy
            auto dst = &pedestal_(gain, 0, 0);
            for (auto it = accumulate.begin(); it != accumulate.end(); ++it)
                *dst++ = static_cast<T>(*it) / static_cast<T>(n_frames);
        }
        det_->stop();
        det_->set_gain(pervious_gain);
        det_->set_period(pervious_period);
        det_->start();
        pedestal_mode_ = false;
    }
    void convert_and_add(const DataSpan<uint16_t, 2> src, DataSpan<T, 2> dst) {
        for (int row = 0; row != src.shape(0); ++row) {
            for (int col = 0; col != src.shape(1); ++col) {
                uint16_t value = src(row, col);
                auto gain = get_gain(value);
                value &= ADC_MASK;

                T e_val = (value - pedestal_(gain, row, col)) /
                          calibration_(gain, row, col);

                // if (row == 500 && col == 500)
                //     fmt::print("value: {}, gain: {} cal: {} pd: {} e_val:
                //     {}\n", value,gain,
                //                calibration_(gain, row, col),
                //                pedestal_(gain, row, col), e_val);

                if (e_val > threshold_)
                    dst(row, col) += e_val;
            }
        }
    }
};

} // namespace reuss