#include "catalog.h"
#include "dirs_tree.h"
#include "files_table.h"
#include <QSplitter>
#include <QSplitter>
#include <QShowEvent>
#include <QVBoxLayout>

Catalog::Catalog(QWidget *parent) : QWidget{parent},
    splitter_{new QSplitter(Qt::Horizontal)},
    dirs_{new DirsTree},
    files_{new FilesTable}
{
    splitter_->setHandleWidth(SPLITTER_HANDLE_WIFTH);
    splitter_->addWidget(dirs_);
    splitter_->addWidget(files_);

    auto const main = new QVBoxLayout;
    main->addWidget(splitter_);
    setLayout(main);
}

void Catalog::showEvent(QShowEvent* e) {
    auto const width = size().width();
    auto const w0 = 25. * width / 100.;
    auto const w1 = width - splitter_->handleWidth() - w0;
    splitter_->setSizes({static_cast<int>(w0), static_cast<int>(w1)});
}
