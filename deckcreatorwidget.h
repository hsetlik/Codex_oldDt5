#ifndef DECKCREATORWIDGET_H
#define DECKCREATORWIDGET_H

#include <QWidget>
#include <QLocale>
#include "decklistwidget.h"

class CodexLang
{
public:
    static std::unordered_map<QString, int> langEnumsAndNames()
    {
        std::unordered_map<QString, int> output;
        output["Albanian"] = QLocale::Albanian;
        output["Belarusian"] = QLocale::Belarusian;
        output["Cantonese"] = QLocale::Cantonese;
        output["Danish"] = QLocale::Danish;
        output["English"] = QLocale::English;
        output["French"] = QLocale::French;
        output["German"] = QLocale::German;
        output["Hausa"] = QLocale::Hausa;
        output["Italian"] = QLocale::Italian;
        output["Japanese"] = QLocale::Japanese;
        output["Korean"] = QLocale::Korean;
        output["Latvian"] = QLocale::Latvian;
        output["Moldavian"] = QLocale::Moldavian;
        output["Nepali"] = QLocale::Nepali;
        output["Occitan"] = QLocale::Occitan;
        output["Persian"] = QLocale::Persian;
        output["Quechua"] = QLocale::Quechua;
        output["Russian"] = QLocale::Russian;
        output["Samoan"] = QLocale::Samoan;
        output["Tigre"] = QLocale::Tigre;
        output["Ukrainian"] = QLocale::Ukrainian;
        output["Vietnamese"] = QLocale::Vietnamese;
        output["Walloon"] = QLocale::Walloon;
        output["Xhosa"] = QLocale::Xhosa;
        output["Yoruba"] = QLocale::Yoruba;
        output["Zulu"] = QLocale::Zulu;
        return output;
    }
};

namespace Ui {
class DeckCreatorWidget;
}

class DeckCreatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeckCreatorWidget(QWidget *parent = nullptr);
    ~DeckCreatorWidget();
Q_SIGNALS:
    void newDeck(QLocale native, QLocale target, QString name);
    void abortNewDeck();

private Q_SLOTS:
    void on_createButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::DeckCreatorWidget *ui;
};

#endif // DECKCREATORWIDGET_H
