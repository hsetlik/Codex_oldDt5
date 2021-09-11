#include "codexmainwindow.h"
#include "codexstyle.h"

#include <aws/core/Aws.h>
#include <aws/text-to-speech/TextToSpeechManager.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    QApplication a(argc, argv);
    a.setStyle(new CodexStyle);
    auto labelFont = a.font("QLabel");
    auto size = labelFont.pointSizeF();
    size *= 1.2f;
    labelFont.setPointSizeF(size);
    a.setFont(labelFont, "QLabel");
    CodexMainWindow w;
    w.show();
    auto value = a.exec();
    Aws::ShutdownAPI(options);
    return value;
}
