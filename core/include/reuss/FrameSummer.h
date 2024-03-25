#pragma once

#include "reuss/ImageData.h"
#include "reuss/conversion_helper.h"

#include "reuss/DetectorInterface.h"
#include <chrono>
#include <fmt/color.h>
#include <fmt/core.h>
namespace reuss {

template <typename T> class FrameSummer {
    ImageData<T, 3> pedestal_{{3, 512, COL_MAX - COL_MIN}, 0.0};
    ImageData<T, 3> calibration_{{3, 512, COL_MAX - COL_MIN}, 1.0};
    ImageData<uint16_t> buffer_{{512, COL_MAX - COL_MIN}, 0};
    std::atomic<T> threshold_{-5000.0}; // add everything
    std::atomic<bool> stopped_{false};
    std::atomic<bool> stop_requested_{false};
    std::atomic<int> frames_to_sum_{5};
    
    std::atomic<bool> pedestal_update_{false};

    ImageFifo *raw_fifo_; // Read frames from the receiver
    ImageFifo summed_fifo_{100, IMAGE_SIZE.rows *IMAGE_SIZE.cols * sizeof(T)};
    DetectorInterface *det_;
    std::atomic<int> total_summing_threads_{};

  public:
    FrameSummer(ImageFifo *fifo, DetectorInterface *d, int total_summing_threads)
        : raw_fifo_(fifo), det_(d), total_summing_threads_(total_summing_threads) {
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
    ImageFifo *fifo() { return &summed_fifo_; }

    void set_frames_to_sum(int n) { frames_to_sum_ = n; }
    int get_frames_to_sum() const { return frames_to_sum_; }
    void update_pedestal(){
        pedestal_update_ = true;
    }
    void accumulate(int cpu) {
        fmt::print("Summed fifo size: {}\n", summed_fifo_.size());
        
        // fmt::print("Step: {}\n", step);
        pin_this_thread(cpu);

        int summed_frames = 0;
        std::array<ssize_t, 2> shape{IMAGE_SIZE.rows, IMAGE_SIZE.cols};
        int64_t last_frame = -1;
        while (!stop_requested_) {
            if(pedestal_update_){
                fmt::print("Updating pedestal\n");
                pedestal_ = load<T,3>("/dev/shm/reuss/pedestal.bin", PEDESTAL_SHAPE);
                pedestal_update_ = false;
            }

            ImageView summed_image = summed_fifo_.pop_free();
            DataSpan<T, 2> summed_span(reinterpret_cast<T *>(summed_image.data),
                                       shape);
            summed_span = 0;
            int n_frames = frames_to_sum_;
            int step = (total_summing_threads_-1)*n_frames+1;
            while ((summed_frames < n_frames) && !stop_requested_) {
                ImageView raw_image;
                if (raw_fifo_->try_pop_image(raw_image)) {
                    if ((last_frame != -1) &&
                        (raw_image.frameNumber - last_frame != 1) &&
                        (raw_image.frameNumber - last_frame != step)
                        )
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
        }
        stopped_ = true;
        fmt::print(fg(fmt::color::hot_pink), "FrameSummer::accumulate done!\n");
    }

  private:
    void convert_and_add(const DataSpan<uint16_t, 2> src, DataSpan<T, 2> dst) {
        T th = threshold_;
        T tracking_factor = static_cast<T>(0.001); // avoid warnings when using float
        T tracking_threshold = static_cast<T>(2.0);
        for (int row = 0; row != src.shape(0); ++row) {
            for (int col = 0; col != src.shape(1); ++col) {
                uint16_t value = src(row, col);
                auto gain = get_gain(value);
                value &= ADC_MASK;

                T diff = value - pedestal_(gain, row, col);
                T e_val = diff / calibration_(gain, row, col);

                if (e_val > th)
                    dst(row, col) += e_val;

                //update pedestal if e_val is less than 2 keV
                if((gain == 0) && (e_val < tracking_threshold)){
                    pedestal_(gain, row, col) += diff*tracking_factor;
                }
            }
        }
    }
};

} // namespace reuss