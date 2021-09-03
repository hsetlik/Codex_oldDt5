#include "decklistwidget.h"
#include "ui_decklistwidget.h"
#include <QtWidgets>

DeckMenuItem::DeckMenuItem(QString name, QWidget* parent) : QWidget(parent), deckName(name)
{
    auto tempDeck = new Deck(deckName);
    const int numDue = tempDeck->numDueToday();
    QString labelText = "Due Today: " + QString::number(numDue);
    auto hLayout = new QHBoxLayout;
    deckButton = new QPushButton(deckName, this);
    dueLabel = new QLabel(labelText, this);
    hLayout->addWidget(deckButton);
    hLayout->addWidget(dueLabel);
    setLayout(hLayout);
    delete tempDeck;
}

void DeckMenuItem::refreshLabel()
{
    auto tempDeck = new Deck(deckName);
    const int numDue = tempDeck->numDueToday();
    QString labelText = "Due Today: " + QString::number(numDue);
    dueLabel->setText(labelText);
    delete tempDeck;
}
//==================================================================
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
        auto button = new DeckMenuItem(name, this);
        menuItems.push_back(button);
        ui->decksVBox->addWidget(button);
        connect(button->deckButton, &QPushButton::clicked, this, &DeckListWidget::deckButtonClicked);
    }
}
DeckListWidget::~DeckListWidget()
{
    delete ui;
}
void DeckListWidget::on_manageDecksButon_clicked()
{
    printf("Manage decks clicked\n");
    Q_EMIT launchDeckManager();
}
void DeckListWidget::deckButtonClicked()
{
    auto button = qobject_cast<QPushButton*>(sender());
    auto name = button->text();
    Q_EMIT openDeck(name);
}
void DeckListWidget::updateLabels()
{
    for (auto & button : menuItems)
        button->refreshLabel();
}


