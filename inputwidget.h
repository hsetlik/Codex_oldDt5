#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QLabel>
#include <QWidget>
#include "cardstructure.h"

class WordLabel : public QLabel
{
    Q_OBJECT
public:
    WordLabel(QString txt, PhrasePair* parentPair, QWidget* parentWidget) :
        QLabel(parentWidget),
        linkedPair(parentPair),
        backgroundColor(QColor(80, 80, 80)),
        hoverBackground(QColor(100, 100, 100)),
        mouseIsOver(false)
    {
        QString label = txt;
        setFrameStyle(QFrame::Panel);
        setText(label);
    }
    PhrasePair* const linkedPair;
    bool operator==(const WordLabel& other) {return (text() == other.text() && linkedPair == other.linkedPair); }
    bool fromSamePhrase(const WordLabel& other) {return (linkedPair == other.linkedPair); }
    void paintEvent(QPaintEvent*) override;
    void setBackgroundColor(QColor newColor) {backgroundColor = newColor; }
private:
    void enterEvent(QEvent*) override {mouseIsOver = true; repaint(); }
    void leaveEvent(QEvent*) override {mouseIsOver = false; repaint(); }
    QColor backgroundColor;
    QColor hoverBackground;
    bool mouseIsOver;
};
//==========================================================================================
class PhraseWidget : public QWidget
{
public:
    explicit PhraseWidget(const QString& fullPhrase, PhrasePair* p, QWidget *parent = nullptr);
    virtual ~PhraseWidget() {}
    const QString content;
    PhrasePair* const linkedPair;
    WordLabel* labelWidget(QString word);
    bool containsWord(QString word);
    // this is pure virtual because words on the upper row connect from the bottom, lower row connect from the top
    virtual QPointF getConnectionPointFor(QString word)=0;
private:
    std::vector<QLabel*> addedLabels;
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
Q_SIGNALS:

};
//==========================================================================================
//Subclasses for native & target phrases. Constructors don't change, only the 3 virtual functions are different
class NPhraseWidget : public PhraseWidget
{
public:
    NPhraseWidget(const QString& fullPhrase, PhrasePair* p, QWidget *parent = nullptr);
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    QPointF getConnectionPointFor(QString word) override;
};

class TPhraseWidget : public PhraseWidget
{
public:
    TPhraseWidget(const QString& fullPhrase, PhrasePair* p, QWidget *parent = nullptr);
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;
    QPointF getConnectionPointFor(QString word) override;
};
//==========================================================================================
class PhrasePairWidget :
        public QWidget,
        public PhrasePairListener
{
    Q_OBJECT
public:
    explicit PhrasePairWidget(PhrasePair* pair, QWidget* parent);
    void phraseChanged(PhrasePair*) override {repaint(); }
    void paintEvent(QPaintEvent* event) override;
    PhrasePair* const linkedPair;
private:
    NPhraseWidget* nativePhrase;
    TPhraseWidget* targetPhrase;
};
//==========================================================================================
namespace Ui
{
class InputForm;
};

class InputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputWidget(QWidget *parent = nullptr);
    void advancePhrasePair();
    QJsonArray getPairCardsAsArray()
    {
        QJsonArray arr;
        for(auto& set : createdCardSets)
        {
            auto sObj = set.getPairJson();
            arr.append(sObj);
        }
        return arr;
    }

public Q_SLOTS:
    void prepareEditorsFor(std::vector<PhrasePair>& pairs);

Q_SIGNALS:
    void returnNewPairCards(QJsonArray cards);
    void exitToDeck();

private Q_SLOTS:
    void on_prevButton_clicked();
    void on_nextButton_clicked();
    void on_fullBox_stateChanged(int arg1);
    void on_finishButton_clicked();

    void on_exitButton_clicked();

private:
    std::vector<PhrasePair> allPairs;
    std::unique_ptr<PhrasePairWidget> currentPhrasePair;
    std::vector<PhrasePairCards> createdCardSets;
    int pairIndex;
    int totalCardsAdded;
    Ui::InputForm* ui;
};

#endif // INPUTWIDGET_H
