#include "reuss/ImageFifo.h"
#include "reuss/ImageView.h"

#include <catch2/catch_test_macros.hpp>

using reuss::ImageFifo;
using reuss::ImageView;

TEST_CASE("Properties on an newly constructed fifo"){
    size_t fifo_size = 10;
    size_t image_size_bytes = 1024;
    ImageFifo fifo(fifo_size, image_size_bytes);
    REQUIRE(fifo.size() == fifo_size);
    REQUIRE(fifo.numFreeSlots() == fifo_size);
    REQUIRE(fifo.numFilledSlots() == 0);
}

TEST_CASE("Basic push and pop"){
    size_t fifo_size = 10;
    size_t image_size_bytes = 1024;
    ImageFifo fifo(fifo_size, image_size_bytes);
    auto image = fifo.pop_free();
    image.frameNumber = 732;
    
    REQUIRE(fifo.numFreeSlots() == fifo_size-1);
    REQUIRE(fifo.numFilledSlots() == 0);

    fifo.push_image(image);
    REQUIRE(fifo.numFreeSlots() == fifo_size-1);
    REQUIRE(fifo.numFilledSlots() == 1);

    auto image2 = fifo.pop_image();
    REQUIRE(image2.frameNumber == 732);
}

TEST_CASE("Move fifo"){
    size_t fifo_size = 10;
    size_t image_size_bytes = 1024;
    // ImageFifo fifo(fifo_size, image_size_bytes);

    ImageFifo fifo(ImageFifo(fifo_size, image_size_bytes));
    REQUIRE(fifo.size() == fifo_size);
    REQUIRE(fifo.numFreeSlots() == fifo_size);
    REQUIRE(fifo.numFilledSlots() == 0);
}

TEST_CASE("Vector of ImageFifos"){
    std::vector<ImageFifo> fifos;
    size_t fifo_size = 10;
    size_t image_size_bytes = 1024;
    for (int i = 0; i<10; i++){
        fifos.emplace_back(fifo_size, image_size_bytes);
    }
    REQUIRE(fifos.size() == 10);
    for (auto &f : fifos){
        REQUIRE(f.size() == fifo_size);
        REQUIRE(f.numFreeSlots() == fifo_size);
        REQUIRE(f.numFilledSlots() == 0);
    }
}