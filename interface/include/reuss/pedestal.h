#pragma once
#include "reuss/DataSpan.h"
#include "reuss/ImageData.h"
#include "reuss/DetectorInterface.h"
#include "reuss/ZmqReceiver.h"
#include "reuss/project_defs.h"
#include <cstddef>
#include <fmt/core.h>
#include <vector>

namespace reuss {

template <typename T> ImageData<T, 3> take_pedestal(DetectorInterface *det,
int n_frames) {
    ImageData<T, 3> pedestal({3, 512, COL_MAX-COL_MIN}, 0);
    take_pedestal<T>(pedestal.span(), det, n_frames);
    return pedestal;
}

template <typename T>
void take_pedestal(DataSpan<T, 3> pedestal, DetectorInterface *det,
                   int n_frames) {
    ZmqReceiver r{DEFAULT_RECEIVE};
    ImageData<uint16_t> buffer{{512, COL_MAX-COL_MIN},
                               0}; // todo need to allow configuration!!!

    ImageData<int64_t> accumulate{{512, COL_MAX-COL_MIN},
                               0};
    
    constexpr std::array<double, 3> period{-1, 0.01, 0.01};

    const auto pervious_period = det->get_period();
    const auto pervious_gain = det->get_gain();

    for (int gain = 0; gain < 3; ++gain) {
        accumulate = 0;
        fmt::print("Gain {}\n", gain);
        det->stop();
        if (period[gain] > 0)
            det->set_period(period[gain]);
        det->set_gain(gain);
        det->start();

        r.connect();
        int frames_caught{0};
        int64_t frame_number;
        while (frames_caught < n_frames) {
            int fc = r.receive_into(1, &frame_number, buffer.buffer());
            if (fc!= 1){
                fmt::print("Got: {} frames\n", fc);
            }
            frames_caught += fc;
            // auto out = &pedestal(gain, 0, 0);
            auto out = accumulate.data();
            for (auto it = buffer.begin(); it != buffer.end(); ++it) {
                // *out++ += static_cast<T>(*it & ADC_MASK);
                *out++ += *it & ADC_MASK;
            }
        }
        r.disconnect();

        //divide and copy
        auto dst = &pedestal(gain, 0, 0);
        for (auto it = accumulate.begin(); it != accumulate.end(); ++it)
            *dst++ = *it / static_cast<T>(n_frames);
    }

    det->stop();
    det->set_gain(pervious_gain);
    det->set_period(pervious_period);
    det->start();


    // for (auto it = pedestal.begin(); it != pedestal.end(); ++it)
    //     *it = *it / static_cast<T>(n_frames);
}

} // namespace reuss