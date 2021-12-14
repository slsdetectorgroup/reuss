#include "utils.h"

// std::vector<std::pair<int, int>> split_task(int first, int last,
//                                             int n_threads) {
//     std::vector<std::pair<int, int>> vec;
//     vec.reserve(n_threads);

//     int n_frames = last - first;

//     if (n_threads >= n_frames) {
//         for (int i = 0; i != n_frames; ++i) {
//             vec.push_back({i, i + 1});
//         }
//         return vec;
//     }

//     int step = (n_frames) / n_threads;
//     for (int i = 0; i != n_threads; ++i) {
//         int start = step * i;
//         int stop = step * (i + 1);
//         if (i == n_threads - 1)
//             stop = last;
//         vec.push_back({start, stop});
//     }
//     return vec;
// }

double arr_min(double* arr, size_t len){
    if (len == 0)
        return 0.0; //Hmm...
    double r = arr[0];
    for(size_t i = 1; i<len; ++i){
        r = std::min(r, arr[i]);
    }
    return r;
}

double arr_max(double* arr, size_t len){
    if (len == 0)
        return 0.0; //Hmm...
    double r = arr[0];
    for(size_t i = 1; i<len; ++i){
        r = std::max(r, arr[i]);
    }
    return r;
}