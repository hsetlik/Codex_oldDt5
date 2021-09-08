#ifndef DECKMANAGERWIDGET_H
#define DECKMANAGERWIDGET_H

#include <QWidget>
#include "deckcreatorwidget.h"

class DeckManagerRow : public QWidget
{
    Q_OBJECT
public:
    explicit DeckManagerRow(QString name, QWidget* parent = nullptr);
    const QString deckName;
Q_SIGNALS:
    void openExportDialog(QString name);
    void openStatsViewer(QString name);
private:
    QLabel* deckNameLabel;
    QPushButton* exportButton;
    QPushButton* viewStatsButton; //TODO: make a statistics viewer type thing with graphs and stuff
private Q_SLOTS:
    void on_exportClick() { Q_EMIT openExportDialog(deckName); }
    void on_statsClick() { Q_EMIT openStatsViewer(deckName); }
};
//==============================================================

namespace Ui {
class DeckManagerWidget;
}

class DeckManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DeckManagerWidget(QWidget *parent = nullptr);
    ~DeckManagerWidget();
Q_SIGNALS:
    void exitDeckManager();
    void createStatsScreen(QString name);
private Q_SLOTS:
    void createExportDialog(QString name);
    void openStatsWidget(QString name);
    void chooseFolder();
    void on_backButton_clicked();

private:
    Ui::DeckManagerWidget *ui;
    std::vector<DeckManagerRow*> rows;
    QPushButton* exportButton;
    QHBoxLayout* nameLayout;
    QLabel* nameLabel;
    QLabel* histLabel;
    QPushButton* expButton;
    QLineEdit* nameEdit;
    QCheckBox* histCheck;
    QHBoxLayout* histLayout;
    QString exportActiveName;
};

#endif // DECKMANAGERWIDGET_H
