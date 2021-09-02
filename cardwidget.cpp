#include "cardwidget.h"
#include "ui_cardwidget.h"

CardContent::CardContent(Card* c, QWidget* parent) :
    QWidget(parent),
    linkedCard(c)
{
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);

}
//===========================================================================
NtaContent::NtaContent(Card* card, QWidget* parent) :
    CardContent(card, parent),
    targetLabel(nullptr),
    nativeLabel(nullptr),
    layout(nullptr)
{
    printf("\nNew Nta Content\n");
    targetStr = card->getFrontData();
    printf("Target: %s\n", targetStr.toStdString().c_str());
    nativeStr = card->getBackData();
    printf("Native: %s\n", card->getBackData().toStdString().c_str());
    targetLabel = new QLabel(targetStr, this);
    nativeLabel = new QLabel(nativeStr, this);
    targetLabel->setAttribute(Qt::WA_DeleteOnClose);
    nativeLabel->setAttribute(Qt::WA_DeleteOnClose);

    layout = new QVBoxLayout;
    layout->addWidget(targetLabel);
    layout->addWidget(nativeLabel);
    nativeLabel->setVisible(false);
    setLayout(layout);
}
void NtaContent::flip()
{
    nativeLabel->setVisible(true);
}
//===========================================================================
ClozeContent::ClozeContent(Card* _card, QWidget* parent) :
    CardContent(_card, parent),
    clozeBox(nullptr)
{
    auto card = dynamic_cast<ClozeCard*>(_card);
    printf("Creating cloze content. . . \n");
    QString fullTarget = card->getFullTarget();
    auto tWords = fullTarget.split(QRegExp("\\s+"));
    printf("Target phrase has %d words\n", (int)tWords.size());
    auto clozeWord = card->getBackData();
    int x= 5;
    int y = 5;
    for(auto& word : tWords)
    {
        auto label = new QLabel(word, this);
        //printf("Cloze label for word: %s\n", word);
        label->move(x, y);
        label->show();
        label->setAttribute(Qt::WA_DeleteOnClose);
        x += label->width() + 2;
        if(word == clozeWord)
        {
            qDebug() << "Cloze Word: " << word;
            clozeBox = new QLineEdit(this);
            clozeBox->setGeometry(label->x(), label->y(), label->width(), label->height());
            clozeBox->show();
            label->setVisible(false);
        }
        targetLabels.push_back(label);
    }
    x = 5;
    y = 20;
    auto nativeWords = stdu::matchesAsVector(card->getFullNative(), std::regex("\\w+"));
    for(auto& word : nativeWords)
    {
        auto label = new QLabel(word, this);
        label->move(x, y);
        label->show();
        label->setAttribute(Qt::WA_DeleteOnClose);
        x += label->width() + 2;
        nativeLabels.push_back(label);
    }
}
void ClozeContent::flip()
{
    if(clozeBox != nullptr)
        clozeBox->setVisible(false);
    for(auto l : targetLabels)
    {
        l->setVisible(true);
    }
}
//===========================================================================
FullContent::FullContent(Card* card, QWidget* parent) :
    CardContent(card, parent),
    fullTarget(nullptr),
    fullNative(nullptr),
    answerBox(nullptr)
{
    int y = 5;
    int x = 5;
    fullTarget = new QLabel(card->getFrontData(), this);
    fullTarget->move(x, y);
    fullTarget->show();
    y += fullTarget->height() * 2;
    fullNative = new QLabel(card->getBackData(), this);
    fullNative->move(x, y);
    fullNative->show();
    fullNative->setVisible(false);
    answerBox = new QLineEdit(this);
    answerBox->move(x, y);
    answerBox->setMaximumWidth(fullNative->width());
    answerBox->show();
}
void FullContent::flip()
{
    answerBox->setVisible(false);
    fullNative->setVisible(true);
}
//===========================================================================
CardViewer::CardViewer(std::vector<Card*>& cards, QWidget *parent) :
    QWidget(parent),
    allCards(cards),
    cardIdx(0),
    contentLayout(new QVBoxLayout),
    currentContent(nullptr)
{
    addContentForCard(allCards[0]);
    setLayout(contentLayout);
}

