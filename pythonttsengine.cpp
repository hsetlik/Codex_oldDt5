#include "pythonttsengine.h"
#include <QtWidgets>

PythonTtsEngine::PythonTtsEngine()
{
    printf("TTS Engine created\n");
    Q_INIT_RESOURCE(python_resources);
    //step 1: make sure the script file is in the working directory
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
    engineMod = py::module_::import("tts_engine");
    //test stuff
    auto connected = engineMod.attr("has_connection")();
   auto isConnected = connected.cast<bool>();
   if(isConnected)
       printf("Configuring gTTS. . .\n");
   else
       printf("No connection! Configuring pyttsx3\n");
   // step 2: make sure gTTS & pyttsx3 get copied
   auto gttsDir = QDir("gTTS");
   if(!gttsDir.exists())
   {
       QString resourceFolderName = "://libraries/extern/gTTS";
       copyDir(resourceFolderName, "gTTS");
       printf("gTTS copied\n");
   }
   auto pyttsDir = QDir("pyttsx3");
   if(!pyttsDir.exists())
   {
       QString resourceFolderName = "://libraries/extern/pyttsx3";
       copyDir(resourceFolderName, "pyttsx3");
       printf("pyttsx3 Copied\n");
   }
   gTTSMod = py::module_::import("gtts");
   pyttsMod = py::module_::import("pyttsx3");

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
//recursively copies the contents of one directory to another
void PythonTtsEngine::copyDir(const QString &fromDir, const QString &toDir)
{
    QDirIterator it(fromDir, QDirIterator::Subdirectories);
    QDir dir(fromDir);
    const int absSourcePathLength = dir.absoluteFilePath(fromDir).length();
    while (it.hasNext()){
        it.next();
        const auto fileInfo = it.fileInfo();
        if(!fileInfo.isHidden())
        { //filters dot and dotdot
            const QString subPathStructure = fileInfo.absoluteFilePath().mid(absSourcePathLength);
            const QString constructedAbsolutePath = toDir + subPathStructure;
            if(fileInfo.isDir())
            {
                //Create directory in target folder
                dir.mkpath(constructedAbsolutePath);
            }
            else if(fileInfo.isFile())
            {
                //Copy File to target directory
                //Remove file at target location, if it exists, or QFile::copy will fail
                QFile::remove(constructedAbsolutePath);
                QFile::copy(fileInfo.absoluteFilePath(), constructedAbsolutePath);
            }
        }
    }
}

void PythonTtsEngine::installPythonLibrary(QString& resourcePath)
{
}
