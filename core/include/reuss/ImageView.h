#pragma once
#include <cstddef>
#include <cstdint>
namespace reuss {
struct ImageView {
    // ImageView() {}
    // ImageView(int64_t frameNumber, char *data)
    //     : frameNumber(frameNumber), data(data) {}
    int64_t frameNumber{-1};
    int64_t framesInPack{1};
    char *data{nullptr};
};
} // namespace reuss