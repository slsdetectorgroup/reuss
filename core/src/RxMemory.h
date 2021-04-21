#pragma once
#include <memory>
namespace ur {
// C++11 make_unique implementation for exception safety
// already available as std::make_unique in C++14
template <typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
typename std::enable_if<std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(std::size_t n) {
    typedef typename std::remove_extent<T>::type RT;
    return std::unique_ptr<T>(new RT[n]);
}
} // namespace ur
