#include <pybind11/pybind11.h>

#include "OpenGLUtil.h"
#include "Matrix.h"
#include "Vector4.h"
#include "GameObject.h"
#include "Engine.h"

namespace py = pybind11;

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(le, m) {
    m.doc() = "LittleEngine";

//    m.def("add", &add, "A function which adds two numbers");

#if 1
    py::class_<Engine>(m, "Engine")
        .def(py::init<>())
        .def("start", &Engine::start);
#endif
}

