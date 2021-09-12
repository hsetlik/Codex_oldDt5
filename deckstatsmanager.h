#ifndef DECKSTATSMANAGER_H
#define DECKSTATSMANAGER_H
#include "cardstructure.h"
enum SnapshotType
{
    TotalCards,
    AverageEase,
    AverageInterval
};
class DeckStatsManager
{
public:
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
    DeckStatsManager(QString name);
    ~DeckStatsManager();
    const QString deckName;
    static std::vector<DeckSnapshot> fromJsonArray(QJsonArray arr);
    void addSnapshot(Deck* sourceDeck);
    static void addSnapshot(Deck* sourceDeck, QString name);
    QJsonObject getSnapshot(Deck* const src);
    void saveToFile();
    std::vector<float> latestCardEases();
    std::map<QDate, int> getAdditionHistory();
    std::map<QDate, int> getSnapshots(SnapshotType type);
private:
    std::map<QDate, int> snapshotNumCards();
    std::map<QDate, int> snapshotAvgEase();
    std::map<QDate, int> snapshotAvgInterval();
    bool hasSnapshotOnDate(QDate date);
    void removeSnapshotOnDate(QDate date);
    Deck* currentDeck;
    QJsonArray snapshots;
};

#endif // DECKSTATSMANAGER_H
