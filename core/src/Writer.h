#pragma once
#include "DirectWriter.h"
#include "File.h"
#include "ImageFifo.h"
#include "ImageView.h"
#include <atomic>

namespace reuss {

class Writer {
    ImageFifo *fifo_;
    std::atomic<bool> stopped_{false};
    File<DirectWriter> file_;

  public:
    Writer(ImageFifo *fifo)
        : fifo_(fifo), file_("file", 1000, fifo_->image_size()) {}

    void write(int cpu) {
        while (!stopped_) {
            auto img = fifo_->pop_image(DEFAULT_WAIT, stopped_);
            // fmt::print("img.frameNumber {}\n", img.frameNumber);
            if (img.frameNumber != -1)
                file_.write(img);
            fifo_->push_free(img);
        }
    }
    void stop() { stopped_ = true; }
};

} // namespace reuss