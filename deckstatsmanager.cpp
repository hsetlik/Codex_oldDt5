#include "deckstatsmanager.h"

DeckSnapshot::DeckSnapshot(Deck* deck)
{
    //this constructor should only be used when creating a new snapshot on the current day
    date = QDate::currentDate();
    // 1. calculate total cards
    totalCards = (int)deck->allCards.size();
    // 2. get the average ease & average interval
    double totalEase = 0.0f;
    double totalInterval = 0.0f;
    for (auto& card : deck->allCards)
    {
        totalEase += card->getEase();
        totalInterval += (double)card->getCurrentInterval();
    }
    avgEase = (double)(totalEase / totalCards);
    avgInterval = (double)(totalInterval / totalCards);
}

DeckSnapshot::DeckSnapshot(QJsonObject obj)
{
    //this corresponds to getJsonObject
    date = QDate::fromString(obj["Date"].toString());
    totalCards = obj["TotalCards"].toInt();
    avgEase = obj["AverageEase"].toDouble();
    avgInterval = obj["AverageInterval"].toDouble();
}
QJsonObject DeckSnapshot::getJsonObject()
{
    QJsonObject obj;
    obj["Date"] = date.toString();
    obj["TotalCards"] = totalCards;
    obj["AverageEase"] = avgEase;
    obj["AverageInterval"] = avgInterval;
    return obj;
}
//==================================================================
DeckStatsManager::DeckStatsManager(QString name) :
    deckName(name)
{
    if(!QDir::current().mkdir("deck_stats"))
        printf("Stats folder found\n");
    QString fileName = name + "_stats.json";
    auto filePath = "deck_stats/" + fileName;
    QFile statsFile(filePath);
    if(statsFile.open(QIODevice::ReadWrite))
    {
        auto byteData = statsFile.readAll();
         QJsonDocument doc(QJsonDocument::fromJson(byteData));
         auto snapObjects = doc.array();
         for (int i = 0; i < snapObjects.size(); ++i)
         {
             auto obj = snapObjects[i].toObject();
             snapshots.push_back(DeckSnapshot(obj));
         }
    }
    currentDeck = new Deck(deckName);
}

QJsonArray DeckStatsManager::getSnapshotArray()
{
    QJsonArray arr;
    for (auto& snap : snapshots)
    {
        arr.append(snap.getJsonObject());
    }
    return arr;
}

DeckStatsManager::~DeckStatsManager()
{
    saveToFile();
}

void DeckStatsManager::saveToFile()
{
    QString deckFileName = "deck_stats/" + deckName + "_stats.json";
    QFile loadFile(deckFileName);
    if(!loadFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        printf("File not loaded\n");

    auto deckJsonDoc = QJsonDocument(getSnapshotArray());
    auto bytesWritten = loadFile.write(deckJsonDoc.toJson());
    printf("%lld bytes written to snapshot file: \"%s\"\n", bytesWritten, deckFileName.toStdString().c_str());
    loadFile.close();
}

void DeckStatsManager::createSnapshot(Deck* deck)
{
    DeckStatsManager manager(deck->getName());
    manager.addSnapshot(deck);
    manager.saveToFile();
}

std::vector<float> DeckStatsManager::latestCardEases()
{
    std::vector<float> eases;
    for (auto & card : currentDeck->allCards)
    {
        float ease = (float)card->getEase();
        eases.push_back(ease);
    }
    return eases;
}

std::map<QDate, int> DeckStatsManager::getAdditionHistory()
{
    //map of all the relevant dates
    std::map<QDate, int> map;
    QDate current = QDate::currentDate();
    QDate finish = currentDeck->createdOn().date();
    while(current > finish)
    {
        map[current] = 0;
        current = current.addDays(-1);
    }
    //check when each card was added and increment the correct date
    for (auto& card : currentDeck->allCards)
    {
        auto date = card->createdOn().date();
        map[date] += 1;
    }
    return map;
}
