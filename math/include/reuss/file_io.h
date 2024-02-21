#pragma once

#include <array>
#include <chrono>
#include <fmt/core.h>
#include <fstream>
#include <map>
#include <reuss/DataSpan.h>
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
    if (DataType(header.dtype()) != typeid(T)) {
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

template<typename T, size_t Ndim>
void save_numpy(const std::string& fname, DataSpan<T,Ndim> data){

    

}

template <typename T> ImageData<T, 3> load_raw_bin(const std::string &fname) {
    std::ifstream f(fname, std::ios::binary);
    if (!f)
        throw std::runtime_error("File not found");

    f.seekg(0, std::ios::end);
    auto eof = f.tellg();
    f.seekg(-16, std::ios::end);
    int64_t n_frames, meta_size;
    f.read(reinterpret_cast<char *>(&n_frames), sizeof(int64_t));
    f.read(reinterpret_cast<char *>(&meta_size), sizeof(int64_t));
    f.seekg(0, std::ios::beg);

    std::array<ssize_t, 3> shape{n_frames, 512, 1024};
    ImageData<uint16_t, 3> data(shape);
    size_t n_bytes = data.size() * sizeof(uint16_t);
    fmt::print("shape: {}, {}, {}\n", shape[0], shape[1], shape[2]);
    fmt::print("Trying to read {} frames for a total of: {} bytes\n", n_frames,
               n_bytes);

    f.read(reinterpret_cast<char *>(data.buffer()),
           data.size() * sizeof(uint16_t));

    std::vector<int64_t> meta(n_frames);
    f.read(reinterpret_cast<char *>(meta.data()),
           meta.size() * sizeof(int64_t));
    auto expect_size = meta_size + data.size() * sizeof(T);
    if (eof != expect_size) {
        throw std::runtime_error("File size does not match, expected: " +
        std::to_string(expect_size) + ", actual: " + std::to_string(eof) + ". Corrupt file?");
    }
    return data;
}

} // namespace reuss
