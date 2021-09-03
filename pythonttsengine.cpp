#include "pythonttsengine.h"
#include <QtWidgets>

PythonTtsEngine::PythonTtsEngine()
{
    printf("TTS Engine created\n");
    py::module_ pyTest = py::module_::import("gtts");
    QFile scriptFile("/Users/hayden/Desktop/Programming/QtProjects/Codex/scripts/test_script.py");
    if(!scriptFile.open(QIODevice::ReadWrite))
        printf("File not opened\n");
    else
        printf("Script file found");
    auto byteArray = scriptFile.readAll();
    QTextStream stream(byteArray);
    auto fileStr = stream.readAll();
    qDebug() << "SCRIPT CONTENT: " << fileStr;
}
