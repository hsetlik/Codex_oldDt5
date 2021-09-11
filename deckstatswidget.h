#ifndef DECKSTATSWIDGET_H
#define DECKSTATSWIDGET_H

#include <QWidget>
#include <QtCharts/QChartGlobal>
#include "deckstatsmanager.h"

namespace Ui
{
class DeckStatsWidget;
}

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE


class SnapshotChartFactory
{
public:
    SnapshotChartFactory(DeckStatsManager* m) : manager(m)
    {
    }
    DeckStatsManager* const manager;
    QChart* getChartForType(SnapshotType type);
};

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
    void changeToSnapType(int type);

private:
    QChart* easeCurveForCurrentDeck(std::vector<float>& easeData);
    QChart* additionHistory();
    Ui::DeckStatsWidget *ui;
    DeckStatsManager manager;
    int totalCards;
    QWidget* mainWidget;
    QVBoxLayout* mainLayout;
    QComboBox* snapBox;
    QChartView* snapView;
    SnapshotChartFactory snapFactory;
};

#endif // DECKSTATSWIDGET_H
