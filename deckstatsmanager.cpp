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
    //TODO
    return eases;
}
