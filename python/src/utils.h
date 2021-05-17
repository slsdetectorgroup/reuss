#pragma once
#include <vector>
#include <cstddef>
std::vector<std::pair<int, int>> split_task(int first, int last,
                                            int n_threads);

double arr_min(double* arr, size_t len);
double arr_max(double* arr, size_t len);
