#include "catalog.h"
#include "dirs_tree.h"
#include "files_table.h"
#include <QSplitter>
#include <QVBoxLayout>

Catalog::Catalog(QWidget *parent):
    QWidget{parent},
    dirs_{new DirsTree},
    files_{new FilesTable}
{
    auto const splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(dirs_);
    splitter->addWidget(files_);

    auto const main = new QVBoxLayout;
    main->addWidget(splitter);
    setLayout(main);
}
