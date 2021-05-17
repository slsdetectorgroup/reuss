

#pragma once
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "reuss/DataSpan.h"

namespace py = pybind11;

template <class T, int Flags> auto get_shape(py::array_t<T, Flags> arr) {
    return reuss::Shape<3>{arr.shape(0), arr.shape(1), arr.shape(2)};
}

template <class T, int Flags> auto make_span(py::array_t<T, Flags> arr) {
    return reuss::DataSpan<T, 3>(arr.mutable_data(), get_shape<T, Flags>(arr));
}

template <class T, int Flags> auto get_shape2(py::array_t<T, Flags> arr) {
    return reuss::Shape<2>{arr.shape(0), arr.shape(1)};
}

template <class T, int Flags> auto make_span2(py::array_t<T, Flags> arr) {
    return reuss::DataSpan<T, 2>(arr.mutable_data(), get_shape2<T, Flags>(arr));
}