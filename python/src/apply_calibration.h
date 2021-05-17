#pragma once
#include <fmt/core.h>
#include <future>
#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <utility>
#include <vector>

#include "reuss/DataSpan.h"
#include "span_helpers.h"
#include "utils.h"

namespace py = pybind11;

constexpr uint16_t mask = 0x3FFF;

int get_gain(uint16_t raw){
    switch (raw >> 14)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    case 3:
        return 2;
    default:
        return 0;
    }
}

namespace reuss{

template <class DataType>
void apply(DataSpan<DataType, 3> res, DataSpan<uint16_t, 3> raw_data,
           DataSpan<DataType, 3> ped, DataSpan<DataType, 3> cal, ssize_t start,
           ssize_t stop) {
    
    // fmt::print("start: {}, stop: {}\n", start, stop);
    for (int frame_nr = start; frame_nr != stop; ++frame_nr) {
        // fmt::print("{}\n", frame_nr);
        for (int row = 0; row != raw_data.shape(1); ++row) {
            for (int col = 0; col != raw_data.shape(2); ++col) {
                auto gain = get_gain(raw_data(frame_nr, row, col));
                int value = raw_data(frame_nr, row, col) & mask;
                res(frame_nr, row, col) =
                    (value - ped(gain, row, col)) / cal(gain, row, col);
            }
        }
    }
}

template <class DataType>
py::array_t<DataType> apply_calibration(
    //TODO! Assert that we can access this 
    py::array_t<uint16_t, py::array::c_style | py::array::forcecast> data,
    py::array_t<DataType, py::array::c_style | py::array::forcecast> pedestal,
    py::array_t<DataType, py::array::c_style | py::array::forcecast>
        calibration,
    int n_threads = 8) {

    auto data_span = make_span(data);
    auto ped = make_span(pedestal);
    auto cal = make_span(calibration);

    /* No pointer is passed, so NumPy will allocate the buffer */
    auto result = py::array_t<DataType>(data_span.shape());
    auto res = make_span(result);

    std::vector<std::future<void>> futures;
    futures.reserve(n_threads);
    auto limits = split_task(0, data_span.shape(0), n_threads);
    for (const auto &lim : limits)
        futures.push_back(std::async(&apply<DataType>, res, data_span, ped, cal,
                                     lim.first, lim.second));
    for (auto &f : futures)
        f.get();
    return result;
}

template <class DataType>
py::array_t<DataType> apply_calibration_and_sum(
    py::array_t<uint16_t, py::array::c_style | py::array::forcecast> raw_data,
    py::array_t<DataType, py::array::c_style | py::array::forcecast> pedestal,
    py::array_t<DataType, py::array::c_style | py::array::forcecast>
        calibration,
    int n_frames = 50, DataType threshold = 10) {

    auto data_span = make_span(raw_data);
    auto ped = make_span(pedestal);
    auto cal = make_span(calibration);

    ssize_t n_frames_out = raw_data.shape(0) / n_frames;

    /* No pointer is passed, so NumPy will allocate the buffer */
    auto result = py::array_t<DataType>(
        {n_frames_out, data_span.shape(1), data_span.shape(2)});
    auto res = make_span(result);
    res = 0;

    // Time to convert
    int in_frame_nr = 0;
    for (int frame_nr = 0; frame_nr != res.shape(0); ++frame_nr) {
        for (int i = 0; i != n_frames; ++i) {
            for (int row = 0; row != data_span.shape(1); ++row) {
                for (int col = 0; col != data_span.shape(2); ++col) {
                    auto gain = get_gain(data_span(in_frame_nr, row, col));
                    int value = data_span(in_frame_nr, row, col) & mask;
                    DataType e_val =
                        (value - ped(gain, row, col)) / cal(gain, row, col);

                    if (e_val > threshold)
                        res(frame_nr, row, col) += e_val;
                }
            }
            in_frame_nr++;
        }
    }

    return result;
}

}