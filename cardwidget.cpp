#include "cardwidget.h"
#include "ui_cardwidget.h"
#include "deckstatsmanager.h"

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
void ClozeLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen(Qt::black, 1.8f, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin);
    painter.setPen(pen);
    painter.drawText(QRectF(frameRect()), Qt::AlignLeft, text());
    /*
    QBrush brush(background);
    painter.fillRect(frameRect(), brush);
    */
}
QSize ClozeLabel::sizeHint() const
{
    auto metrics = fontMetrics();
    auto size = metrics.size(Qt::TextSingleLine, text());
    return size;
}
//===========================================================================
ClozePhraseWidget::ClozePhraseWidget(Card* card, QWidget* parent) :
    QWidget(parent),
    linkedCard(card),
    hiddenLabel(nullptr),
    answerLabel(nullptr),
    isFlipped(false)
{
    auto cCard = dynamic_cast<ClozeCard*>(linkedCard);
    auto fullTarget = cCard->getFullTarget();
    auto clozeWord = cCard->getBackData();
    //split the target words into a vector to create labels
    auto exp = QRegExp("\\s+");
    auto words = fullTarget.split(exp);
    int fullWidth = 0;
    int x = 5;
    int y = 5;
    for(auto& word : words)
    {
        auto wordLabel = new ClozeLabel(word, this);
        wordLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

        wordLabel->move(x, y);
        wordLabel->show();
        wordLabel->setAttribute(Qt::WA_DeleteOnClose);
        if(word == clozeWord)
        {
            hiddenLabel = wordLabel;
            hiddenLabel->setVisible(false);
        }
        x += wordLabel->sizeHint().width() + 2;
        if(x > fullWidth)
            fullWidth = x;
        if (x > 350)
        {
            y += wordLabel->height() + 2;
            x = 5;
        }
        targetLabels.push_back(wordLabel);
    }
    phraseWidth = fullWidth;
    printf("Phrase Width: %d\n", phraseWidth);
    setFixedWidth(phraseWidth);

}

void ClozePhraseWidget::paintEvent(QPaintEvent *)
{

}

void ClozePhraseWidget::flip(const QString& answer)
{
    hiddenLabel->setVisible(true);
    isFlipped = true;
    //create the answer label
    answerLabel = new ClozeLabel(answer, this);
    answerLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    int xMatch = 0;
    int yBottom = 0;
    for (auto& label : targetLabels)
    {
        if(label->text() == answer)
        {
            xMatch = label->x();
        }
        if(label->y() + label->height() > yBottom)
            yBottom = label->y() + label->height();
    }
    answerLabel->move(xMatch, yBottom + 2);
    answerLabel->show();
    answerLabel->setAttribute(Qt::WA_DeleteOnClose);






}
//===========================================================================
ClozeContent::ClozeContent(Card* _card, QWidget* parent) :
    CardContent(_card, parent)
{
    mainLayout = new QVBoxLayout;
    phraseWidget = new ClozePhraseWidget(_card, this);
    nativeLabel = new QLabel(dynamic_cast<ClozeCard*>(linkedCard)->getFullNative(), this);
    editBox = new QLineEdit(this);
    mainLayout->addWidget(nativeLabel);
    mainLayout->addWidget(phraseWidget);
    phraseWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    phraseWidget->setFixedWidth(phraseWidget->getPhraseWidth());
    mainLayout->addWidget(editBox);
    setLayout(mainLayout);
}
void ClozeContent::flip()
{
    auto answer = editBox->text();
    phraseWidget->flip(answer);
    mainLayout->removeWidget(nativeLabel);
    mainLayout->removeWidget(editBox);
    delete nativeLabel;
    delete editBox;
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
    if(currentContent)
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
    auto a1 = viewer->getCurrent()->daysForAnswer(1);
    auto a2 = viewer->getCurrent()->daysForAnswer(2);
    auto a3 = viewer->getCurrent()->daysForAnswer(3);
    setButtonDayValues(a1, a2, a3);
    setButtonsVisible(false);
    viewer->currentContent->setFocus(Qt::ActiveWindowFocusReason);
    updateIndexLabel();
}

void CardWidget::nextCard()
{
    ++cardIdx;
    viewer->nextCard();
    viewer->currentContent->setFocus(Qt::ActiveWindowFocusReason);
    updateIndexLabel();
    auto a1 = viewer->getCurrent()->daysForAnswer(1);
    auto a2 = viewer->getCurrent()->daysForAnswer(2);
    auto a3 = viewer->getCurrent()->daysForAnswer(3);
    setButtonDayValues(a1, a2, a3);
    setButtonsVisible(false);
    auto fWidget = focusWidget();
    if(dynamic_cast<CardViewer*>(fWidget))
        printf("Viewer in focus\n");
    else if(dynamic_cast<CardContent*>(fWidget))
        printf("Content in focus\n");
    else if(fWidget == this)
        printf("Card widget in focus\n");
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
    viewer->currentContent->linkedCard->updateWithAnswer(3);
    nextCard();
}
void CardWidget::on_button4_clicked()
{
    viewer->currentContent->linkedCard->updateWithAnswer(6);
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
    auto dName = linkedDeck->getName();
    DeckStatsManager::addSnapshot(linkedDeck, dName);
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
