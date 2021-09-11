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
    DeckStatsManager(QString name);
    ~DeckStatsManager();
    const QString deckName;
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
