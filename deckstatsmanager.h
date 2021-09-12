#ifndef DECKSTATSMANAGER_H
#define DECKSTATSMANAGER_H
#include "cardstructure.h"
enum SnapshotType
{
    TotalCards,
    AverageEase,
    AverageInterval
};
class DeckSnapshot
{
public:
    // constructor 1 creates a snapshot for *today* based on the data in the deck ptr
    DeckSnapshot(Deck* m);
    // constructor 2 reloads a saved snapshot
    DeckSnapshot(QJsonObject obj);
    QJsonObject getJsonObject();
private:
    QDate date;
    int totalCards;
    double avgEase;
    double avgInterval;
};
class DeckStatsManager
{
public:
    DeckStatsManager(QString name);
    ~DeckStatsManager();
    const QString deckName;
    void saveToFile();
    std::vector<float> latestCardEases();
    std::map<QDate, int> getAdditionHistory();
    // call this function on any deck to find and update its deck_stats.json file
    static void createSnapshot(Deck* deck);
private:
    Deck* currentDeck;
    std::vector<DeckSnapshot> snapshots;
    QJsonArray getSnapshotArray();
    void addSnapshot(Deck* deck)
    {
        snapshots.push_back(DeckSnapshot(deck));
    }
};

#endif // DECKSTATSMANAGER_H
