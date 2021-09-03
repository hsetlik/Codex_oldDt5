#include "deckwidget.h"
#include "ui_deckwidget.h"

DeckWidget::DeckWidget(Deck* deck, QWidget *parent) :
    QWidget(parent),
    linkedDeck(deck),
    ui(new Ui::DeckWidget)
{
    ui->setupUi(this);
    updateLabels();
}

DeckWidget::~DeckWidget()
{
    delete ui;
}

void DeckWidget::on_addButton_clicked()
{
    Q_EMIT goToInputScreen();
}

void DeckWidget::updateLabels()
{
    QString name = linkedDeck->getName();
    auto numDue = linkedDeck->numDueToday();
    if(numDue < 1)
        ui->studyButton->setEnabled(false);
    else
        ui->studyButton->setEnabled(true);
    QString sNumDue = QString::number(numDue);
    ui->cardsDueValue->setText(sNumDue);
    ui->nameValue->setText(name);
}
void DeckWidget::on_studyButton_clicked()
{
    Q_EMIT studyScreenWith();
}


void DeckWidget::on_backButton_clicked()
{
    Q_EMIT goBack();
}

