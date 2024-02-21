#include <array>
#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
// #include <iostream>

#include "reuss/DataType.h"
namespace reuss {

// copy between
inline std::string_view find_between(const char d1, const char d2,
                                     std::string_view sv) {
    auto start = sv.find_first_of(d1) + 1;
    auto stop = sv.find_first_of(d2, start);
    auto size = stop - start;
    return std::string_view(&sv[start], size);
}

// copy between
inline std::string_view find_between(const char delimiter,
                                     std::string_view sv) {
    return find_between(delimiter, delimiter, sv);
}

inline std::vector<int> str2vec(std::string_view sv) {
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

inline std::string vec2str(const std::vector<int> vec) {
    std::ostringstream oss;
    oss << "(";
    if (!vec.empty()) {
        auto it = vec.cbegin();
        oss << *it++ << ',';
        while (it != vec.cend())
            oss << ' ' << *it++ << ',';
    }
    oss << ")";
    return oss.str();
}

class NumpyArrDescr {
    std::string descr_;
    bool fortran_order_{false};
    std::vector<int> shape_;

  public:
    NumpyArrDescr() = default;
    NumpyArrDescr(const NumpyArrDescr &) = default;
    NumpyArrDescr(NumpyArrDescr &&) = default;
    NumpyArrDescr &operator=(const NumpyArrDescr &) = default;
    NumpyArrDescr(std::string_view sv) {
        constexpr char key_0[] = "\'descr\'";
        auto pos = sv.find(key_0);
        pos = sv.find_first_of(':', pos);
        descr_ =
            find_between('\'', std::string_view(&sv[pos], sv.size() - pos));

        constexpr char key_1[] = "\'fortran_order\'";
        pos = sv.find(key_1);
        pos = sv.find_first_of(':', pos);
        auto b =
            find_between('\'', std::string_view(&sv[pos], sv.size() - pos));
        fortran_order_ = (b == "True") ? true : false;

        constexpr char key_2[] = "\'shape\'";
        pos = sv.find(key_2);
        pos = sv.find_first_of(':', pos);
        auto s = find_between('(', ')',
                              std::string_view(&sv[pos], descr_.size() - pos));
        shape_ = str2vec(s);
    }

    std::vector<int> shape() const { return shape_; }
    void set_shape(std::vector<int> sh) { shape_ = sh; }
    std::string descr() const { return descr_; }
    bool fortran_order() const noexcept { return fortran_order_; }
    std::string str() const {
        return fmt::format(
            "{{'descr': '{}', 'fortran_order': {}, 'shape': {}, }}", descr_,
            (fortran_order_) ? "True" : "False", vec2str(shape_));
    }
};

class NumpyFileHeader {
    uint8_t major_ver_{};
    uint8_t minor_ver_{};

  public:
    static constexpr std::array<char, 6> magic_str{'\x93', 'N', 'U',
                                                   'M',    'P', 'Y'};
    uint8_t major_ver() const noexcept { return major_ver_; }
    uint8_t minor_ver() const noexcept { return minor_ver_; }

    NumpyArrDescr descr;

    std::vector<int> shape() const { return descr.shape(); }
    size_t ndim() const noexcept { return descr.shape().size(); }
    std::string dtype() const { return descr.descr(); }
    bool fortran_order() const noexcept { return descr.fortran_order(); }
    NumpyFileHeader() = default;
    NumpyFileHeader(DataType dt, std::vector<int> sh)
        : major_ver_(1), minor_ver_(0){
        descr.set_shape(sh);
        }

    static NumpyFileHeader fromFile(std::ifstream &f) {
        std::array<char, 6> tmp{};
        f.read(tmp.data(), tmp.size());
        if (tmp != NumpyFileHeader::magic_str) {
            for (auto item : tmp)
                fmt::print("{}, ", int(item));
            fmt::print("\n");
            throw std::runtime_error("Not a numpy file");
        }
        NumpyFileHeader h;
        f.read(reinterpret_cast<char *>(&h.major_ver_), 1);
        f.read(reinterpret_cast<char *>(&h.minor_ver_), 1);

        uint16_t header_len;
        f.read(reinterpret_cast<char *>(&header_len), 2);

        std::string header;
        header.resize(header_len);
        f.read(&header[0], header_len);
        h.descr = NumpyArrDescr(header);
        return h;
    }

    static NumpyFileHeader fromFile(const std::string &fname) {
        std::ifstream f(fname, std::ios::binary);
        if (!f)
            throw std::runtime_error("File not found");
        return NumpyFileHeader::fromFile(f);
    }
};

} // namespace reuss