void CardViewer::nextCard()
{
    //1. remove the existing content card
    ++cardIdx;
    if(!(cardIdx >= (int)allCards.size()))
    {
        addContentForCard(allCards[cardIdx]);
    }
    else
    {
        //Q_EMIT a signal here to exit study mode
        Q_EMIT finishStudyMode();
    }
}
void CardViewer::repeatCard()
{
    auto card = currentContent->linkedCard;
    allCards.push_back(card);
}
void CardViewer::flip()
{
    currentContent->flip();
    Q_EMIT cardFlipped();
}
void CardViewer::addContentForCard(Card* card)
{
    if(currentContent != nullptr)
        delete currentContent;
    CardContent* newContent;
    if(card->cardType == CardType::NTA)
    {
        newContent = new NtaContent(card, this);
    }
    else if(card->cardType == CardType::Cloze)
    {
        newContent = new ClozeContent(card, this);
    }
    else
    {
        newContent = new FullContent(card, this);
    }
    QObject::connect(newContent, &CardContent::checkAnswer, this, &CardViewer::flip);
    contentLayout->addWidget(newContent);
    currentContent = newContent;
}
std::vector<int> CardViewer::lengthsForCard()
{
    auto numAnswers = currentContent->linkedCard->getTimesAnswered();
    int lHard = Card::daysToDelay(numAnswers, 2);
    int lMed = Card::daysToDelay(numAnswers, 3);
    int lEasy = Card::daysToDelay(numAnswers, 4);
    return {lHard, lMed, lEasy};
}
//===========================================================================
CardWidget::CardWidget(Deck* deck, QWidget *parent) :
    QWidget(parent),
    linkedDeck(deck),
    ui(new Ui::CardWidget),
   viewer(nullptr),
   cardIdx(0)
{
    cardsDue = linkedDeck->dueToday();
    viewer = new CardViewer(cardsDue, this);
    numCards = (int)cardsDue.size();
    QObject::connect(viewer, &CardViewer::cardFlipped, this, &CardWidget::submitCard);
    ui->setupUi(this);
    ui->contentVBox->addWidget(viewer);
    connect(viewer, &CardViewer::finishStudyMode, this, &CardWidget::finishStudying);
    auto newValues = viewer->lengthsForCard();
    setButtonDayValues(newValues[0], newValues[1], newValues[2]);
    setButtonsVisible(false);
    updateIndexLabel();
}

void CardWidget::nextCard()
{
    ++cardIdx;
    viewer->nextCard();
    updateIndexLabel();
    auto newValues = viewer->lengthsForCard();
    setButtonDayValues(newValues[0], newValues[1], newValues[2]);
    setButtonsVisible(false);
}
CardWidget::~CardWidget()
{
    delete ui;
}
void CardWidget::on_button1_clicked()
{
    viewer->currentContent->linkedCard->updateWithAnswer(0);
    viewer->repeatCard();
    //decrement cardIdx so that the cards remaining count stays accurate
    --cardIdx;
    nextCard();
}
void CardWidget::on_button2_clicked()
{
    viewer->currentContent->linkedCard->updateWithAnswer(1);
    nextCard();
}
void CardWidget::on_button3_clicked()
{
    viewer->currentContent->linkedCard->updateWithAnswer(2);
    nextCard();
}
void CardWidget::on_button4_clicked()
{
    viewer->currentContent->linkedCard->updateWithAnswer(3);
    nextCard();
}
void CardWidget::setButtonsVisible(bool shouldBeVisible)
{
    ui->button1->setVisible(shouldBeVisible);
    ui->button2->setVisible(shouldBeVisible);
    ui->button3->setVisible(shouldBeVisible);
    ui->button4->setVisible(shouldBeVisible);
}

void CardWidget::submitCard()
{
    setButtonsVisible(true);
}
void CardWidget::finishStudying()
{
    Q_EMIT dueCardsFinished();
}
void CardWidget::setButtonDayValues(int l1, int l2, int l3)
{
    QString hStr = "Hard (" + QString::number(l1) + " days)";
    QString mStr = "Medium (" + QString::number(l2) + " days)";
    QString eStr = "Easy (" + QString::number(l3) + " days)";
    ui->button2->setText(hStr);
    ui->button3->setText(mStr);
    ui->button4->setText(eStr);
}
void CardWidget::updateIndexLabel()
{
    QString lStr = "Card " + QString::number(cardIdx) + "/" + QString::number(numCards);
    ui->label->setText(lStr);
}
