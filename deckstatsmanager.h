#ifndef DECKSTATSMANAGER_H
#define DECKSTATSMANAGER_H
#include "cardstructure.h"

class DeckStatsManager
{
public:
    DeckStatsManager(QString name);
    const QString deckName;
    void addSnapshot(Deck* sourceDeck);
    static QJsonObject getSnapshot(Deck* src);
    void saveToFile();
private:
    QJsonArray snapshots;
};

#endif // DECKSTATSMANAGER_H
