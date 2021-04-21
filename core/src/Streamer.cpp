#include "reuss/Streamer.h"
#include "reuss/ZmqSocket.h"
#include "reuss/utils.h"
#include <string>
namespace reuss {

Streamer::Streamer(const std::string &endpoint, ImageFifo *fifo)
    : fifo_(fifo), socket_(endpoint) {}

void Streamer::stream(int cpu) {
    pin_this_thread(cpu);
    while (!stopped_) {
        ImageView img;
        if (fifo_->try_pop_image(img)) {
            socket_.send(img, fifo_->image_size());
            if (img.frameNumber % PRINT_MOD == 0)
                fmt::print(fg(fmt::color::gold), "Streamed out frame {}\n",
                           img.frameNumber);
            fifo_->push_free(img);
        }
        std::this_thread::sleep_for(DEFAULT_WAIT);
    }
}
} // namespace reuss