#include "reuss/Streamer.h"
#include "reuss/ZmqSocket.h"
#include "reuss/utils.h"
#include <string>
namespace reuss {

Streamer::Streamer(const std::string &endpoint, ImageFifo *fifo)
    : fifo_(fifo), socket_(endpoint) {}

void Streamer::stream(int cpu) {
    pin_this_thread(cpu);
    while (!stop_requested_) {
        ImageView img;
        if (fifo_->try_pop_image(img)) {
            socket_.send(img, fifo_->image_size());
            fifo_->push_free(img);
            last_frame_ = img.frameNumber;
            total_frames_++;
        }else{
            std::this_thread::sleep_for(DEFAULT_WAIT);
        }
    }
    stopped_ = true;
    fmt::print(fg(fmt::color::hot_pink), "Streamer::stream done!\n");
}
} // namespace reuss