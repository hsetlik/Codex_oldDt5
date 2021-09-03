#ifndef PYTHONTTSENGINE_H
#define PYTHONTTSENGINE_H

#include <QObject>
#include <extern/pybind11/include/pybind11/embed.h>

namespace py = pybind11;
using namespace py::literals;

class PythonTtsEngine
{
public:
    PythonTtsEngine();
};

#endif // PYTHONTTSENGINE_H
