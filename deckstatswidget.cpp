#include "deckstatswidget.h"
#include "ui_deckstatswidget.h"
//include all the relevant chart classes
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator>
#include <QtCharts/QBarCategoryAxis>
#include <QtWidgets/QApplication>
#include <QtCharts/QValueAxis>
//=================================================================
SnapshotGraphWidget::SnapshotGraphWidget(DeckStatsManager* m, QWidget* parent) :
    QWidget(parent),
    manager(m),
    currentGraph(nullptr)
{
    mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
}
SnapshotGraphWidget::SnapshotGraphWidget(DeckStatsManager* m, const QString& property, QWidget* parent) :
    QWidget(parent),
    manager(m),
    currentGraph(nullptr)
{
    mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    setGraphFor(property);
}
QChart* SnapshotGraphWidget::createChart(const QString& property)
{
    auto chart = new QChart();

    return chart;
}

void SnapshotGraphWidget::setGraphFor(const QString &property)
{

}
//=================================================================
DeckStatsWidget::DeckStatsWidget(QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeckStatsWidget),
    manager(name),
    totalCards(0)
{
    ui->setupUi(this);
    mainLayout = new QVBoxLayout;
    mainWidget = new QWidget(this);
    mainWidget->setMinimumHeight(800);
    mainWidget->setLayout(mainLayout);
    mainWidget->setFocus(Qt::ActiveWindowFocusReason);
    ui->scrollArea->setWidget(mainWidget);
    ui->scrollArea->setWidgetResizable(true);
    auto lStr = name + " Deck Statistics";
    ui->deckNameLabel->setText(lStr);

    QString cardCountStr = "Total Cards: " + QString::number(totalCards);
    auto cardCountLabel = new QLabel(cardCountStr, this);
    mainLayout->addWidget(cardCountLabel);

    auto easeView = new QChartView(easeCurveForCurrentDeck(), this);
    easeView->setMinimumHeight(easeView->sizeHint().height());
    mainLayout->addWidget(easeView);

    auto addedView = new QChartView(additionHistory(), this);
    addedView->setMinimumHeight(addedView->sizeHint().height());
    mainLayout->addWidget(addedView);
}

DeckStatsWidget::~DeckStatsWidget()
{
    delete ui;
}

void DeckStatsWidget::on_backButton_clicked()
{
    Q_EMIT exitToManager();
}

QChart* DeckStatsWidget::easeCurveForCurrentDeck()
{
    //create the chart
    QChart *chart = new QChart();
    chart->setTitle("Current Card Ease");
    //grab the ease data from the manager
    auto easeData = manager.latestCardEases();
    std::sort(easeData.begin(), easeData.end());
    auto yMax = easeData.back();
    auto xMax = (int)easeData.size();
    totalCards = xMax;
    //create line series
    auto lowerLine = new QLineSeries(chart);
    auto upperLine = new QLineSeries(chart);
    //populate lines
    for (int i = 0; i < xMax; ++i)
    {
        QPointF upper((float)i, easeData[i]);
        QPointF lower((float)i, 0.0f);
        upperLine->append(upper);
        lowerLine->append(lower);
    }
    //create area
    QAreaSeries *area = new QAreaSeries(upperLine, lowerLine);
    chart->addSeries(area);
    //set up axes
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, xMax - 1);
    chart->axes(Qt::Vertical).first()->setRange(0, yMax);
    chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    return chart;
}
QChart* DeckStatsWidget::additionHistory()
{
    QChart *chart = new QChart();
    chart->setTitle("Cards Added");
    auto addMap = manager.getAdditionHistory();
    QBarSeries* series = new QBarSeries(chart);
    auto set = new QBarSet("");
    int maxValue = std::numeric_limits<int>::min();
    for(auto& date : addMap)
    {
        if(date.second > maxValue)
            maxValue = date.second;
        *set << date.second;
    }
    series->append(set);
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(0, maxValue);
    chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return chart;
}

