#ifndef MASTERSTACKEDWIDGET_H
#define MASTERSTACKEDWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "deckmanagerwidget.h"
#include "deckstatswidget.h"

class MasterStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit MasterStackedWidget(QWidget *parent = nullptr);
    bool stackContains(QWidget* w)
    {
        for(int i = 0; i < count(); ++i)
        {
            if(widget(i) == w)
                return true;
        }
        return false;
    }
private Q_SLOTS:
    void switchToDeckCreatorView()
    {
        deckCreator = new DeckCreatorWidget(this);
        addWidget(deckCreator);
        setCurrentWidget(deckCreator);
        connect(deckCreator, &DeckCreatorWidget::newDeck, this, &MasterStackedWidget::createNewDeck);
    }
    void switchToCardEditors();
    void switchToPhraseInput();
    void switchToStudyView();
    void switchToDeckView()
    {
        setCurrentWidget(deckScreen);
        deckScreen->updateLabels();
    }
    void switchToDeckManager();
    void switchToDeckList();
    void createNewDeck(QLocale native, QLocale target, QString name);
    void exitStatScreen();
public Q_SLOTS:
    void finishAddingCards(QJsonArray pairs);
    void openDeckWithName(QString name);
    void statScreenWithName(QString name);
private:
    std::unique_ptr<Deck> currentDeck;
    DeckListWidget* deckMenuScreen;
    DeckCreatorWidget* deckCreator;
    DeckManagerWidget* deckManager;
    DeckWidget* deckScreen;
    CardWidget* studyScreen;
    PhraseInputForm* phraseScreen;
    InputWidget* editorScreen;
    DeckStatsWidget* statScreen;
Q_SIGNALS:

};

#endif // MASTERSTACKEDWIDGET_H
