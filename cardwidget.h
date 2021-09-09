#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QWidget>
#include "cardstructure.h"

class CardContent : public QWidget
{
    Q_OBJECT
public:
    explicit CardContent(Card* c, QWidget* parent = nullptr);
    virtual ~CardContent() {}
    Card* const linkedCard;
    virtual void flip() {}
    void keyPressEvent(QKeyEvent* e) override
    {
        if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
            Q_EMIT checkAnswer();
    }
Q_SIGNALS:
    void checkAnswer();
};
//==========================================================
class NtaContent : public CardContent
{
    Q_OBJECT
public:
    explicit NtaContent(Card* card, QWidget* parent = nullptr);
    void flip() override;
private:
    QString targetStr;
    QString nativeStr;
    QLabel* targetLabel;
    QLabel* nativeLabel;
    QVBoxLayout* layout;
};
//================================================================
struct ClozeLabel : public QLabel
{
public:
    explicit ClozeLabel(QString text, QWidget* parent = nullptr) :
        QLabel(parent)
    {
        setText(text);
        setFrameStyle(QFrame::Panel);
    }
    void paintEvent(QPaintEvent* ) override;
};
//================================================================
struct ClozePhraseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ClozePhraseWidget(Card* card, QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    void flip();
private:
    Card* const linkedCard;
    QHBoxLayout* targetLayout;
    QVBoxLayout* mainLayout;
    QLabel* nativeLabel;
    std::vector<ClozeLabel*> targetLabels;
    QLineEdit* editBox;
    ClozeLabel* hiddenLabel;
    QLabel* answerLabel;
    bool isFlipped;
};

//================================================================
class ClozeContent : public CardContent
{
    Q_OBJECT

public:
    explicit ClozeContent(Card* card, QWidget* parent=nullptr);
    void flip() override;
private:
    ClozePhraseWidget* phraseWidget;
};
//==========================================================
class FullContent : public CardContent
{
public:
    explicit FullContent(Card* card, QWidget* parent=nullptr);
    void flip() override;
private:
    QLabel* fullTarget;
    QLabel* fullNative;
    QLineEdit* answerBox;
};
//==========================================================
class CardViewer : public QWidget
{
    Q_OBJECT
public:
    explicit CardViewer(std::vector<Card*>& cards, QWidget *parent = nullptr);
    void nextCard();
    void repeatCard();
    Card* getCurrent() {return currentContent->linkedCard; }
public Q_SLOTS:
    void flip();
Q_SIGNALS:
    void cardFlipped();
    void finishStudyMode();
private:
    void addContentForCard(Card* card);
    std::vector<Card*> allCards;
    int cardIdx;
    QVBoxLayout* contentLayout;
public:
    CardContent* currentContent;
};

//==========================================================
namespace Ui
{
class CardWidget;
}
class CardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardWidget(Deck* deck, QWidget *parent = nullptr);
    Deck* const linkedDeck;
    void nextCard();

    ~CardWidget();
Q_SIGNALS:
    void dueCardsFinished();
private Q_SLOTS:
    void on_button1_clicked();

    void on_button2_clicked();

    void on_button3_clicked();

    void on_button4_clicked();

    void submitCard();

    void finishStudying();
private:
    void setButtonsVisible(bool shouldBeVisible);
    void setButtonDayValues(int l1, int l2, int l3);
    void updateIndexLabel();
    Ui::CardWidget *ui;
    CardViewer* viewer;
    std::vector<Card*> cardsDue;
    int cardIdx;
    int numCards;
};
#endif // CARDWIDGET_H
