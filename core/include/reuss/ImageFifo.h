#pragma once
#include "reuss/ImageView.h"
#include "reuss/SimpleQueue.h"
#include "reuss/project_defs.h"

#include <chrono>
#include <fmt/color.h>
#include <fmt/format.h>
#include <memory>
#include <thread>
namespace ur {

class ImageFifo {
    size_t fifo_size_;
    size_t image_size_;
    SimpleQueue<ImageView> free_slots;
    SimpleQueue<ImageView> filled_slots;
    char *data;

  public:
    ImageFifo(size_t fifo_size, size_t image_size);
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
} // namespace ur