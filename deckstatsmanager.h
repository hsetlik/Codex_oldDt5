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
    static QJsonObject getSnapshot(Deck* src);
    void saveToFile();
    std::vector<float> latestCardEases();
    std::map<QDate, int> getAdditionHistory();
private:
    Deck* currentDeck;
    QJsonArray snapshots;
};

#endif // DECKSTATSMANAGER_H
