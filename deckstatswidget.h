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

class SnapshotGraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SnapshotGraphWidget(DeckStatsManager* m, QWidget* parent = nullptr);
    SnapshotGraphWidget(DeckStatsManager* m, const QString& property, QWidget* parent = nullptr);
    DeckStatsManager* const manager;
    void setGraphFor(const QString& property);
private:
    QChart* createChart(const QString& property);
    QWidget* currentGraph;
    QVBoxLayout* mainLayout;
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

private:
    QChart* easeCurveForCurrentDeck();
    QChart* additionHistory();
    Ui::DeckStatsWidget *ui;
    DeckStatsManager manager;
    int totalCards;
    QWidget* mainWidget;
    QVBoxLayout* mainLayout;
};

#endif // DECKSTATSWIDGET_H
