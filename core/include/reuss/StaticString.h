#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>
namespace reuss {

template <size_t Capacity> class StaticString {
    std::array<char, Capacity + 1> buffer_{}; // nullterm
    size_t size_{};

  public:
    StaticString() = default;

    template <size_t OtherSize>
    explicit StaticString(const char (&other)[OtherSize]) {
        *this = other;
    }
    StaticString(const StaticString &) = default;
    StaticString(StaticString &&) = default;

    size_t size() const noexcept { return size_; }

    bool operator==(const char *other) const noexcept { return sv() == other; }

    bool operator==(const StaticString &other) { return sv() == other.sv(); }

    char &operator[](size_t i) { return buffer_[i]; }

    const char &operator[](size_t i) const { return buffer_[i]; }

    template <size_t OtherSize>
    StaticString &operator=(const char (&other)[OtherSize]) noexcept {
        static_assert(OtherSize < Capacity, "Size of other char[] to big");
        std::string_view v(other);
        std::copy(v.begin(), v.end(), buffer_.data());
        size_ = v.size();
        return *this;
    }

    std::string_view sv() const noexcept {
        return std::string_view(buffer_.data(), size_);
    }

    void clear() noexcept {
        buffer_.fill('\0');
        size_ = 0;
    }

    auto begin(){
        buffer_.begin();
    }

    auto end(){
        buffer_.end();
    }
};

} // namespace reuss