#ifndef DECKMANAGERWIDGET_H
#define DECKMANAGERWIDGET_H

#include <QWidget>
#include "deckcreatorwidget.h"

class DeckManagerRow : public QWidget
{
    Q_OBJECT
public:
    explicit DeckManagerRow(QString name, QWidget* parent = nullptr);
Q_SIGNALS:
    void openExportDialog(QString name);
    void openStatsViewer(QString name);
private:
    const QString deckName;
    QLabel* deckNameLabel;
    QPushButton* exportButton;
    QPushButton* viewStatsButton; //TODO: make a statistics viewer type thing with graphs and stuff
private Q_SLOTS:
    void on_exportClick() { Q_EMIT openExportDialog(deckName); }
    void on_statsClick() { Q_EMIT openStatsViewer(deckName); }
};


namespace Ui {
class DeckManagerWidget;
}

class DeckManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeckManagerWidget(QWidget *parent = nullptr);
    ~DeckManagerWidget();
private Q_SLOTS:
    void createExportDialog(QString name);
    void openStatsWidget(QString name);

private:
    Ui::DeckManagerWidget *ui;
};

#endif // DECKMANAGERWIDGET_H
