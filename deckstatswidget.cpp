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
    mainLayout(nullptr),
    dataBox(nullptr),
    currentChart(nullptr),
    currentChartView(nullptr),
    currentType(TotalCards)
{
    mainLayout = new QVBoxLayout;
    dataBox = new QComboBox(this);
    const std::vector<QString> snapshotTypes =
    {
        "Total Cards",
        "Average Ease",
        "Average Interval"
    };
    for (auto& type  : snapshotTypes)
    {
        dataBox->addItem(type);
    }
    dataBox->setMaximumHeight(dataBox->sizeHint().height());
    mainLayout->addWidget(dataBox);
    setLayout(mainLayout);
    //connect the combo box
    connect(dataBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SnapshotGraphWidget::typeBoxChanged);
    //set the combo box to return to default value and call the setGraphFor signal
    dataBox->setCurrentIndex(0);
    setGraphFor(currentType);
}
void SnapshotGraphWidget::setGraphFor(SnapshotType type)
{
    //clear out the old graph
    if (currentChartView != nullptr)
    {
        mainLayout->removeWidget(currentChartView);
        delete currentChartView;
    }
    currentChart = makeChartFor(type);
    currentChartView = new QChartView(currentChart, this);
    mainLayout->addWidget(currentChartView);
    currentChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}
QChart* SnapshotGraphWidget::makeChartFor(SnapshotType type)
{
    const std::vector<QString> snapshotTypes =
    {
        "Total Cards",
        "Average Ease",
        "Average Interval"
    };
    const QString chartName = snapshotTypes[(int)type];
    auto chart = new QChart();
    chart->setTitle(chartName);
    auto map = manager->getSnapshots(type);
    auto series = new QLineSeries;
    int idx = 0;
    int yMax = 0;
    for (auto& snap : map)
    {
        if (snap.second > yMax)
            yMax = snap.second;
        printf("Snapshot %d value: %d", idx, snap.second);
        series->append((float)idx, (float)snap.second);
    }
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, (int)map.size() - 1);
    chart->axes(Qt::Vertical).first()->setRange(0, yMax);
    chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    printf("Snapshot chart height: %lf\n", chart->size().height());
    return chart;
}
void SnapshotGraphWidget::typeBoxChanged(int typeIdx)
{
    if (dataBox != nullptr)
    {
        if (typeIdx != (int)currentType)
        {
            currentType = (SnapshotType)typeIdx;
            setGraphFor(currentType);
        }
    }
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

    auto snapshotView = new SnapshotGraphWidget(&manager, this);
    printf("Snapshot hinted height is: %d\n", snapshotView->sizeHint().height());
    snapshotView->setMinimumHeight(snapshotView->sizeHint().height());
    mainLayout->addWidget(snapshotView);
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
    chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    printf("Ease chart height: %lf\n", chart->size().height());
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
    chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    printf("Addition chart height: %lf\n", chart->size().height());
    return chart;
}

