#ifndef DECKLISTWIDGET_H
#define DECKLISTWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "inputwidget.h"
#include "phraseinputform.h"
#include "deckwidget.h"
#include "cardwidget.h"

//basic widget with a button and a label for cards due
class DeckMenuItem : public QWidget
{
    Q_OBJECT
public:
    explicit DeckMenuItem(QString name, QWidget* parent = nullptr);
    QPushButton* deckButton;
    QLabel* dueLabel;

};
//==========================================================
namespace Ui {
class DeckListWidget;
}

class DeckListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeckListWidget(QWidget *parent = nullptr);
    ~DeckListWidget();
public Q_SLOTS:
    void on_newDeckButton_clicked() { Q_EMIT launchNewDeckDialog(); }
    void on_manageDecksButon_clicked();
    void deckButtonClicked();
Q_SIGNALS:
    void launchNewDeckDialog();
    void openDeck(QString name);
private:
    Ui::DeckListWidget *ui;
};

#endif // DECKLISTWIDGET_H
