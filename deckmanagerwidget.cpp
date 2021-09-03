#include "deckmanagerwidget.h"
#include "ui_deckmanagerwidget.h"


DeckManagerRow::DeckManagerRow(QString name, QWidget* parent) :
    QWidget(parent),
    deckName(name)
{
    //do the layout
    auto layout = new QHBoxLayout;
    deckNameLabel = new QLabel(deckName, this);
    exportButton = new QPushButton("Export Deck", this);
    viewStatsButton = new QPushButton("View Stats", this);
    layout->addWidget(deckNameLabel);
    layout->addWidget(exportButton);
    layout->addWidget(viewStatsButton);
    setLayout(layout);
    //set up connections
    connect(exportButton, &QPushButton::clicked, this, &DeckManagerRow::on_exportClick);
    connect(viewStatsButton, &QPushButton::clicked, this, &DeckManagerRow::on_statsClick);
}

//======================================================
DeckManagerWidget::DeckManagerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeckManagerWidget)
{
    ui->setupUi(this);

    auto currentDir = QDir::current();
    auto fileNames = currentDir.entryList();
    std::vector<QString> deckNames;
    for (int i = 0; i < fileNames.size(); ++i)
    {
        auto file = fileNames.at(i);
        if(file.contains(".json"))
        {
            file = file.remove(".json");
            deckNames.push_back(file);
        }
    }

    for (auto & name : deckNames)
    {
        auto row = new DeckManagerRow(name, this);
        connect(row, &DeckManagerRow::openExportDialog, this, &DeckManagerWidget::createExportDialog);
        connect(row, &DeckManagerRow::openStatsViewer, this, &DeckManagerWidget::openStatsWidget);
        ui->deckTable->addWidget(row);
    }
}

DeckManagerWidget::~DeckManagerWidget()
{
    delete ui;
}
void DeckManagerWidget::createExportDialog(QString name)
{


}
void DeckManagerWidget::openStatsWidget(QString name)
{

}
