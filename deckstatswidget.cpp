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

DeckStatsWidget::DeckStatsWidget(QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeckStatsWidget),
    manager(name),
    totalCards(0)
{
    ui->setupUi(this);
    auto lStr = name + " Deck Statistics";
    ui->nameLabel->setText(lStr);
    auto easeView = new QChartView(easeCurveForCurrentDeck(), this);
    ui->mainLayout->addWidget(easeView);
    auto addedView = new QChartView(additionHistory(), this);
    ui->mainLayout->addWidget(addedView);
    QString cardCountStr = "Total Cards: " + QString::number(totalCards);
    auto cardCountLabel = new QLabel(cardCountStr, this);
    ui->mainLayout->addWidget(cardCountLabel);
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
    return chart;
}
