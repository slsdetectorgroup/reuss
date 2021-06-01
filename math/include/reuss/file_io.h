#pragma once

#include <array>
#include <chrono>
#include <fmt/core.h>
#include <fstream>
#include <map>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include <reuss/ImageData.h>
// #include <string>
#include <typeinfo>
#include <typeindex>

namespace reuss {



enum class DataType{
    INT32,
    UINT32,
    INT64,
    UINT64,
    FLOAT,
    DOUBLE,
    NOT_SUPPORTED,
};

DataType typeid2enum(std::type_index t){
    if (t==std::type_index(typeid(int))){
        return DataType::INT32;
    }
    return DataType::NOT_SUPPORTED;
}

// copy between
std::string find_between(const char delimiter, std::string_view s) {
    auto start = s.find_first_of(delimiter) + 1;
    auto stop = s.find_first_of(delimiter, start);
    auto size = stop - start;
    std::string ret;
    ret.resize(size);
    s.copy(&ret[0], size, start);
    return ret;
}

// copy between
std::string find_between(const char d1, const char d2, std::string_view s) {
    auto start = s.find_first_of(d1) + 1;
    auto stop = s.find_first_of(d2, start);
    auto size = stop - start;
    std::string ret;
    ret.resize(size);
    s.copy(&ret[0], size, start);
    return ret;
}

std::vector<int> str2vec(std::string_view sv) {
    std::vector<int> vec;
    vec.reserve(3);
    size_t curr = 0;
    constexpr char number[] = "0123456789";
    size_t next = sv.find_first_of(number, curr);
    while (next != std::string_view::npos) {
        int res = std::atoi(&sv[next]);
        vec.push_back(res);
        curr = sv.find_first_of(',', next);
        next = sv.find_first_of(number, curr);
    }
    return vec;
}

struct NumpyArrDescr {
    std::string descr;
    bool fortran_order;
    std::vector<int> shape;

    NumpyArrDescr() = default;

    NumpyArrDescr(std::string_view sv) {

        auto end = sv.end();
        auto it = sv.begin();

        constexpr char key_0[] = "\'descr\'";
        auto pos = sv.find(key_0);
        pos = sv.find_first_of(':', pos);
        descr = find_between('\'', std::string_view(&sv[pos], sv.size() - pos));

        constexpr char key_1[] = "\'fortran_order\'";
        pos = sv.find(key_1);
        pos = sv.find_first_of(':', pos);
        auto b =
            find_between('\'', std::string_view(&sv[pos], sv.size() - pos));
        fortran_order = (b == "True") ? true : false;

        constexpr char key_2[] = "\'shape\'";
        pos = sv.find(key_2);
        pos = sv.find_first_of(':', pos);
        auto s = find_between('(', ')',
                              std::string_view(&sv[pos], descr.size() - pos));
        shape = str2vec(s);
    }

    NumpyArrDescr(const NumpyArrDescr&) = default;
    NumpyArrDescr(NumpyArrDescr&&) = default;
    NumpyArrDescr& operator=(const NumpyArrDescr&) = default;
};

struct NumpyFileHeader {
    static constexpr std::array<char, 6> magic_str{char(0x93), 'N', 'U',
                                                   'M',        'P', 'Y'};
    uint8_t major_ver;
    uint8_t minor_ver;
    uint16_t header_len;
    NumpyArrDescr descr;

    std::vector<int> shape() const{
        return descr.shape;
    }
    size_t ndim() const noexcept{
        return descr.shape.size();
    }
};


NumpyFileHeader load_numpy_header(std::ifstream& f){
std::array<char, 6> tmp{};
    f.read(tmp.data(), tmp.size());
    if (tmp != NumpyFileHeader::magic_str) {
        for (auto item : tmp)
            fmt::print("{}, ", int(item));
        fmt::print("\n");
        throw std::runtime_error("Not a numpy file");
    }
    NumpyFileHeader h;
    f.read(reinterpret_cast<char *>(&h.major_ver), 1);
    f.read(reinterpret_cast<char *>(&h.minor_ver), 1);
    f.read(reinterpret_cast<char *>(&h.header_len), 2);

    std::string header;
    header.resize(h.header_len);
    f.read(&header[0], h.header_len);
    h.descr = NumpyArrDescr(header);
    return h;
}

inline NumpyFileHeader load_numpy_header(const std::string &fname) {

    std::ifstream f(fname, std::ios::binary);
    if (!f)
        throw std::runtime_error("File not found");
    
    return load_numpy_header(f);
}

template<typename T, size_t Ndim>
ImageData<T, Ndim> load_numpy(const std::string& fname){
    std::ifstream f(fname, std::ios::binary);
    if (!f)
        throw std::runtime_error("File not found");
    
    auto header = load_numpy_header(f);
    if (header.ndim() != Ndim){
        throw std::runtime_error("Cannot load file, dimensions of ImageData and numpy array does not match");
    }
    Shape<Ndim> shape{};
    auto vec = header.shape();
    std::copy(vec.begin(), vec.end(), shape.begin());

    ImageData<T, Ndim> data{shape};
    f.read(reinterpret_cast<char*>(data.buffer()), data.size()*sizeof(typename ImageData<T, Ndim>::value_type));
    return data;
}

} // namespace reuss
