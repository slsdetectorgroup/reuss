

#include "reuss/ImageData.h"
#include "reuss/conversion_helper.h"
#include "reuss/file_io.h"
#include "reuss/project_defs.h"
#include <chrono>
#include <fmt/format.h>

template <typename T>
void convert_and_add(const reuss::DataSpan<uint16_t, 2> src,
                     reuss::DataSpan<T, 2> dst, reuss::DataSpan<T, 3> pedestal,
                     reuss::DataSpan<T, 3> calibration, T threshold) {

    for (int row = 0; row != src.shape(0); ++row) {
        for (int col = 0; col != src.shape(1); ++col) {
            uint16_t value = src(row, col);
            auto gain = get_gain(value);
            value &= ADC_MASK;

            T e_val = (value - pedestal(gain, row, col)) /
                      calibration(gain, row, col);

            // if (row == 500 && col == 500)
            //     fmt::print("value: {}, gain: {} cal: {} pd: {} e_val:
            //     {}\n", value,gain,
            //                calibration_(gain, row, col),
            //                pedestal_(gain, row, col), e_val);

            if (e_val > threshold)
                dst(row, col) += e_val;
        }
    }
}

int main() {
    fmt::print("Hello, world!\n");

    auto t0 = std::chrono::high_resolution_clock::now();
    auto data = reuss::load_raw_bin<uint16_t>(
        "/home/l_msdetect/erik/epoc/data/000_21-Feb-2024_084312/file_0.bin");
    auto t1 = std::chrono::high_resolution_clock::now();
    auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    fmt::print("Loaded data in {} ms {:.3f} GB/s\n", ms,
               data.size() * 2 / (1024.0 * 1024.0 * ms));

    auto pd =
        reuss::load_numpy<float, 3>("/home/l_msdetect/erik/epoc/data/"
                                    "000_21-Feb-2024_084312/pedestal_0.npy");
    auto cal0 =
        reuss::load_numpy<double, 3>("/home/l_msdetect/erik/epoc/data/"
                                     "000_21-Feb-2024_084312/calibration.npy");
    reuss::ImageData<float, 3> cal(cal0.shape());
    for (int i = 0; i < cal0.size(); i++) {
        cal(i) = cal0(i);
    }

    reuss::ImageData<float, 3> summed_data{{500, 512, 1024}};
    // auto src = reuss::DataSpan<uint16_t, 2>(data.data(),
    //                                         std::array<ssize_t, 2>{512,
    //                                         1024});
    // auto dst = reuss::DataSpan<float, 2>(summed_data.data(),
    //                                      std::array<ssize_t, 2>{512, 1024});
    ssize_t frame_size = 512 * 1024 * 2;
    ssize_t summed_frame_size = 512 * 1024 * 4;
    size_t n_packs = 20;
    size_t pack_size = 100;
    size_t frames = 5000;
    auto t2 = std::chrono::high_resolution_clock::now();
    float threshold = -5000;
    for (int frame = 0; frame < frames; frame++) {
        for (int row = 0; row != data.shape(1); ++row) {
            for (int col = 0; col != data.shape(2); ++col) {
                uint16_t value = data(frame, row, col);
                auto gain = get_gain(value);
                value &= ADC_MASK;

                float e_val = (value - pd(gain, row, col)) /
                          cal(gain, row, col);

                if (e_val > threshold)
                    summed_data(frame/pack_size, row, col) += e_val;
            }
        }
    }

    auto t3 = std::chrono::high_resolution_clock::now();

    // auto frames = n_packs * pack_size;
    auto t =
        std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count();
    auto fps = frames / (t / 1000.0);

    fmt::print("Processed {} frames in {} ms {:.2f} FPS\n", frames, t, fps);
    // convert_and_add<float>(src, dst, pd.span(), cal.span(), -5000);
    fmt::print("pixel 0,0: {}\n", summed_data(0, 0, 0));

    return 0;
}