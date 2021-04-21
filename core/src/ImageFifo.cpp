#include "reuss/ImageFifo.h"

namespace ur {

size_t ImageFifo::size() const noexcept { return fifo_size_; }
size_t ImageFifo::image_size() const noexcept { return image_size_; }
size_t ImageFifo::numFilledSlots() const noexcept {
    return filled_slots.sizeGuess();
}
size_t ImageFifo::numFreeSlots() const noexcept {
    return free_slots.sizeGuess();
}

ImageFifo::ImageFifo(size_t fifo_size, size_t image_size)
    : fifo_size_(fifo_size), image_size_(image_size), free_slots(fifo_size_),
      filled_slots(fifo_size_) {

    posix_memalign(reinterpret_cast<void **>(&data), IO_ALIGNMENT,
                   fifo_size_ * image_size_);

    ImageView v;
    for (size_t i = 0; i < fifo_size_; ++i) {
        v.data = data + i * image_size_;
        free_slots.push(v);
    }
}

ImageView ImageFifo::pop_free() {
    ImageView v;
    while (!free_slots.pop(v))
        ;
    return v;
}

ImageView ImageFifo::pop_image() {
    ImageView v;
    while (!filled_slots.pop(v))
        ;
    return v;
}

ImageView ImageFifo::pop_image(std::chrono::nanoseconds wait) {
    ImageView v;
    while (!filled_slots.pop(v)) {
        std::this_thread::sleep_for(wait);
    }
    return v;
}

ImageView ImageFifo::pop_image(std::chrono::nanoseconds wait,
                               std::atomic<bool> &stopped) {
    ImageView v;
    while (!filled_slots.pop(v) && !stopped) {
        std::this_thread::sleep_for(wait);
    }
    if (stopped)
        fmt::print(fg(fmt::color::rosy_brown), "STOPPED!\n");
    return v;
}

bool ImageFifo::try_pop_image(ImageView &img) { return filled_slots.pop(img); }

void ImageFifo::push_image(ImageView &v) {
    while (!filled_slots.push(v))
        ;
}

void ImageFifo::push_free(ImageView &v) {
    while (!free_slots.push(v))
        ;
}

} // namespace ur