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
    void refreshLabel();
    QPushButton* deckButton;
    QLabel* dueLabel;
private:
    const QString deckName;

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
    void updateLabels();
public Q_SLOTS:
    void on_newDeckButton_clicked() { Q_EMIT launchNewDeckDialog(); }
    void on_manageDecksButon_clicked();
    void deckButtonClicked();
Q_SIGNALS:
    void launchDeckManager();
    void launchNewDeckDialog();
    void openDeck(QString name);
private:
    Ui::DeckListWidget *ui;
    std::vector<DeckMenuItem*> menuItems;
};

#endif // DECKLISTWIDGET_H
