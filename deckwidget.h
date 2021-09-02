#ifndef DECKWIDGET_H
#define DECKWIDGET_H

#include <QWidget>
#include "cardstructure.h"

namespace Ui {
class DeckWidget;
}

class DeckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeckWidget(Deck* deck, QWidget *parent = nullptr);
    Deck* const linkedDeck;
    void updateLabels();
    ~DeckWidget();
Q_SIGNALS:
    void goToInputScreen();
    void studyScreenWith();
private Q_SLOTS:
    void on_addButton_clicked();

    void on_studyButton_clicked();

private:
    Ui::DeckWidget *ui;
};

#endif // DECKWIDGET_H
