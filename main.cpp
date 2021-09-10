#include "codexmainwindow.h"
#include "codexstyle.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new CodexStyle);
    auto labelFont = a.font("QLabel");
    auto size = labelFont.pointSizeF();
    size *= 1.2f;
    labelFont.setPointSizeF(size);
    a.setFont(labelFont, "QLabel");
    CodexMainWindow w;
    w.show();
    return a.exec();
}
