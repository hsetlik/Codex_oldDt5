#include "codexmainwindow.h"
#include "codexstyle.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new CodexStyle);
    CodexMainWindow w;
    w.show();
    return a.exec();
}
