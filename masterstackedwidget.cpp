#include "masterstackedwidget.h"

MasterStackedWidget::MasterStackedWidget(QWidget *parent) :
    QStackedWidget(parent),
    currentDeck(nullptr)
{
    deckMenuScreen = new DeckListWidget(this);
    addWidget(deckMenuScreen);
    setCurrentWidget(deckMenuScreen);
    connect(deckMenuScreen, &DeckListWidget::openDeck, this, &MasterStackedWidget::openDeckWithName);
    connect(deckMenuScreen, &DeckListWidget::launchNewDeckDialog, this, &MasterStackedWidget::switchToDeckCreatorView);
    connect(deckMenuScreen, &DeckListWidget::launchDeckManager, this, &MasterStackedWidget::switchToDeckManager);
}

void MasterStackedWidget::switchToCardEditors()
{
    setCurrentWidget(editorScreen);
}
void MasterStackedWidget::switchToStudyView()
{
    studyScreen = new CardWidget(&*currentDeck, this);
    connect(studyScreen, &CardWidget::dueCardsFinished, this, &MasterStackedWidget::switchToDeckView);
    addWidget(studyScreen);
    setCurrentWidget(studyScreen);
}
void MasterStackedWidget::finishAddingCards(QJsonArray pairs)
{
    currentDeck->addNewPairs(pairs);
    currentDeck->saveToFile();
    auto name = currentDeck->getName();
    openDeckWithName(name);
}

void MasterStackedWidget::switchToPhraseInput()
{
    setCurrentWidget(phraseScreen);
}

void MasterStackedWidget::openDeckWithName(QString name)
{
    //make sure any changes in the open deck are saved to disk before we take it out of memory
    if(currentDeck != nullptr)
        currentDeck->saveToFile();
    currentDeck.reset(new Deck(name));
    //! note this is only here for debuging
    //currentDeck->pushBackDueDates(3);
    auto cardsDue = currentDeck->numDueToday();
    printf("%d cards due today\n", cardsDue);
    deckScreen = new DeckWidget(&*currentDeck, this);
    addWidget(deckScreen);
    setCurrentWidget(deckScreen);
    phraseScreen = new PhraseInputForm(this);
    editorScreen = new InputWidget(this);
    addWidget(editorScreen);
    addWidget(phraseScreen);
    connect(deckScreen, &DeckWidget::goToInputScreen, this, &MasterStackedWidget::switchToPhraseInput);
    connect(deckScreen, &DeckWidget::goBack, this, &MasterStackedWidget::switchToDeckList);
    connect(phraseScreen, &PhraseInputForm::getPairList, editorScreen, &InputWidget::prepareEditorsFor);
    connect(phraseScreen, &PhraseInputForm::getPairList, this, &MasterStackedWidget::switchToCardEditors);
    connect(phraseScreen, &PhraseInputForm::exitToDeck, this, &MasterStackedWidget::switchToDeckView);
    connect(editorScreen, &InputWidget::returnNewPairCards, this, &MasterStackedWidget::finishAddingCards);
    connect(deckScreen, &DeckWidget::studyScreenWith, this, &MasterStackedWidget::switchToStudyView);
}

void MasterStackedWidget::createNewDeck(QLocale native, QLocale target, QString name)
{
    printf("Native Locale is: %s\n", native.nativeCountryName().toStdString().c_str());
    printf("Target Locale is: %s\n", target.nativeCountryName().toStdString().c_str());
    //printf("Deck Name is: %s\n", name);
    auto sDeckFile = name + ".json";
    QString deckFileName = sDeckFile;
    QFile loadFile(deckFileName);
    if(!loadFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        printf("File not loaded\n");
    QJsonObject deckInfo;
    deckInfo["DeckName"] = name;
    deckInfo["NativeLocale"] = (int)native.language();
    deckInfo["TargetLocale"] = (int)target.language();
    QJsonArray array;
    //just set up an empty array for the phrase pairs
    deckInfo["PhrasePairs"] = array;
    QJsonDocument deckDoc(deckInfo);
    //write the document to the file
    auto bytesWritten = loadFile.write(deckDoc.toJson());
    printf("%lld bytes written to file\n", bytesWritten);
    loadFile.close();
    //now reload the current Deck object and deck list widget
    currentDeck.reset(new Deck(name));
    removeWidget(deckMenuScreen);
    delete deckMenuScreen;
    //just re-run code from constructor here...
    deckMenuScreen = new DeckListWidget(this);
    addWidget(deckMenuScreen);
    setCurrentWidget(deckMenuScreen);
    connect(deckMenuScreen, &DeckListWidget::openDeck, this, &MasterStackedWidget::openDeckWithName);
    connect(deckMenuScreen, &DeckListWidget::launchNewDeckDialog, this, &MasterStackedWidget::switchToDeckCreatorView);
    connect(deckMenuScreen, &DeckListWidget::launchDeckManager, this, &MasterStackedWidget::switchToDeckManager);
}

void MasterStackedWidget::switchToDeckList()
{
    if(currentDeck != nullptr)
        currentDeck->saveToFile();
    deckMenuScreen->updateLabels();
    setCurrentWidget(deckMenuScreen);
}

void MasterStackedWidget::switchToDeckManager()
{
    deckManager = new DeckManagerWidget(this);
    addWidget(deckManager);
    setCurrentWidget(deckManager);
    connect(deckManager, &DeckManagerWidget::exitDeckManager, this, &MasterStackedWidget::switchToDeckList);
}
