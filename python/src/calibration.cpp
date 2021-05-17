#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "reuss/pedestal.h"
#include "reuss/DataSpan.h"
#include "reuss/DetectorInterface.h"
#include "span_helpers.h"

#include "apply_calibration.h"

namespace py = pybind11;




void init_calibration(py::module &m) {
    m.def("apply_calibration", &reuss::apply_calibration<double>,
          py::arg().noconvert(), py::arg().noconvert(), py::arg().noconvert(),
          py::arg() = 8);
    m.def("apply_calibration", &reuss::apply_calibration<float>, py::arg().noconvert(),
          py::arg().noconvert(), py::arg().noconvert(), py::arg() = 8);
    m.def("apply_calibration_and_sum", &reuss::apply_calibration_and_sum<float>,
          py::arg().noconvert(), py::arg().noconvert(), py::arg().noconvert(),
          py::arg() = 50, py::arg() = 10.);
    m.def("split_task", &split_task);
}

