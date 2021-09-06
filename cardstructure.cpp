#include "cardstructure.h"
#include <memory>

//parameters for the SM-2 algorithm. Tweaked since Codex uses fewer answer rating options than the original Super Memo
#define SM2A 0.25f  //orig 0.1f
#define SM2B 0.06f //orig 0.08f
#define SM2C 0.04f //orig 0.02f
//===================================================================
//Implements the SuperMemo SM-2 spaced repitition algorithm described here: https://en.wikipedia.org/wiki/SuperMemo#Algorithms
void Card::updateWithAnswer(int answer)
{
    auto dEase = (SM2A - (4 - answer) * (SM2B + (4 - answer) * SM2C));
    easeLevel += dEase;
    if(easeLevel < 1.3f)
        easeLevel = 1.3f;
    // 0 <= answer < 4
    if(answer > 0)
    {
        if(timesAnswered == 0)
            intervalDays = 1;
        else if(timesAnswered == 1)
            intervalDays = 3;
        else
            intervalDays = (int)std::floor(intervalDays * easeLevel);
        ++timesAnswered;
    }
    else
    {
        timesAnswered = 0;
        intervalDays = 1;
    }
    setDueIn(intervalDays);
}
int Card::daysForAnswer(int answer)
{

    auto dEase = (SM2A - (4 - answer) * (SM2B + (4 - answer) * SM2C));
    auto tempEaseLevel = easeLevel + dEase;
    if(tempEaseLevel < 1.3f)
        tempEaseLevel = 1.3f;
    if(answer > 0)
    {
        if(timesAnswered == 0)
            return 1;
        else if(timesAnswered == 1)
            return 3;
        else
            return (int)std::floor(intervalDays * tempEaseLevel);
    }
    else
        return 1;
}
//===================================================================
NtaCard::NtaCard(QString native, QString target, PhrasePair* parent, bool flipped) :
    Card(parent, CardType::NTA),
    nativeWord(native),
    targetWord(target)
{
    //so that we can easily instantiate forward/reverse version of the card
    if(flipped)
    {
        auto oldTarget = targetWord;
        targetWord = nativeWord;
        nativeWord = oldTarget;
    }

}
NtaCard::NtaCard(QJsonObject& obj) :
    Card(obj),
    nativeWord(""),
    targetWord("")
{
    //printf("Creating NTA from JSON\n");
    auto qNative = obj["NativeWord"].toString();
    nativeWord = qNative;
    auto qTarget = obj["TargetWord"].toString();
    targetWord = qTarget;
    auto qDateString = obj["DateNextDue"].toString();
    dateNextDue = QDateTime::fromString(qDateString);
}
ClozeCard::ClozeCard(QString toRemove, PhrasePair* parent) :
    Card(parent, CardType::Cloze),
    clozeSentence(" "),
    answer(toRemove)
{
    fullTarget = parent->targetPhrase.fullPhrase;
    fullNative = parent->nativePhrase.fullPhrase;
    clozeSentence = fullTarget;
    auto clozeIdx = clozeSentence.indexOf(answer);
    for(int i = (int)clozeIdx; i < (int)(clozeIdx + answer.length()); ++i)
    {
        clozeSentence[i] = ' ';
    }
}
ClozeCard::ClozeCard(QJsonObject& obj) :
    Card(obj),
    clozeSentence(""),
    answer("")
{
    //printf("Creating Cloze from JSON\n");
    answer = obj["ClozeWord"].toString();
    fullTarget = obj["FullTarget"].toString();
    auto pairId = obj["ParentPairId"].toString();
    fullNative = pairId.remove(fullTarget);
    clozeSentence = fullTarget;
    auto clozeIdx = clozeSentence.indexOf(answer);
    for(int i = (int)clozeIdx; i < (int)(clozeIdx + answer.length()); ++i)
    {
        clozeSentence[i] = ' ';
    }
    auto qDateString = obj["DateNextDue"].toString();
    dateNextDue = QDateTime::fromString(qDateString);
}
FullCard::FullCard(PhrasePair* parent) :
    Card(parent, CardType::Full)
{
    fullNative = parent->nativePhrase.fullPhrase;
    fullTarget = parent->targetPhrase.fullPhrase;
}
FullCard::FullCard(QJsonObject& obj) :
    Card(obj)
{
    fullNative = obj["NativePhrase"].toString();
    fullTarget = obj["TargetPhrase"].toString();
    auto qDateString = obj["DateNextDue"].toString();
    dateNextDue = QDateTime::fromString(qDateString);
}
QJsonObject NtaCard::getJson()
{
    QJsonObject obj
    {
        {"CardType", "NTA"},
        {"ParentPairId", parentPairId},
        {"NativeWord", nativeWord},
        {"TargetWord", targetWord},
        {"DateNextDue", dateNextDue.toString()},
        {"TimesAnswered", timesAnswered},
        {"LastAnswer", lastAnswer},
        {"EaseLevel", easeLevel},
        {"IntervalDays", intervalDays}
    };
    return obj;
}
QJsonObject ClozeCard::getJson()
{
    QJsonObject obj
    {
        {"CardType", "Cloze"},
        {"ParentPairId", parentPairId},
        {"FullTarget", fullTarget},
        {"ClozeWord", answer},
        {"DateNextDue", dateNextDue.toString()},
        {"TimesAnswered", timesAnswered},
        {"LastAnswer", lastAnswer},
        {"EaseLevel", easeLevel},
        {"IntervalDays", intervalDays}
    };
    return obj;
}
QJsonObject FullCard::getJson()
{
    QJsonObject obj
    {
        {"CardType", "Full"},
        {"ParentPairId", parentPairId},
        {"NativePhrase", fullNative},
        {"TargetPhrase", fullTarget},
        {"DateNextDue", dateNextDue.toString()},
        {"TimesAnswered", timesAnswered},
        {"LastAnswer", lastAnswer},
        {"EaseLevel", easeLevel},
        {"IntervalDays", intervalDays}
    };
    return obj;
}
//===============================================================================
PhrasePairCards::PhrasePairCards(PhrasePair* pair) :
    full(nullptr)
{
    pairId = pair->getJsonIdString();
    fullNative = pair->nativePhrase.fullPhrase;
    fullTarget = pair->targetPhrase.fullPhrase;
    for(auto& nta : pair->ntaPairs)
    {
        ntaCards.push_back(std::make_unique<NtaCard>(nta.first, nta.second, pair));
        //add a flipped version of the cards so user gets asked both target->native and native->target
        ntaCards.push_back(std::make_unique<NtaCard>(nta.first, nta.second, pair, true));
    }
    for(auto & cloze : pair->clozeWords)
    {
        clozeCards.push_back(std::make_unique<ClozeCard>(cloze, pair));
    }
    if(pair->includesFull())
        full = std::make_unique<FullCard>(pair);
}
PhrasePairCards::PhrasePairCards(QJsonObject& obj) :
    full(nullptr)
{
    pairId = obj["PhrasePairId"].toString();
    fullNative = obj["NativePhrase"].toString();
    fullTarget = obj["TargetPhrase"].toString();
    auto ntaArray = obj["NtaCards"].toArray();
    for(int i = 0; i < ntaArray.size(); ++i)
    {
        //printf("Adding Nta Card\n");
        auto ntaObject = ntaArray[i].toObject();
        ntaCards.push_back(std::make_unique<NtaCard>(ntaObject));
    }
    auto clozeArray = obj["ClozeCards"].toArray();
    for(int i = 0; i < clozeArray.size(); ++i)
    {
        //printf("adding cloze card\n");
        auto clozeObject = clozeArray.at(i).toObject();
        clozeCards.push_back(std::make_unique<ClozeCard>(clozeObject));

    }
    if(obj.contains("FullCard"))
    {
        auto fullCardObject = obj["FullCard"].toObject();
        full = std::make_unique<FullCard>(fullCardObject);
    }
}
QJsonArray PhrasePairCards::getNtaJsons()
{
    QJsonArray ntaArray;
    for(auto& card : ntaCards)
    {
        auto newVal = QJsonValue(card->getJson());
        ntaArray.append(newVal);
    }
    return ntaArray;
}
QJsonArray PhrasePairCards::getClozeJsons()
{
    QJsonArray clozeArray;
    for(auto& cloze : clozeCards)
    {
        auto newVal = QJsonValue(cloze->getJson());
        clozeArray.append(newVal);
    }
    return clozeArray;
}
void PhrasePairCards::appendToDeckArray(QJsonArray &array)
{
    auto ntas = getNtaJsons();
    auto clozes = getClozeJsons();
    for(int i = 0; i < ntas.size(); ++i)
    {
        array.append(ntas[i]);
    }
    for(int i = 0; i < clozes.size(); ++i)
    {
        array.append(clozes[i]);
    }
    if(full != nullptr)
    {
        auto fullJson = full->getJson();
        array.append(fullJson);
    }
}
//add pointers to all the cards to the array
void PhrasePairCards::addAllToVector(std::vector<Card*>& array)
{
    for(auto& nta : ntaCards)
    {
        array.push_back(&*nta);
    }
    for(auto& cloze : clozeCards)
    {
        array.push_back(&*cloze);
    }
    if(full.get() != nullptr)
        array.push_back(&*full);
}
QJsonObject PhrasePairCards::getPairJson()
{
    QJsonObject obj
    {
        {"PhrasePairId", pairId},
        {"NativePhrase", fullNative},
        {"TargetPhrase", fullTarget}
    };
    obj["NtaCards"] = getNtaJsons();
    obj["ClozeCards"] = getClozeJsons();
    if(full != nullptr)
        obj["FullCard"] = full->getJson();
    return obj;
}
//===============================================================================
Deck::Deck(QString name) :
    deckName(name)
{
    //1. determine the deck file name
    auto sDeckFile = deckName + ".json";
    QString deckFileName = sDeckFile;
    //2. load file into ByteArray and by extension JSON
    QFile loadFile(deckFileName);
    if(!loadFile.open(QIODevice::ReadWrite))
        printf("File not loaded\n");
    if(loadFile.exists())
    {
        printf("File: %s exists\n", loadFile.fileName().toStdString().c_str());
    }
    QFileInfo info(loadFile);
    auto fullPath = info.absoluteFilePath().toStdString();
    printf("FULL FILE PATH: %s\n", fullPath.c_str());
    QByteArray deckData = loadFile.readAll();
    printf("File has size: %d bytes\n", deckData.size());
    QJsonDocument doc(QJsonDocument::fromJson(deckData));
    auto masterObject = doc.object();
    auto iNativeLocale = masterObject["NativeLocale"].toInt();
    nativeLocale = QLocale((QLocale::Language) iNativeLocale);
    qDebug() << "Native Locale: " << nativeLocale.nativeCountryName();
    auto iTargetLocale = masterObject["TargetLocale"].toInt();
    targetLocale = QLocale((QLocale::Language) iTargetLocale);
    qDebug() << "Target Locale: " << targetLocale.nativeCountryName();
    //the master deck JSON data is stored as an object w/ two properties: "DeckName" : string, and "PhrasePairs" : array of objects
    auto pairArray = masterObject["PhrasePairs"].toArray();
    printf("%d pairs found\n", pairArray.size());
    for(int i = 0; i < pairArray.size(); ++i)
    {
        auto pairObj = pairArray[i].toObject();
        addPhrasePairFrom(pairObj);
    }
    loadFile.close();
}
Deck::~Deck()
{
    saveToFile();
}
void Deck::saveToFile()
{
    //1. determine the deck file name
    auto sDeckFile = deckName + ".json";
    QString deckFileName = sDeckFile;
    //2. load file into ByteArray and by extension JSON
    QFile loadFile(deckFileName);
    if(!loadFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        printf("File not loaded\n");
    auto deckJsonDoc = QJsonDocument(getDeckAsObject());
    auto bytesWritten = loadFile.write(deckJsonDoc.toJson());
    printf("%lld bytes written to file\n", bytesWritten);
    loadFile.close();
}
int Deck::numDueToday()
{
     int numDue = 0;
     auto date = QDateTime::currentDateTime();
     for(int i = 0; i < (int)allCards.size(); ++i)
     {
         if (allCards[i]->isDue (date))
             ++numDue;
     }
     return numDue;
}
void Deck::addPhrasePairFrom(QJsonObject obj)
{

    phrasePairs.push_back(PhrasePairCards(obj));
    phrasePairs.back().addAllToVector(allCards);
}
std::vector<Card*> Deck::dueToday()
{
    std::vector<Card*> due;
    auto date = QDateTime::currentDateTime();
    for(int i = 0; i < (int)allCards.size(); ++i)
    {
        if (allCards[i]->isDue (date))
            due.push_back (allCards[i]);
    }
    return due;
}
QJsonObject Deck::getDeckAsObject()
{
    QJsonObject obj
    {
        {"DeckName", deckName},
        {"NativeLocale", (int)nativeLocale.language()},
        {"TargetLocale", (int)targetLocale.language()},
        {"PhrasePairs", getPairJsons()}
    };
    return obj;
}
QJsonArray Deck::getPairJsons()
{
    QJsonArray arr;
    for(auto& pair : phrasePairs)
    {
        auto pairObj = pair.getPairJson();
        arr.append(pairObj);
    }
    return arr;
}
void Deck::addNewPairs(QJsonArray newPairs)
{
    for(int i = 0; i < newPairs.size(); ++i)
    {
        auto pairObj = newPairs[i].toObject();
        addPhrasePairFrom(pairObj);
    }
}
void Deck::pushBackDueDates(int numDays)
{
    for(auto& card : allCards)
    {
        card->setDueIn(numDays * -1);
    }
}
void Deck::resetEaseFactors()
{
    for (auto& card : allCards)
        card->resetEase();
}
void Deck::resetAnswerHistories()
{
    for (auto & card : allCards)
        card->resetTimesAnswered();
}
void Deck::exportDeck(QDir& dir, QString newName, bool keepHistory)
{
    //make sure that we're exporting all new changes
    saveToFile();
    //create a temporary deck object to edit without affecting this object
    auto tempDeck = new Deck(deckName);
    tempDeck->setName(newName);
    if(!keepHistory)
    {
        tempDeck->resetAnswerHistories();
        tempDeck->resetEaseFactors();
        tempDeck->pushBackDueDates(0);
    }

    auto fullPath = dir.filePath(newName + ".json");

    QFile loadFile(fullPath);
    if(!loadFile.open(QIODevice::ReadWrite))
        printf("File not loaded\n");
    QJsonDocument deckDoc(tempDeck->getDeckAsObject());
    auto bytesWritten = loadFile.write(deckDoc.toJson());
    printf("%lld bytes exported to file\n", bytesWritten);
    loadFile.close();
    delete tempDeck;
}
