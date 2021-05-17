

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

void init_zmqwriter(py::module &);
void init_pedestal(py::module &);
void init_interface(py::module &);
void init_calibration(py::module &);

PYBIND11_MODULE(_reuss, m) {
    m.doc() = R"pbdoc(
        C/C++ API
        -----------------------
        .. warning ::

            This is the compiled c extension. You probably want to look at the
            interface provided by sls instead.

    )pbdoc";

     init_zmqwriter(m);
     init_pedestal(m);
     init_interface(m);
     init_calibration(m);
    

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}