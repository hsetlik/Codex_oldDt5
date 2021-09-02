#include "codexmainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    pybind11::scoped_interpreter guard{};
    QApplication a(argc, argv);
    CodexMainWindow w;
    w.show();
    return a.exec();
}
