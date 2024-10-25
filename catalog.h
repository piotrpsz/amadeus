#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QTreeWidget>

class DirsTree;
class FilesTable;


class Catalog : public QWidget {
    Q_OBJECT
    DirsTree* const dirs_;
    FilesTable* const files_;
public:
    explicit Catalog(QWidget *parent = nullptr);

};
