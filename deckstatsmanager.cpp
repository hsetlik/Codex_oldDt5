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
    auto sDeckFile = "deck_stats/" + deckName + "_stats.json";
    QString deckFileName = sDeckFile;
    QFile loadFile(deckFileName);
    if(!loadFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        printf("File not loaded\n");
    auto deckJsonDoc = QJsonDocument(snapshots);
    auto bytesWritten = loadFile.write(deckJsonDoc.toJson());
    printf("%lld bytes written to file\n", bytesWritten);
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
        auto tDeck = new Deck(obj);
        auto date = QDateTime::fromString(obj["Date"].toString()).date();
        int count = (int)tDeck->allCards.size();
        valueMap[date] = count;
        delete tDeck;
    }
    return valueMap;
}

