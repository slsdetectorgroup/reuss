#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "reuss/pedestal.h"
#include "reuss/DataSpan.h"
#include "reuss/DetectorInterface.h"
#include "span_helpers.h"

namespace py = pybind11;

template<typename T>
py::array_t<T> py_pedestal(reuss::DetectorInterface* det){

    auto pedestal = py::array_t<T>({3, 512, int(COL_MAX-COL_MIN)});
    auto span = make_span(pedestal);
    take_pedestal(span, det, 500);
    return pedestal;
}

void init_pedestal(py::module &m) {
    m.def("take_pedestal_float", &py_pedestal<float>);
    m.def("take_pedestal_double", &py_pedestal<double>);
}