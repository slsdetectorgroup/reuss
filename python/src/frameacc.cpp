#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "reuss/DataSpan.h"
#include "reuss/DetectorInterface.h"
#include "reuss/pedestal.h"
#include "span_helpers.h"

#include "np_to_image.h"
#include "reuss/FrameAccumulator.h"

namespace py = pybind11;

using DataType = float;
using Accumulator = reuss::FrameAccumulator<DataType>;


reuss::ImageData<int, 3> some_func() {
    return reuss::ImageData<int, 3>({3, 4, 5}, 7);
}

void init_frameacc(py::module &m) {

    py::class_<reuss::FrameAccumulator<DataType>> FrameAccumulator(
        m, "FrameAccumulator");
    FrameAccumulator.def(py::init())
        .def("accumulate",
             [](reuss::FrameAccumulator<DataType> &a, int n_frames) {
                 auto image =
                     new reuss::ImageData<DataType, 2>(a.accumulate(n_frames));
                 return return_image_data(image);
             })

        .def("set_threshold", &reuss::FrameAccumulator<DataType>::set_threshold)
        .def("get_threshold", &reuss::FrameAccumulator<DataType>::set_threshold)
        .def("set_pedestal", [](reuss::FrameAccumulator<DataType> &a, py::array_t<DataType> pd) {
            auto span = make_span(pd);
            a.set_pedestal(span);
        })
        .def("get_pedestal", [](const reuss::FrameAccumulator<DataType> &a) {
            auto pd = new reuss::ImageData<DataType, 3>(a.get_pedestal());
            return return_image_data(pd);
        })
        .def("set_calibration", [](reuss::FrameAccumulator<DataType> &a, py::array_t<DataType> pd) {
            auto span = make_span(pd);
            a.set_calibration(span);
        })
        .def("get_calibration", [](const reuss::FrameAccumulator<DataType> &a) {
            auto pd = new reuss::ImageData<DataType, 3>(a.get_calibration());
            return return_image_data(pd);
        });
}
