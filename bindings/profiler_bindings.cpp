#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../include/profiler/profiler.hpp"

namespace py = pybind11;
using namespace profiler;

PYBIND11_MODULE(latency_profiler_py, m) {
    m.doc() = "Low-latency C++ Profiler Python Bindings";

    py::class_<Profiler::Report>(m, "Report")
        .def_readonly("name", &Profiler::Report::name)
        .def_readonly("events", &Profiler::Report::events)
        .def_readonly("mean_us", &Profiler::Report::mean_us)
        .def_readonly("p50_us", &Profiler::Report::p50_us)
        .def_readonly("p99_us", &Profiler::Report::p99_us)
        .def_readonly("p999_us", &Profiler::Report::p999_us)
        .def_readonly("min_ns", &Profiler::Report::min_ns)
        .def_readonly("max_ns", &Profiler::Report::max_ns);

    py::class_<Profiler>(m, "Profiler")
        .def(py::init<std::string>(), py::arg("name") = "default")
        .def("start", &Profiler::start)
        .def("stop", &Profiler::stop)
        .def("report", &Profiler::report)
        .def("reset", &Profiler::reset);
}