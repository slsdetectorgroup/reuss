#pragma once
#include <string>
#include <iostream>
#include <fmt/core.h>
namespace reuss {

enum class endian
{
#ifdef _WIN32
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};

class DataType {
  public:
    explicit DataType(const std::type_info &t) {
        if (t == typeid(int32_t))
            type_ = TypeIndex::INT32;
        else if (t == typeid(uint32_t))
            type_ = TypeIndex::UINT32;
        else if (t == typeid(int64_t))
            type_ = TypeIndex::INT64;
        else if (t == typeid(uint64_t))
            type_ = TypeIndex::UINT64;
        else if (t == typeid(float))
            type_ = TypeIndex::FLOAT;
        else if (t == typeid(double))
            type_ = TypeIndex::DOUBLE;
        else
            throw std::runtime_error("Could not construct data type. Type no supported.");
    }
    explicit DataType(std::string_view sv) {

        //Check if the file is using our native endianess
        if (auto pos = sv.find_first_of("<>"); pos != std::string_view::npos){
            const auto endianess = [](const char c){
                if (c == '<')
                    return endian::little;
                return endian::big;
                }(sv[pos]);
            if (endianess != endian::native){
                throw std::runtime_error("Non native endianess not supported");
            }
        }

        //we are done with the endianess so we can remove the prefix
        sv.remove_prefix(std::min(sv.find_first_not_of("<>"), sv.size()));

        if (sv == "i4")
            type_ = TypeIndex::INT32;
        else if (sv == "u4")
            type_ = TypeIndex::UINT32;
        else if (sv == "i8")
            type_ = TypeIndex::INT64;
        else if (sv == "u8")
            type_ = TypeIndex::UINT64;
        else if (sv == "f4")
            type_ = TypeIndex::FLOAT;
        else if (sv == "f8")
            type_ = TypeIndex::DOUBLE;
        else
            throw std::runtime_error("Could not construct data type. Type no supported.");
    }

    bool operator==(const DataType &other) const noexcept {
        return type_ == other.type_;
    }
    bool operator!=(const DataType &other) const noexcept {
        return !(*this == other);
    }

    bool operator==(const std::type_info &t) const {
        return DataType(t) == *this;
    }
    bool operator!=(const std::type_info &t) const {
        return DataType(t) != *this;
    }

    std::string str() const {
        switch (type_) {
        case TypeIndex::INT32:
            return "i4";
        case TypeIndex::UINT32:
            return "u4";
        case TypeIndex::INT64:
            return "i8";
        case TypeIndex::UINT64:
            return "u8";
        case TypeIndex::FLOAT:
            return "f4";
        case TypeIndex::DOUBLE:
            return "f8";
        case TypeIndex::ERROR:
            return "ERROR";
        }
        return {};
    }

  private:
    enum class TypeIndex { INT32, UINT32, INT64, UINT64, FLOAT, DOUBLE, ERROR };
    TypeIndex type_{TypeIndex::ERROR};
};

inline std::ostream &operator<<(std::ostream &os, const DataType &dt) {
    os << dt.str();
    return os;
}

} // namespace reuss