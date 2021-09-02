#include "inputwidget.h"
#include "ui_inputform.h"


void WordLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    auto brushColor = (mouseIsOver) ? backgroundColor.lighter(60) : backgroundColor;
    QBrush brush(brushColor);
    auto cornerSize = (float)frameRect().height() / 5.0f;
    auto bounds = QRectF(frameRect());
    painter.setBrush(brush);
    painter.drawRoundedRect(bounds, cornerSize, cornerSize);
    QPen pen(Qt::white, 1.8f, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin);
    painter.setPen(pen);
    auto textCorner = QPointF(cornerSize, cornerSize * 3.3f);
    painter.drawText(textCorner, text());
}
//==========================================================================================
PhraseWidget::PhraseWidget(const QString& fullPhrase, PhrasePair* p, QWidget *parent) :
    QWidget(parent),
    linkedPair(p)
{
    qDebug() << "Making editor widget with phrase: " << fullPhrase;
    auto exp = QRegExp("\\s+");
    auto words = fullPhrase.split(exp);
    int x = 5;
    int y = 5;
    for(auto& word : words)
    {
        auto label = new WordLabel(word, linkedPair, this);
        addedLabels.push_back(label);
        label->move(x, y);
        label->show();
        label->setAttribute(Qt::WA_DeleteOnClose);

        x += label->width() + 2;
    }
    setAcceptDrops(true);
}

WordLabel* PhraseWidget::labelWidget(QString inputWord)
{
    for(auto w : addedLabels)
    {
        auto word = dynamic_cast<WordLabel*>(w);
        if(word->text() == inputWord)
            return word;

    }
    return nullptr;
}
bool PhraseWidget::containsWord(QString input)
{
    for(auto w : addedLabels)
    {
        auto word = dynamic_cast<WordLabel*>(w);
        if(word->text() == input)
            return true;

    }
    return false;
}
void PhraseWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("word")) {
        if (children().contains(event->source())) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
void PhraseWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("word")) {
        if (children().contains(event->source())) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
//==========================================================================================
NPhraseWidget::NPhraseWidget(const QString& fullPhrase, PhrasePair* p, QWidget *parent) :
    PhraseWidget(fullPhrase, p, parent)
{

}

void NPhraseWidget::mousePressEvent(QMouseEvent *event)
{
    WordLabel* child = dynamic_cast<WordLabel*>(childAt(event->pos()));
    if(child == nullptr)
        return;
    if(event->button() == Qt::RightButton)
    {
        auto nativeWord = child->text();
        linkedPair->removeNtaPairByNative(nativeWord);
    }
    else
    {
        auto clickSpot = event->pos() - child->pos();

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << child->text() << QPoint(clickSpot);

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("word", itemData);
        mimeData->setText(child->text());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(child->pixmap(Qt::ReturnByValue));
        drag->setHotSpot(clickSpot);
        drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction);
    }

}
void NPhraseWidget::dropEvent(QDropEvent *event)
{
    WordLabel* child = dynamic_cast<WordLabel*>(childAt(event->pos()));
    if(child == nullptr)
        return;
    auto sNative = child->text();
    auto sTarget = event->mimeData()->text();
    linkedPair->addNtaPair(sNative, sTarget);
}
QPointF NPhraseWidget::getConnectionPointFor(QString word)
{
    auto label = labelWidget(word);
    if(label == nullptr)
        return QPointF(0.0f, 0.0f);
    float x0 = (float)label->x() + label->width() / 2;
    float y0 = (float)label->y() + label->height() * 1.05f;
    return QPointF(x0, y0) + QPointF(pos());
}
//==========================================================================================
TPhraseWidget::TPhraseWidget(const QString& fullPhrase, PhrasePair* p, QWidget *parent) :
    PhraseWidget(fullPhrase, p, parent)
{

}
void TPhraseWidget::mousePressEvent(QMouseEvent *event)
{
    WordLabel* child = dynamic_cast<WordLabel*>(childAt(event->pos()));
    if(child == nullptr)
        return;
    if(event->button() == Qt::RightButton)
    {
        auto targetWord = child->text();
        linkedPair->removeNtaPairByTarget(targetWord);
    }
    else
    {
        auto clickSpot = event->pos() - child->pos();

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << child->text() << QPoint(clickSpot);

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("word", itemData);
        mimeData->setText(child->text());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(child->pixmap(Qt::ReturnByValue));
        drag->setHotSpot(clickSpot);
        drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction);
    }
}
void TPhraseWidget::dropEvent(QDropEvent *event)
{
    WordLabel* child = dynamic_cast<WordLabel*>(childAt(event->pos()));
    if(child == nullptr)
        return;
    auto sTarget = child->text();
    auto sNative = event->mimeData()->text();
    if(!containsWord(sNative))
        linkedPair->addNtaPair(sNative, sTarget);
}
QPointF TPhraseWidget::getConnectionPointFor(QString word)
{
    auto label = labelWidget(word);
    if(label == nullptr)
        return QPointF(0.0f, 0.0f);
    float x0 = (float)label->x() + label->width() / 2;
    float y0 = (float)label->y() - label->height() * 0.05f;
    return QPointF(x0, y0) + QPointF(pos());
}
void TPhraseWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QColor clozeColor(30, 156, 30);
    QColor background(80, 80, 80);
    auto wordToToggle = dynamic_cast<WordLabel*>(childAt(event->pos()));
    if(wordToToggle == nullptr)
        return;
    auto qWord = wordToToggle->text();
    auto sWord = qWord;
    if(wordToToggle == nullptr)
        return;
    if(!linkedPair->hasCloze(sWord))
    {
        linkedPair->addCloze(sWord);
        wordToToggle->setBackgroundColor(clozeColor);
    }
    else
    {
        linkedPair->removeCloze(sWord);
        wordToToggle->setBackgroundColor(background);
    }
    wordToToggle->repaint();
}
//==========================================================================================
PhrasePairWidget::PhrasePairWidget(PhrasePair* pair, QWidget* parent) :
    QWidget(parent),
    linkedPair(pair),
    nativePhrase(nullptr),
    targetPhrase(nullptr)
{
    pair->addListener(this);
    QString nativeQ = linkedPair->nativePhrase.fullPhrase;
    QString targetQ = linkedPair->targetPhrase.fullPhrase;
    nativePhrase = new NPhraseWidget(nativeQ, linkedPair, this);
    targetPhrase = new TPhraseWidget(targetQ, linkedPair, this);
    auto layout = new QVBoxLayout;
    layout->addWidget(nativePhrase);
    layout->addWidget(targetPhrase);
    //layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);
}

void PhrasePairWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    QPen pen(QColor(80, 80, 80), 1.8f, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin);
    painter.setPen(pen);
    auto& pairs = linkedPair->ntaPairs;
    for(auto& p : pairs)
    {
         auto p1 = nativePhrase->getConnectionPointFor(p.first);
         auto p2 = targetPhrase->getConnectionPointFor(p.second);
         auto centerY = std::min(p1.y(), p2.y()) + (std::abs(p1.y() - p2.y()) / 2);
         auto centerX = std::min(p1.x(), p2.x()) + (std::abs(p1.x() - p2.x()) / 2.0f);
         auto c1 = QPointF(p1.x(), centerY);
         auto c2 = QPointF(p2.x(), centerY);
         QPainterPath path;
         path.moveTo(p1);
         path.quadTo(c1, QPointF(centerX, centerY));
         path.quadTo(c2, p2);
         painter.drawPath(path);
    }
}


//==========================================================================================

InputWidget::InputWidget(QWidget *parent) :
    QWidget(parent),
    currentPhrasePair(nullptr),
    pairIndex(0),
    totalCardsAdded(0),
    ui(new Ui::InputForm)
{
    ui->setupUi(this);
    pairIndex = -1;
    advancePhrasePair();
    ui->finishButton->setVisible(false);
}

void InputWidget::advancePhrasePair()
{
    if(currentPhrasePair != nullptr)
    {
        createdCardSets.push_back(PhrasePairCards(currentPhrasePair->linkedPair));
    }
     ++pairIndex;
    ui->verticalLayout->removeWidget(&*currentPhrasePair);
    printf("Displaying Pair Number %d\n", pairIndex);
    auto& pairToAdd = allPairs[pairIndex];
    currentPhrasePair.reset(new PhrasePairWidget(&pairToAdd, this));
    ui->verticalLayout->addWidget(&*currentPhrasePair);
    QString currentCards = "Cards Added: " + QString::number(totalCardsAdded);
    ui->numCardsLabel->setText(currentCards);
    if(pairIndex == (int)allPairs.size() - 1)
    {
         ui->finishButton->setVisible(true);
         ui->nextButton->setEnabled(false);
    }
    else
        ui->finishButton->setVisible(false);
}
void InputWidget::prepareEditorsFor(std::vector<PhrasePair>& pairs)
{
    allPairs.clear();
    for(auto& pair : pairs)
        allPairs.push_back(pair);
    //printf("Preparing editors. . .\n");
    pairIndex = -1;
    advancePhrasePair();
}
void InputWidget::on_prevButton_clicked()
{
    if(pairIndex < 1)
        return;
    --pairIndex;
    totalCardsAdded -= currentPhrasePair->linkedPair->getNumCards();
    if(createdCardSets.size() >= 1)
        createdCardSets.pop_back();
    ui->verticalLayout->removeWidget(&*currentPhrasePair);
    //printf("Displaying Pair Number %d\n", pairIndex);
    auto& pairToAdd = allPairs[pairIndex];
    currentPhrasePair.reset(new PhrasePairWidget(&pairToAdd, this));
    ui->verticalLayout->addWidget(&*currentPhrasePair);
    QString currentCards = "Cards Added: " + QString::number(totalCardsAdded);
    ui->numCardsLabel->setText(currentCards);
}
void InputWidget::on_nextButton_clicked()
{
    totalCardsAdded += currentPhrasePair->linkedPair->getNumCards();
    advancePhrasePair();
}

void InputWidget::on_fullBox_stateChanged(int)
{
    allPairs[pairIndex].toggleIncludeFull();
}
void InputWidget::on_finishButton_clicked()
{
    if(currentPhrasePair != nullptr)
    {
        createdCardSets.push_back(PhrasePairCards(currentPhrasePair->linkedPair));
    }
    Q_EMIT returnNewPairCards(getPairCardsAsArray());
}
void InputWidget::on_exitButton_clicked()
{
    Q_EMIT exitToDeck();
}

