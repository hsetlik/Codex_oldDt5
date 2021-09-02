#ifndef PHRASEINPUTFORM_H
#define PHRASEINPUTFORM_H

#include <QWidget>
#include "datastructures.h"

namespace Ui {
class PhraseInputForm;
}

class PhraseInputForm : public QWidget
{
    Q_OBJECT

public:
    explicit PhraseInputForm(QWidget *parent = nullptr);
    ~PhraseInputForm();
    PhrasePair newPairFromFields();
public Q_SLOTS:
    void on_addButton_clicked();

    void on_backButton_clicked();

    void on_createSetupButton_clicked();
Q_SIGNALS:
    void getPairList(std::vector<PhrasePair>& pairs);
    void exitToDeck();

private Q_SLOTS:
    void on_exitButton_clicked();

private:
    Ui::PhraseInputForm *ui;
    std::vector<PhrasePair> addedPairs;
    void addDummyPairs(int pairsToAdd = 10);

};

#endif // PHRASEINPUTFORM_H
