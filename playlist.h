#pragma once

#include <QWidget>

class PlayListTree;
class PlayListTable;
class QSplitter;
class QShowEvent;

class PlayList : public QWidget {
    Q_OBJECT
    static constexpr int SPLITTER_HANDLE_WIDTH{3};
    QSplitter* const splitter_;
    PlayListTree* const lists_;
    PlayListTable* const files_;
public:
    explicit PlayList(QWidget *parent = nullptr);
private:
    void showEvent(QShowEvent*) override;
};
