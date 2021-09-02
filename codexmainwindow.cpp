#include "codexmainwindow.h"
#include "./ui_codexmainwindow.h"

CodexMainWindow::CodexMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CodexMainWindow)
{
    ui->setupUi(this);
    pybind11::exec("print('hello world')");
}

CodexMainWindow::~CodexMainWindow()
{
    delete ui;
}

