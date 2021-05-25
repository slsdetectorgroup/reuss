#pragma once

#include <string>
#include <array>
#include <fstream>
#include <map>
#include <fmt/core.h>
#include <string_view>
#include <vector>
#include <thread>
#include <chrono>
// #include <string>

namespace reuss {

struct NumpyFileHeader {
    static constexpr std::array<char,6> magic_str{char(0x93), 'N', 'U', 'M', 'P', 'Y'};
    uint8_t major_ver;
    uint8_t minor_ver;
    uint16_t header_len;
};

struct NumpyArrDescr{
    std::string descr;
    bool fortran_order;
    std::vector<int> shape;
};

//copy between
std::string find_between(const char delimiter, std::string_view s){
    auto start = s.find_first_of(delimiter)+1;
    auto stop = s.find_first_of(delimiter, start);
    auto size = stop-start;
    std::string ret;
    ret.resize(size);
    s.copy(&ret[0], size, start);
    return ret;
}

//copy between
std::string find_between(const char d1, const char d2,  std::string_view s){
    auto start = s.find_first_of(d1)+1;
    auto stop = s.find_first_of(d2, start);
    auto size = stop-start;
    std::string ret;
    ret.resize(size);
    s.copy(&ret[0], size, start);
    return ret;
}

std::vector<int> str2vec(std::string_view sv){
    std::vector<int> vec; 
    vec.reserve(3);
    size_t curr = 0;
    constexpr char number[] = "0123456789";
    size_t next = sv.find_first_of(number, curr);
    //(45, 45, 89)
    while (next != std::string_view::npos){
        int res = std::atoi(&sv[next]);
        vec.push_back(res);
        curr = sv.find_first_of(',', next);
        next = sv.find_first_of(number, curr);
        // fmt::print("curr: {}, next: {}, npos: {}\n", curr, next, std::string_view::npos);
        // std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return vec;
}


inline NumpyArrDescr parse_numpy_descr(const std::string& descr){

    NumpyArrDescr arr_d;
    auto end = descr.end();
    auto it = descr.begin();

    constexpr char key_0[] = "\'descr\'";
    auto pos = descr.find(key_0);
    pos = descr.find_first_of(':', pos);
    arr_d.descr = find_between('\'', std::string_view(&descr[pos], descr.size()-pos));


    constexpr char key_1[] = "\'fortran_order\'";
    pos = descr.find(key_1);
    pos = descr.find_first_of(':', pos);
    auto b = find_between('\'', std::string_view(&descr[pos], descr.size()-pos));
    arr_d.fortran_order = (b == "True" ) ? true : false;

    constexpr char key_2[] = "\'shape\'";
    pos = descr.find(key_2);
    pos = descr.find_first_of(':', pos);
    auto s = find_between('(', ')', std::string_view(&descr[pos], descr.size()-pos));
    arr_d.shape = str2vec(s);


    // fmt::print("pos:{}\n", pos);
    return arr_d;
}

inline NumpyFileHeader load_numpy_header(const std::string &fname){

    std::ifstream f(fname, std::ios::binary);
    if(!f)
        throw std::runtime_error("File not found");
    std::array<char, 6> tmp{};
    f.read(tmp.data(),tmp.size());
    if(tmp != NumpyFileHeader::magic_str){
        for (auto item : tmp)
            fmt::print("{}, ", int(item));
        fmt::print("\n");
        throw std::runtime_error("Not a numpy file");
    }
    NumpyFileHeader h;
    f.read(reinterpret_cast<char*>(&h.major_ver), 1);
    f.read(reinterpret_cast<char*>(&h.minor_ver), 1);
    f.read(reinterpret_cast<char*>(&h.header_len), 2);
    fmt::print("header ver: {}.{}\n", h.major_ver, h.minor_ver);
    fmt::print("header size: {}\n", h.header_len);
    std::string header;
    header.resize(h.header_len);
    f.read(&header[0], h.header_len);
    fmt::print("header: {}end\n", header);
        
    return {};
}

} // namespace reuss

