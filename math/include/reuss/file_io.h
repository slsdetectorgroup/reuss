#pragma once

#include <array>
#include <chrono>
#include <fmt/core.h>
#include <fstream>
#include <map>
#include <reuss/ImageData.h>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
// #include <string>
#include <typeindex>
#include <typeinfo>

#include "reuss/DataType.h"
#include "reuss/NumpyFileHeader.h"
namespace reuss {


template <typename T, size_t Ndim>
ImageData<T, Ndim> load_numpy(const std::string &fname) {
    std::ifstream f(fname, std::ios::binary);
    if (!f)
        throw std::runtime_error("File not found");

    auto header = NumpyFileHeader::fromFile(f);
    if (header.ndim() != Ndim) {
        throw std::runtime_error("Cannot load file, dimensions of ImageData "
                                 "and numpy array does not match");
    }
    if(DataType(header.dtype()) != typeid(T)){
        throw std::runtime_error("Types does not match");
    }

    Shape<Ndim> shape{};
    auto vec = header.shape();
    std::copy(vec.begin(), vec.end(), shape.begin());

    ImageData<T, Ndim> data{shape};
    f.read(reinterpret_cast<char *>(data.buffer()),
           data.size() * sizeof(typename ImageData<T, Ndim>::value_type));
    return data;
}

} // namespace reuss
