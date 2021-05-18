#pragma once

#include "reuss/ImageData.h"
#include "reuss/ZmqReceiver.h"

namespace reuss {

template <typename T> class FrameAccumulator {

    ImageData<T, 3> pedestal_{};
    ImageData<T, 3> calibration_{};

  public:
    void set_calibration(ImageData<T, 3> cal) { calibration_ = std::move(cal); }

    void set_pedestal(ImageData<T, 3> pd) { pedestal_ = std::move(pd); }

    ImageData<T, 2> accumulate(int n_frames) {
        ZmqReceiver r{DEFAULT_RECEIVE};
        r.connect();
        ImageData<uint16_t> buffer{{512, COL_MAX - COL_MIN}, 0};
        int64_t fn{};
        r.receive_into(1, &fn, buffer.buffer());
        r.disconnect();


        ImageData<T,2> data(buffer.shape());
        // data = buffer;
        for(int i = 0; i<buffer.size(); ++i){
            data(i) = buffer(i);
        }

        return data;
    }
};

} // namespace reuss