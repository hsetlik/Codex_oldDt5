#ifndef CODEXMAINWINDOW_H
#define CODEXMAINWINDOW_H

#include <QMainWindow>
#include "masterstackedwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CodexMainWindow; }
QT_END_NAMESPACE

class CodexMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CodexMainWindow(QWidget *parent = nullptr);
    ~CodexMainWindow();

private:
    Ui::CodexMainWindow *ui;
};
#endif // CODEXMAINWINDOW_H
