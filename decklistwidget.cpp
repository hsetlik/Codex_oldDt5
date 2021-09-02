#include "decklistwidget.h"
#include "ui_decklistwidget.h"
#include <QtWidgets>

DeckListWidget::DeckListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeckListWidget)
{
    ui->setupUi(this);
    //find all the .json files in the working directory
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
    //create all the deck buttons
    for (auto& name : deckNames)
    {
        auto button = new QPushButton(name, this);
        ui->decksVBox->addWidget(button);
        connect(button, &QPushButton::clicked, this, &DeckListWidget::deckButtonClicked);
    }
}
DeckListWidget::~DeckListWidget()
{
    delete ui;
}
void DeckListWidget::on_manageDecksButon_clicked()
{
    //todo: set up a deck manager

}
void DeckListWidget::deckButtonClicked()
{
    auto button = qobject_cast<QPushButton*>(sender());
    auto name = button->text();
    Q_EMIT openDeck(name);
}


