#ifndef PYTHONTTSENGINE_H
#define PYTHONTTSENGINE_H

#include <QObject>
#include <QFile>
#include <extern/pybind11/include/pybind11/embed.h>

namespace py = pybind11;
using namespace py::literals;

class PythonTtsEngine
{
public:
    PythonTtsEngine();
    static void copyFileContents(QFile& from, QFile& to);
    static void copyDir(const QString &fromDir, const QString &toDir);

    void installPythonLibrary(QString& resourcePath);
private:
    py::module_ gTTSMod;
    py::module_ engineMod;
    py::module_ pyttsMod;


};

#endif // PYTHONTTSENGINE_H
