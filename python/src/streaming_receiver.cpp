#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "reuss/DataSpan.h"
#include "reuss/DetectorInterface.h"
#include "reuss/pedestal.h"
#include "span_helpers.h"
#include "np_to_image.h"

#include "reuss/StreamingReceiver.h"
#include "reuss/SummingReceiver.h"

namespace py = pybind11;

using DataType = float;

void init_streamrecv(py::module &m) {

    py::class_<reuss::StreamingReceiver> StreamingReceiver(m,
                                                           "StreamingReceiver");
    StreamingReceiver.def(py::init())
        .def("start", &reuss::StreamingReceiver::start)
        .def("stop", &reuss::StreamingReceiver::stop)
        .def("lost_packets", &reuss::StreamingReceiver::lost_packets)
        .def("last_frame", &reuss::StreamingReceiver::last_frame)
        .def("total_frames", &reuss::StreamingReceiver::total_frames);

    py::class_<reuss::SummingReceiver> SummingReceiver(m, "SummingReceiver");
    SummingReceiver.def(py::init<size_t>())
        .def("start", &reuss::SummingReceiver::start)
        .def("stop", &reuss::SummingReceiver::stop)
        .def("lost_packets", &reuss::SummingReceiver::lost_packets)
        .def("last_frame", &reuss::SummingReceiver::last_frame)
        .def("total_frames", &reuss::SummingReceiver::total_frames)
        .def("set_pedestal", [](reuss::SummingReceiver &a,
                                py::array_t<DataType> pd) {
            auto span = make_span(pd);
            a.set_pedestal(span);
        })
        .def("get_pedestal", [](const reuss::SummingReceiver &a) {
            auto pd = new reuss::ImageData<DataType, 3>(a.get_pedestal());
            return return_image_data(pd);
        })
        .def("set_calibration", [](reuss::SummingReceiver &a,
                                   py::array_t<DataType> cal) {
            auto span = make_span(cal);
            a.set_calibration(span);
        })
        .def("get_calibration", [](const reuss::SummingReceiver &a) {
            auto cal = new reuss::ImageData<DataType, 3>(a.get_calibration());
            return return_image_data(cal);
        })
        .def("set_frames_to_sum", &reuss::SummingReceiver::set_frames_to_sum)
        .def("get_frames_to_sum", &reuss::SummingReceiver::get_frames_to_sum)
        .def("record_pedestal", &reuss::SummingReceiver::record_pedestal)
        .def("set_threshold", &reuss::SummingReceiver::set_threshold);
}
