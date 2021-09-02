#include "phraseinputform.h"
#include "ui_phraseinputform.h"

PhraseInputForm::PhraseInputForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PhraseInputForm)
{
    ui->setupUi(this);
    auto parentSize = parent->size();
    setMinimumSize(parentSize);
}

PhraseInputForm::~PhraseInputForm()
{
    delete ui;
}

void PhraseInputForm::on_addButton_clicked()
{
    auto newPair = newPairFromFields();
    addedPairs.push_back(newPair);
    ui->nativeLineEdit->clear();
    ui->targetLineEdit->clear();
}

PhrasePair PhraseInputForm::newPairFromFields()
{
    auto sNative = ui->nativeLineEdit->text();
    auto sTarget = ui->targetLineEdit->text();
    return PhrasePair(sNative, sTarget);
}
void PhraseInputForm::on_backButton_clicked()
{
    if(addedPairs.size() < 1)
    {
        return;
    }
    auto currentPhrase = addedPairs.back();
    addedPairs.pop_back();
    QString sNative = currentPhrase.nativePhrase.fullPhrase;
    QString sTarget = currentPhrase.targetPhrase.fullPhrase;
    ui->nativeLineEdit->setText(sNative);
    ui->targetLineEdit->setText(sTarget);
}

void PhraseInputForm::addDummyPairs(int pairsToAdd)
{
    for(int i = 0; i < pairsToAdd; ++i)
    {
        QString sNative = "Dummy Native Phrase Number " + QString::number(i);
        QString sTarget = "Dummy Target Phrase Number " + QString::number(i);
        addedPairs.push_back(PhrasePair(sNative, sTarget));
    }
}

void PhraseInputForm::on_createSetupButton_clicked()
{
    //addDummyPairs();
    Q_EMIT getPairList(addedPairs);
}


void PhraseInputForm::on_exitButton_clicked()
{
    Q_EMIT exitToDeck();
}

