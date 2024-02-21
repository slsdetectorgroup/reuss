#include "reuss/CollectingStreamer.h"
#include "reuss/ZmqSocket.h"
#include "reuss/utils.h"
#include <string>
namespace reuss {

CollectingStreamer::CollectingStreamer(const std::string &endpoint,
                                       std::vector<ImageFifo *> fifos)
    : fifos_(fifos), socket_(endpoint) {}

void CollectingStreamer::stream(int cpu) {
    pin_this_thread(cpu);
    while (!stop_requested_) {
        ImageView img;
        for (auto &fifo : fifos_) {
            if (fifo->try_pop_image(img)) {
                socket_.send(img, fifo->image_size());
                fifo->push_free(img);
                last_frame_ = img.frameNumber;
                total_frames_++;
            } else {
                std::this_thread::sleep_for(DEFAULT_WAIT);
            }
        }
        // if (fifo_->try_pop_image(img)) {
        //     socket_.send(img, fifo_->image_size());
        //     fifo_->push_free(img);
        //     last_frame_ = img.frameNumber;
        //     total_frames_++;
        // }else{
        //     std::this_thread::sleep_for(DEFAULT_WAIT);
        // }
    }
    stopped_ = true;
    fmt::print(fg(fmt::color::hot_pink), "CollectingStreamer::stream done!\n");
}
} // namespace reuss