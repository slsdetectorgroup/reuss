
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "reuss/ZmqWriter.h"
#include "reuss/ZmqReceiver.h"
namespace py = pybind11;

template<typename T>
std::tuple<py::array_t<int64_t> ,py::array_t<T> >receive_n(reuss::ZmqReceiver& r, int n_frames){
    py::array_t<int64_t> frame_numbers(n_frames);
    py::array_t<T> data(std::array<size_t,3>{static_cast<size_t>(n_frames),IMAGE_SIZE.rows,IMAGE_SIZE.cols});

    r.connect();
    r.receive_into(n_frames, frame_numbers.mutable_data(), reinterpret_cast<std::byte*>(data.mutable_data()));
    r.disconnect();
    return {frame_numbers, data};
}

void init_zmqwriter(py::module &m) {

    py::class_<reuss::ZmqWriter> ZmqWriter(m, "ZmqWriter");
    ZmqWriter.def(py::init<std::string>())
    .def("start", &reuss::ZmqWriter::start)
    .def("stop", &reuss::ZmqWriter::stop)
    .def("is_running", &reuss::ZmqWriter::is_running)
    .def("stop_requested", &reuss::ZmqWriter::stop_requested)
    .def("set_path", &reuss::ZmqWriter::set_fname)
    .def("frames_written", &reuss::ZmqWriter::frames_written);


    py::class_<reuss::ZmqReceiver> ZmqReceiver(m, "ZmqReceiver");
    ZmqReceiver.def(py::init<std::string>())
    .def("receive_n", &receive_n<uint16_t>)
    .def("receive_n_float", &receive_n<float>);
}


