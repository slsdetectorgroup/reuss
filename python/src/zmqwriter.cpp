
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "reuss/ZmqWriter.h"
#include "reuss/ZmqReceiver.h"
namespace py = pybind11;

std::tuple<py::array_t<int64_t> ,py::array_t<double> >receive_n(reuss::ZmqReceiver& r, int64_t n_frames){
    py::array_t<int64_t> frame_numbers(n_frames);
    py::array_t<double> data(std::array<size_t,3>{n_frames,512,1024});

    r.receive_into(n_frames, frame_numbers.mutable_data(), reinterpret_cast<std::byte*>(data.mutable_data()));

    return {frame_numbers, data};
}

void init_zmqwriter(py::module &m) {

    py::class_<reuss::ZmqWriter> ZmqWriter(m, "ZmqWriter");
    ZmqWriter.def(py::init<std::string>())
    .def("start", &reuss::ZmqWriter::start)
    .def("stop", &reuss::ZmqWriter::stop)
    .def("is_running", &reuss::ZmqWriter::is_running)
    .def("stop_requested", &reuss::ZmqWriter::stop_requested)
    .def("set_path", &reuss::ZmqWriter::set_fname);


    py::class_<reuss::ZmqReceiver> ZmqReceiver(m, "ZmqReceiver");
    ZmqReceiver.def(py::init<std::string>())
    .def("receive_n", &receive_n);
}


