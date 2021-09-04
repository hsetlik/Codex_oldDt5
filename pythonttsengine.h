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
    void execSpeak(QString text, QString language);

private:
    py::module_ gTTSMod;
    py::module_ engineMod;
    py::module_ pyttsMod;
    py::module_ playsoundMod;


};

#endif // PYTHONTTSENGINE_H
