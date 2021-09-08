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
    snapshots.append(getSnapshot(sourceDeck));
}

QJsonObject DeckStatsManager::getSnapshot(Deck* src)
{
    QJsonObject obj;
    obj["Date"] = QDateTime::currentDateTime().toString();
    obj["Deck"] = src->getDeckAsObject();
    return obj;
}

void DeckStatsManager::saveToFile()
{
    auto sDeckFile = "deck_stats/" + deckName + ".json";
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
    auto tDeck = new Deck(deckName);
    for (auto & card : tDeck->allCards)
    {
        float ease = (float)card->getEase();
        eases.push_back(ease);
    }
    delete tDeck;
    return eases;
}

std::vector<int> DeckStatsManager::getDailyAdditions(int range)
{
    std::vector<int> cardCounts;
    auto tDeck = new Deck(deckName);
    auto today = QDate::currentDate();
    auto earliest = tDeck->addEvents[0].timeAdded.date();
    auto startDate = (range == -1) ? earliest : today.addDays(-1 * range);
    auto numDays = (int)earliest.daysTo(today);
    for(int i =0; i < numDays; ++i)
    {
        auto date = startDate.addDays(i);
        cardCounts.push_back(tDeck->additionsOnDate(date));
    }
    return cardCounts;
}
