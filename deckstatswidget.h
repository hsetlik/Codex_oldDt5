#ifndef DECKSTATSWIDGET_H
#define DECKSTATSWIDGET_H

#include <QWidget>
#include "deckstatsmanager.h"

namespace Ui {
class DeckStatsWidget;
}

class DeckStatsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeckStatsWidget(QString name, QWidget *parent = nullptr);
    ~DeckStatsWidget();
Q_SIGNALS:
    void exitToManager();

private Q_SLOTS:
    void on_backButton_clicked();

private:
    Ui::DeckStatsWidget *ui;
    DeckStatsManager manager;
};

#endif // DECKSTATSWIDGET_H
