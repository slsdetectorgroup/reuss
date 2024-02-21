#pragma once
#include "reuss/ImageView.h"
#include "reuss/SimpleQueue.h"
#include "reuss/project_defs.h"

#include <chrono>
#include <fmt/color.h>
#include <fmt/format.h>
#include <memory>
#include <thread>
namespace reuss {

class ImageFifo {
    size_t fifo_size_;
    size_t image_size_;
    SimpleQueue<ImageView> free_slots;
    SimpleQueue<ImageView> filled_slots;
    char *data;

  public:
    ImageFifo(size_t fifo_size, size_t image_size);
    ImageFifo(const ImageFifo &) = delete;
    ImageFifo &operator=(const ImageFifo &) = delete;
    ImageFifo(ImageFifo &&other){
        fmt::print("Move constructor\n");
        fifo_size_ = other.fifo_size_;
        image_size_ = other.image_size_;
        data = other.data;
        other.data = nullptr;
        free_slots = std::move(other.free_slots);
        filled_slots = std::move(other.filled_slots);
    }
    ~ImageFifo() { free(data); }

    size_t size() const noexcept;
    size_t image_size() const noexcept;
    size_t numFilledSlots() const noexcept;
    size_t numFreeSlots() const noexcept;

    ImageView pop_free();
    ImageView pop_image();
    ImageView pop_image(std::chrono::nanoseconds wait);
    ImageView pop_image(std::chrono::nanoseconds wait,
                        std::atomic<bool> &stopped);
    bool try_pop_image(ImageView &img);
    void push_image(ImageView &v);
    void push_free(ImageView &v);
};
} // namespace reuss