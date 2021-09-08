#include "deckstatswidget.h"
#include "ui_deckstatswidget.h"

DeckStatsWidget::DeckStatsWidget(QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeckStatsWidget),
    manager(name)
{
    ui->setupUi(this);
}

DeckStatsWidget::~DeckStatsWidget()
{
    delete ui;
}

void DeckStatsWidget::on_backButton_clicked()
{
    Q_EMIT exitToManager();
}

