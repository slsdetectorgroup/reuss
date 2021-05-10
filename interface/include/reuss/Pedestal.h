
#pragma once
#include <cstddef>
namespace reuss{


class Pedestal{
    size_t n_frames{500};

    public:
    Pedestal(){}

    void collect();
};

}