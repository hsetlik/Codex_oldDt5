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
    DeckStatsManager* const manager;
    void setGraphFor(SnapshotType type);
private Q_SLOTS:
    void typeBoxChanged(int typeIdx);
private:
    QChart* makeChartFor(SnapshotType type);
    QVBoxLayout* mainLayout;
    QComboBox* dataBox;
    QChart* currentChart;
    QChartView* currentChartView;
    SnapshotType currentType;
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
    QChart* easeCurveForCurrentDeck(std::vector<float>& easeData);
    QChart* additionHistory();
    Ui::DeckStatsWidget *ui;
    DeckStatsManager manager;
    int totalCards;
    QWidget* mainWidget;
    QVBoxLayout* mainLayout;
};

#endif // DECKSTATSWIDGET_H
