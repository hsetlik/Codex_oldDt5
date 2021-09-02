#include "codexmainwindow.h"
#include "./ui_codexmainwindow.h"

CodexMainWindow::CodexMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CodexMainWindow)
{
    ui->setupUi(this);
    auto stack = new MasterStackedWidget();
    ui->verticalLayout->addWidget(stack);
}

CodexMainWindow::~CodexMainWindow()
{
    delete ui;
}

