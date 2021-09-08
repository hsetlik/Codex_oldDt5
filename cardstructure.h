#ifndef CARDSTRUCTURE_H
#define CARDSTRUCTURE_H

#include "datastructures.h"
#include <QtWidgets>

/*
 * TODO LIST updated 9/1:
 * -Add comparison for text entry cards (cloze and full)
 * -Add automatic reversals for NTA cards
 */
enum class CardType
{
    NTA,
    Cloze,
    Audio,
    Full
};
struct Card
{
    Card(PhrasePair* pair, CardType type) :
        cardType(type),
        timesAnswered(0),
        lastAnswer(0),
        easeLevel(2.5f),
        intervalDays(1)
    {
        dateCreated = QDateTime::currentDateTime();
        dateNextDue = QDateTime::currentDateTime();
        parentPairId = pair->getJsonIdString();
    }
    Card(QJsonObject& obj) : cardType(getCardType(obj))
    {
        auto dateString = obj["DateNextDue"].toString();
        dateNextDue = QDateTime::fromString(dateString);
        parentPairId = obj["ParentPairId"].toString();
        timesAnswered = obj["TimesAnswered"].toInt();
        lastAnswer = obj["LastAnswer"].toInt();
        easeLevel = obj["EaseLevel"].toDouble();
        intervalDays = obj["IntervalDays"].toInt();
        if(obj.contains("DateCreated"))
        {
            auto createdString = obj["DateCreated"].toString();
            dateCreated = QDateTime::fromString(createdString);
        }
        else
            dateCreated = QDateTime::currentDateTime();

    }
    static CardType getCardType(QJsonObject& obj)
    {
        auto typeString = obj["CardType"].toString();
        if(typeString == "NTA")
            return CardType::NTA;
        else if(typeString == "Cloze")
            return CardType::Cloze;
        else
            return CardType::Full;
    }
    virtual ~Card() {}
    const CardType cardType;
    //virtual functions to be generated by the card subclasses
    //not pure virtual bc I need to be able to have template containers of type 'Card' but these are junk return values
    virtual QString getFrontData() {return "null"; }
    virtual QString getBackData() {return "null"; }
    //gives JSON object
    virtual QJsonObject getJson()
    {
        QJsonObject obj
        {
            {"Null", 0}
        };
        return obj;
    }
    int daysForAnswer(int answer);
    void updateWithAnswer(int answer);
    void setDueIn(int numDays)
    {
        auto date = QDateTime::currentDateTime();
        dateNextDue = date.addDays(numDays);
    }
    bool isDue(QDateTime current)
    {
        return current >= dateNextDue;
    }
    int getTimesAnswered() {return timesAnswered; }
    int getLastAnswer() {return lastAnswer; }
    QDateTime createdOn() {return dateCreated;}
    double getEase() {return easeLevel; }
    void resetEase() {easeLevel = 2.5f; }
    void resetTimesAnswered() {timesAnswered = 0; }
protected:
    QDateTime dateCreated;
    QDateTime dateNextDue;
    QString parentPairId;
    int timesAnswered;
    int lastAnswer;
    double easeLevel;
    int intervalDays;
};
//=================================================================================
struct NtaCard : public Card
{
public:
    NtaCard(QString native, QString target, PhrasePair* parent, bool flipped=false);
    NtaCard(QJsonObject& obj);
    NtaCard getInverse()
    {
        NtaCard card(*this);
        card.invert();
        return card;
    }
    QString getFrontData() override {return nativeWord; }
    QString getBackData() override {return targetWord; }
    QJsonObject getJson() override;
private:
    QString nativeWord;
    QString targetWord;
    void invert()
    {
        auto oldNative = nativeWord;
        nativeWord = targetWord;
        targetWord = oldNative;
    }
};
//=================================================================================
struct ClozeCard : public Card
{
public:
    ClozeCard(QString toRemove, PhrasePair* parent);
    ClozeCard(QJsonObject& obj);
    QString getFrontData() override {return clozeSentence; }
    QString getBackData() override {return answer; }
    QJsonObject getJson() override;
    QString getFullTarget(){return fullTarget; }
    QString getFullNative(){return fullNative; }
private:
    QString clozeSentence;
    QString answer;
    QString fullTarget;
    QString fullNative;
};
//=================================================================================
struct FullCard : public Card
{
public:
    FullCard(PhrasePair* parent);
    FullCard(QJsonObject& obj);
    QString getFrontData() override {return fullNative; }
    QString getBackData() override {return fullTarget; }
    QJsonObject getJson() override;
private:
    QString fullNative;
    QString fullTarget;
};
//=================================================================================
struct PhrasePairCards
{
    PhrasePairCards(PhrasePair* parent);
    PhrasePairCards(QJsonObject& obj);
    //store all the cards as unique_ptrs and only instantiate card subclasses inside the PhrasePairCards class
    //Outside of this class, all access to and manipulation of card objects should be done via pointer to object of this class
    std::vector<std::unique_ptr<NtaCard>> ntaCards;
    std::vector<std::unique_ptr<ClozeCard>> clozeCards;
    std::unique_ptr<FullCard> full;
    QJsonObject getPairJson();
    void addAllToVector(std::vector<Card*>& allCards);
private:
    //adds all the cards to an external array for storing a deck of multiple phrase pairs
    void appendToDeckArray(QJsonArray& array);
    QJsonArray getNtaJsons();
    QJsonArray getClozeJsons();
    QString pairId;
    QString fullNative;
    QString fullTarget;
};
//================================================================================
//Full deck data structure, includes functionality for storing as file JSON
class Deck
{
public:
    Deck(QString name = "default_deck");
    Deck(QJsonObject obj);
    ~Deck();
    void addPhrasePairFrom(QJsonObject obj);
    std::vector<PhrasePairCards> phrasePairs;
    std::vector<Card*> allCards;
    std::vector<Card*> dueToday();
    int numDueToday();
    void addNewPairs(QJsonArray newPairs);
    void saveToFile();
    QString getName() {return deckName; }
    void pushBackDueDates(int numDays);
    void resetEaseFactors();
    void resetAnswerHistories();
    QLocale getNativeLocale() {return nativeLocale; }
    QLocale getTargetLocale() {return targetLocale; }
    void exportDeck(QDir& dir, QString newName, bool keepHistory);
    void setName(QString str) {deckName = str; }
    QDateTime createdOn() {return dateCreated; }
    QJsonObject getDeckAsObject();
    QJsonArray getAdditionsArray();
private:
    QString deckName;
    QJsonArray getPairJsons();
    QLocale nativeLocale;
    QLocale targetLocale;
    QDateTime dateCreated;
};
#endif // CARDSTRUCTURE_H
