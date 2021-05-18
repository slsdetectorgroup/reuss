#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "reuss/DataSpan.h"
#include "reuss/DetectorInterface.h"
#include "reuss/pedestal.h"
#include "span_helpers.h"

#include "reuss/FrameAccumulator.h"

namespace py = pybind11;

using DataType = float;

void init_frameacc(py::module &m) {
    // m.def("apply_calibration", &reuss::apply_calibration<double>,
    //       py::arg().noconvert(), py::arg().noconvert(), py::arg().noconvert(),
    //       py::arg() = 8);
    // m.def("apply_calibration", &reuss::apply_calibration<float>,
    //       py::arg().noconvert(), py::arg().noconvert(), py::arg().noconvert(),
    //       py::arg() = 8);
    // m.def("apply_calibration_and_sum", &reuss::apply_calibration_and_sum<float>,
    //       py::arg().noconvert(), py::arg().noconvert(), py::arg().noconvert(),
    //       py::arg() = 50, py::arg() = 10.);
    // m.def("split_task", &split_task);

    py::class_<reuss::ImageData<DataType, 2>>(m, "ImageData", py::buffer_protocol())
   .def_buffer([](reuss::ImageData<DataType, 2> &m) -> py::buffer_info {
        return py::buffer_info(
            m.data(),                               /* Pointer to buffer */
            sizeof(DataType),                          /* Size of one scalar */
            py::format_descriptor<DataType>::format(), /* Python struct-style format descriptor */
            2,                                      /* Number of dimensions */
            m.shape(),                 /* Buffer dimensions */
            m.byte_strides()
        );
    });

    py::class_<reuss::FrameAccumulator<DataType>> FrameAccumulator(m, "FrameAccumulator");
    FrameAccumulator.def(py::init())
    .def("accumulate", &reuss::FrameAccumulator<DataType>::accumulate)
    .def("set_pedestal", &reuss::FrameAccumulator<DataType>::set_pedestal);
}

// .def("set_gain", &reuss::DetectorInterface::set_gain)