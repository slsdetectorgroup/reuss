#include "reuss/DataSpan.h"
#include "reuss/ImageData.h"
#include "reuss/JungfrauDetector.h"
#include <fmt/format.h>
#include <chrono>

int main(){
    using reuss::Shape;

    reuss::ImageData<float,2> data(Shape<2>{5,10});

    fmt::print("Strides: {},{}\n", data.strides()[0], data.strides()[1]);
    fmt::print("Byte strides: {},{}\n", data.byte_strides()[0], data.byte_strides()[1]);


}