#ifndef CODEXSTYLE_H
#define CODEXSTYLE_H

#include <QProxyStyle>
#include <QObject>
#include <QWidget>

class CodexStyle : public QProxyStyle
{
    Q_OBJECT
public:
    CodexStyle();
};

#endif // CODEXSTYLE_H
