#include "reuss/DataSpan.h"
#include "reuss/JungfrauDetector.h"
#include <fmt/format.h>
#include <chrono>

int main(){
    using reuss::Shape;
    Shape<3> shape = {3,50,50};
    reuss::DataSpan<int, 3> span{nullptr, shape};

    fmt::print("sizeof(DataSpan): {}\n", sizeof(span));

    reuss::JungfrauDetector d;
    double period = d.get_period();

    fmt::print("period: {}\n", period);

    fmt::print("gain: {}\n", d.get_gain());

}