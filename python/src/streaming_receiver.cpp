#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "reuss/DataSpan.h"
#include "reuss/DetectorInterface.h"
#include "reuss/pedestal.h"
#include "span_helpers.h"

#include "reuss/StreamingReceiver.h"

namespace py = pybind11;

using DataType = float;


void init_streamrecv(py::module &m) {

    py::class_<reuss::StreamingReceiver> StreamingReceiver(m, "StreamingReceiver");
    StreamingReceiver.def(py::init())
    .def("start", &reuss::StreamingReceiver::start)
    .def("stop", &reuss::StreamingReceiver::stop)
    .def("lost_packets", &reuss::StreamingReceiver::lost_packets)
    .def("last_frame", &reuss::StreamingReceiver::last_frame)
    .def("total_frames", &reuss::StreamingReceiver::total_frames);
}
