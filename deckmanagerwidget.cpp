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
ExportDeckDialog::ExportDeckDialog (QString deckName, QWidget* parent) :
    QWidget(parent)
{
    auto tempDeck = new Deck(deckName);
    dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::Directory);
    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(dialog);
    auto hLayout = new QHBoxLayout;
    nameEdit = new QLineEdit(deckName, this);
    eraseBox = new QCheckBox("Include History?", this);
    hLayout->addWidget(nameEdit);
    hLayout->addWidget(eraseBox);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
}

//======================================================
DeckManagerWidget::DeckManagerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeckManagerWidget),
    exportButton(nullptr),
    nameLayout(nullptr),
    histLayout(nullptr),
    exportActiveName("null")
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
        rows.push_back(row);
    }
}

DeckManagerWidget::~DeckManagerWidget()
{
    delete ui;
}
void DeckManagerWidget::createExportDialog(QString name)
{
    exportActiveName = name;
    //remove export widgets if they exist yet
    //find the index of the clicked deck widget
    int index = 1;
    for(auto & row : rows)
    {
        if(row->deckName == name)
            break;
        ++index;
    }
    nameLabel = new QLabel("Name: ", this);
    nameEdit = new QLineEdit(name, this);
    nameLayout = new QHBoxLayout;
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    ui->deckTable->insertLayout(index, nameLayout);
    histLabel = new QLabel("Preserve Answer History?: ", this);
    histCheck = new QCheckBox(this);
    histLayout = new QHBoxLayout;
    histLayout->addWidget(histLabel);
    histLayout->addWidget(histCheck);
    ui->deckTable->insertLayout(index, histLayout);
    expButton = new QPushButton("Export Deck", this);
    ui->deckTable->insertWidget(index, expButton);
    connect(expButton, &QPushButton::clicked, this, &DeckManagerWidget::chooseFolder);

}
void DeckManagerWidget::openStatsWidget(QString name)
{

}

void DeckManagerWidget::chooseFolder()
{
    auto tempDeck = new Deck(exportActiveName);
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    if(dialog.exec())
    {
        auto dir = dialog.directory();
        auto newName = nameEdit->text();
        auto preserveHistory = histCheck->isChecked();
        tempDeck->exportDeck(dir, newName, preserveHistory);
    }
    //remove the export widgets again
    if (nameLayout != nullptr)
    {
        delete nameLayout;
        delete histLayout;
        delete exportButton;
        delete nameEdit;
        delete histCheck;
        delete nameLabel;
        delete histLabel;
        delete expButton;
    }
    delete tempDeck;
}
