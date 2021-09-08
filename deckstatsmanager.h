#ifndef DECKSTATSMANAGER_H
#define DECKSTATSMANAGER_H
#include "cardstructure.h"

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
    std::map<QDate, int> totalCardsTime();
private:
    Deck* currentDeck;
    QJsonArray snapshots;
};

#endif // DECKSTATSMANAGER_H
