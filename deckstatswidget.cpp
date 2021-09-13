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
#include <QDateTimeAxis>
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

    auto addedView = new QChartView(additionHistory(), this);
    printf("Added hinted height is: %d\n", addedView->sizeHint().height());
    addedView->setMinimumHeight(addedView->sizeHint().height());
    mainLayout->addWidget(addedView);
    //snapshot time
    snapshotBox = new QComboBox(this);
    auto names = DeckSnapshot::snapshotTypeNames();
    for (auto str : names)
    {
        snapshotBox->addItem(str);
    }
    mainLayout->addWidget(snapshotBox);
    connect(snapshotBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &DeckStatsWidget::setToSnapshotType);
    snapshotChartView = new QChartView(snapshotChartForType(TotalCards), this);
    mainLayout->addWidget(snapshotChartView);

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

QChart* DeckStatsWidget::snapshotChartForType(SnapshotType type)
{
    auto chart = new QChart();
    auto map = manager.snapshotGraph(type);
    auto name = DeckSnapshot::snapshotTypeNames()[(int)type];
    chart->setTitle(name);
    chart->legend()->hide();
    auto series = new QLineSeries(chart);
    double maxValue = 0.0f;
    int idx = 0;
    for(auto& value : map)
    {
        auto dTime = value.first.startOfDay();
        series->append(dTime.toMSecsSinceEpoch(), value.second);
    }
    chart->addSeries(series);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(map.size());
    axisX->setFormat("dd/MM");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText(name);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    return chart;
}

void DeckStatsWidget::setToSnapshotType(int type)
{
    snapshotChartView->setChart(snapshotChartForType((SnapshotType)type));
}
