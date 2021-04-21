#pragma once
#include <cstddef>
#include <cstdint>
namespace ur {
struct ImageView {
    // ImageView() {}
    // ImageView(int64_t frameNumber, char *data)
    //     : frameNumber(frameNumber), data(data) {}
    int64_t frameNumber{-1};
    char *data{nullptr};
};
} // namespace ur