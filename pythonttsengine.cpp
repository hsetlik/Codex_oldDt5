#include "pythonttsengine.h"
#include <QtWidgets>

PythonTtsEngine::PythonTtsEngine()
{
    printf("TTS Engine created\n");
    Q_INIT_RESOURCE(python_resources);

    auto dir = QDir::current();
    auto engineScriptPath = dir.absoluteFilePath("tts_engine.py");
    QFile engineScriptFile(engineScriptPath);
    if(!engineScriptFile.exists())
    {
        printf("Engine script not found\n");
        QFile engineFile("://scripts/scripts/tts_engine.py");
        copyFileContents(engineFile, engineScriptFile);
    }
    else
        printf("Engine script exists\n");
   // step one finished, but tts_engine has three dependencies: urllib, gTTS, and pyttsx3
   // they can be added as git submodules in 'extern' and added to the .qrc before we follow
    // the same process of copying resources to the working directory as needed
    engineMod = py::module_::import("tts_engine");
    auto connected = engineMod.attr("has_connection")();
   auto isConnected = connected.cast<bool>();
   if(isConnected)
       printf("Configuring gTTS. . .\n");
   else
       printf("No connection! Configuring pyttsx3\n");



}

void PythonTtsEngine::copyFileContents(QFile& from, QFile& to)
{
    if(!from.open(QIODevice::ReadOnly))
        printf("From not opened\n");
    auto fromData = from.readAll();
    from.close();
    if(!to.open(QIODevice::ReadWrite))
        printf("To not opened\n");
    to.write(fromData);
    to.close();
}
