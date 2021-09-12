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
QChart* SnapshotChartFactory::getChartForType(SnapshotType type)
{
    auto chart = new QChart();
    //make sure this stays in the same order as the enum
    QString typeNames[3] = {"Total Cards", "Average Ease", "Average Interval"};
    chart->setTitle(typeNames[(int)type]);

    auto map = manager->getSnapshots(type);
    auto line = new QLineSeries(chart);
    int idx = 0;
    int yMax = 0;
    for (auto& snap : map)
    {
        if (snap.second > yMax)
            yMax = snap.second;
        QPointF point((double)idx, (double)snap.second);
        line->append(point);
    }
    chart->addSeries(line);

    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, idx - 1);
    chart->axes(Qt::Vertical).first()->setRange(0, yMax);

    return chart;
}
//=================================================================
DeckStatsWidget::DeckStatsWidget(QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeckStatsWidget),
    manager(name),
    totalCards(0),
    snapFactory(&manager)
{
    ui->setupUi(this);
    mainLayout = new QVBoxLayout;
    mainWidget = new QWidget(this);
    mainWidget->setMinimumHeight(1000);
    mainWidget->setLayout(mainLayout);
    mainWidget->setFocus(Qt::ActiveWindowFocusReason);
    ui->scrollArea->setWidget(mainWidget);
    ui->scrollArea->setWidgetResizable(true);
    auto lStr = name + " Deck Statistics";
    ui->deckNameLabel->setText(lStr);

    auto easeData = manager.latestCardEases();
    totalCards = (int)easeData.size();

    QString cardCountStr = "Total Cards: " + QString::number(totalCards);
    auto cardCountLabel = new QLabel(cardCountStr, this);
    mainLayout->addWidget(cardCountLabel);

    auto easeView = new QChartView(easeCurveForCurrentDeck(easeData), this);
    printf("Ease hinted height is: %d\n", easeView->sizeHint().height());
    easeView->setMinimumHeight(easeView->sizeHint().height());
    mainLayout->addWidget(easeView);
    std::vector<QString> typeNames = {"Total Cards", "Average Ease", "Average Interval"};
    snapBox = new QComboBox(this);
    for(auto& name : typeNames)
        snapBox->addItem(name);
    connect(snapBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &DeckStatsWidget::changeToSnapType);
    mainLayout->addWidget(snapBox);
    auto snapChart = snapFactory.getChartForType((SnapshotType)snapBox->currentIndex());
    snapView = new QChartView(snapChart, this);
    mainLayout->addWidget(snapView);

    auto addedView = new QChartView(additionHistory(), this);
    printf("Added hinted height is: %d\n", addedView->sizeHint().height());
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

QChart* DeckStatsWidget::easeCurveForCurrentDeck(std::vector<float>& easeData)
{
    //create the chart
    QChart *chart = new QChart();
    chart->setTitle("Current Card Ease");
    //grab the ease data from the manager
    std::sort(easeData.begin(), easeData.end());
    auto yMax = easeData.back();
    auto xMax = (int)easeData.size();
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

void DeckStatsWidget::changeToSnapType(int type)
{
    auto chart = snapFactory.getChartForType((SnapshotType)type);
    snapView->setChart(chart);
}

