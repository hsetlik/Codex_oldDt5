#include "deckstatsmanager.h"

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
         snapshots = doc.array();
    }
    currentDeck = new Deck(deckName);
}

DeckStatsManager::~DeckStatsManager()
{
    saveToFile();
}
void DeckStatsManager::addSnapshot(Deck* sourceDeck)
{
    auto shot = getSnapshot(sourceDeck);
    auto snapDate = QDateTime::fromString(shot["Date"].toString()).date();
    //make sure we don't end up with multiple snapshots recorded on the same day
    removeSnapshotOnDate(snapDate);
    snapshots.append(shot);
}

QJsonObject DeckStatsManager::getSnapshot(Deck* const src)
{
    QJsonObject obj;
    obj["Date"] = QDateTime::currentDateTime().toString();
    obj["Deck"] = src->getDeckAsObject();
    return obj;
}

void DeckStatsManager::saveToFile()
{
    QString deckFileName = "deck_stats/" + deckName + "_stats.json";
    QFile loadFile(deckFileName);
    if(!loadFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        printf("File not loaded\n");
    auto deckJsonDoc = QJsonDocument(snapshots);
    auto bytesWritten = loadFile.write(deckJsonDoc.toJson());
    printf("%lld bytes written to snapshot file: \"%s\"\n", bytesWritten, deckFileName.toStdString().c_str());
    loadFile.close();
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

void DeckStatsManager::addSnapshot(Deck* sourceDeck, QString name)
{
    auto manager = new DeckStatsManager(name);
    manager->addSnapshot(sourceDeck);
    delete manager;
}

std::map<QDate, int> DeckStatsManager::snapshotNumCards()
{
    std::map<QDate, int> valueMap;
    for(int i = 0; i < snapshots.size(); ++i)
    {
        auto obj = snapshots[i].toObject();
        auto tDeck = new Deck(obj["Deck"].toObject());
        auto date = QDateTime::fromString(obj["Date"].toString()).date();
        int count = (int)tDeck->allCards.size();
        valueMap[date] = count;
        delete tDeck;
    }
    return valueMap;
}

std::map<QDate, int> DeckStatsManager::snapshotAvgEase()
{
    std::map<QDate, int> valueMap;
    for(int i = 0; i < snapshots.size(); ++i)
    {
        auto obj = snapshots[i].toObject();
        auto tDeck = new Deck(obj["Deck"].toObject());
        auto date = QDateTime::fromString(obj["Date"].toString()).date();
        //figure out the total ease
        double totalEase = 0.0f;
        for (auto& card : tDeck->allCards)
        {
            totalEase += card->getEase();
        }
        int percentageEase = (int)(totalEase / (double)tDeck->allCards.size()) * 100.0f;
        valueMap[date] = percentageEase;
        delete tDeck;
    }
    return valueMap;
}

std::map<QDate, int> DeckStatsManager::snapshotAvgInterval()
{
    std::map<QDate, int> valueMap;
    for(int i = 0; i < snapshots.size(); ++i)
    {
        auto obj = snapshots[i].toObject();
        auto tDeck = new Deck(obj["Deck"].toObject());
        auto date = QDateTime::fromString(obj["Date"].toString()).date();
        //figure out the total ease
        int totalInterval = 0;
        for (auto& card : tDeck->allCards)
        {
            totalInterval += card->getCurrentInterval();
        }
        int avgInterval = (int)totalInterval / tDeck->allCards.size();
        valueMap[date] = avgInterval;
        delete tDeck;
    }
    return valueMap;
}

std::map<QDate, int> DeckStatsManager::getSnapshots(SnapshotType type)
{
    //one main public function which call the approproate private function for the type
    switch (type)
    {
        case TotalCards:
            return snapshotNumCards();
        case AverageEase:
            return snapshotAvgEase();
        case AverageInterval:
            return snapshotAvgInterval();
        default:
            return snapshotNumCards();
    }
}

bool DeckStatsManager::hasSnapshotOnDate(QDate date)
{
    for(int i = 0; i < snapshots.size(); ++i)
    {
        auto snapDateStr = snapshots[i].toObject()["Date"].toString();
        auto snapDate = QDateTime::fromString(snapDateStr).date();
        if (date == snapDate)
            return true;
    }
    return false;
}
// fine to call this unchecked on empty dates
 void DeckStatsManager::removeSnapshotOnDate(QDate date)
 {
     for(auto it = snapshots.begin(); it != snapshots.end(); ++it)
     {
         auto snapDateStr = it->toObject()["Date"].toString();
         auto snapDate = QDateTime::fromString(snapDateStr).date();
         if (date == snapDate)
         {
             snapshots.erase(it);
             return;
         }
     }
 }
