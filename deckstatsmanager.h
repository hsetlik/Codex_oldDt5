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
    // constructor 2 reloads a saved snapshot from a parsed JSON file
    DeckSnapshot(QJsonObject obj);
    QJsonObject getJsonObject();
    QDate getDate() {return date; }
    int getTotalCards() {return totalCards; }
    double getAvgEase() {return avgEase; }
    double getAvgInterval() {return avgInterval; }
    static std::vector<QString> snapshotTypeNames()
    { //make sure these stay in same order as the enum as more data bits are added
        return
        {
            "Total Cards",
            "Average Ease",
            "Average Interval"
        };
    }
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
    //snapshot-related functions
    int numSnapshots() {return (int)snapshots.size(); }
    DeckSnapshot* snapshotAt(int idx) { return &snapshots[idx]; }
    std::map<QDate, double> snapshotGraph(SnapshotType type);
private:
    Deck* currentDeck;
    std::vector<DeckSnapshot> snapshots;
    QJsonArray getSnapshotArray();
    void addSnapshot(Deck* deck)
    {
        snapshots.push_back(DeckSnapshot(deck));
    }
    std::map<QDate, double> totalCardsSnap();
    std::map<QDate, double> avgEaseSnap();
    std::map<QDate, double> avgIntervalSnap();
};

#endif // DECKSTATSMANAGER_H
