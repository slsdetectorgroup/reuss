#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

#include "reuss/Gotthard2Receiver.h"
namespace py = pybind11;
void init_g2receiver(py::module &m) {

    py::class_<reuss::Gotthard2Receiver> Gotthard2Receiver(m, "Gotthard2Receiver");
    Gotthard2Receiver.def(py::init<>())
    .def_property("udp_source", &reuss::Gotthard2Receiver::UdpSource, &reuss::Gotthard2Receiver::setUdpSource)
    .def_property("frames", &reuss::Gotthard2Receiver::Frames, &reuss::Gotthard2Receiver::setFrames)
    .def_property("fwrite", &reuss::Gotthard2Receiver::Fwrite, &reuss::Gotthard2Receiver::setFwrite)
    .def_property("fpath", &reuss::Gotthard2Receiver::Fpath, &reuss::Gotthard2Receiver::setFpath)
    .def_property("fname", &reuss::Gotthard2Receiver::Fname, &reuss::Gotthard2Receiver::setFname)
    .def_property("findex", &reuss::Gotthard2Receiver::Findex, &reuss::Gotthard2Receiver::setFindex)
    .def_property("stream_nth", &reuss::Gotthard2Receiver::StreamNth, &reuss::Gotthard2Receiver::setStreamNth)
    .def_property_readonly("done", &reuss::Gotthard2Receiver::done)
    .def_property_readonly("running", &reuss::Gotthard2Receiver::running)
    .def_property_readonly("pathname", &reuss::Gotthard2Receiver::Pathname)
    .def_property_readonly("progress", &reuss::Gotthard2Receiver::progress)
    .def("start", &reuss::Gotthard2Receiver::start)
    .def("stop", &reuss::Gotthard2Receiver::stop);
}

    // 