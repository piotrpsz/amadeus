#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QTreeWidget>

class DirsTree;
class FilesTable;
class QSplitter;
class QShowEvent;

class Catalog : public QWidget {
    Q_OBJECT
    static constexpr int SPLITTER_HANDLE_WIFTH{3};
    QSplitter* const splitter_;
    DirsTree* const dirs_;
    FilesTable* const files_;
public:
    explicit Catalog(QWidget *parent = nullptr);
private:
    void showEvent(QShowEvent*) override;
};
