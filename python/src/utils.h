#pragma once
#include <vector>
#include <cstddef>
#include <sys/types.h>
// std::vector<std::pair<int, int>> split_task(int first, int last,
//                                             int n_threads);

template<typename T>
std::vector<std::pair<T, T>> split_task(T first, T last,
                                            ssize_t n_threads) {
    std::vector<std::pair<T, T>> vec;
    vec.reserve(n_threads);

    T n_frames = last - first;

    if (n_threads >= n_frames) {
        for (int i = 0; i != n_frames; ++i) {
            vec.push_back({i, i + 1});
        }
        return vec;
    }

    T step = (n_frames) / n_threads;
    for (ssize_t i = 0; i != n_threads; ++i) {
        T start = step * i;
        T stop = step * (i + 1);
        if (i == n_threads - 1)
            stop = last;
        vec.push_back({start, stop});
    }
    return vec;
}


double arr_min(double* arr, size_t len);
double arr_max(double* arr, size_t len);
