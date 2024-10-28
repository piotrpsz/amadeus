#pragma once

#include <QWidget>

class ListTree;
class ListFilesTable;
class QSplitter;
class QShowEvent;

class PlayList : public QWidget {
    Q_OBJECT
    static constexpr int SPLITTER_HANDLE_WIDTH{3};
    QSplitter* const splitter_;
    ListTree* const lists_;
    ListFilesTable* const files_;
public:
    explicit PlayList(QWidget *parent = nullptr);
private:
    void showEvent(QShowEvent*) override;
};
