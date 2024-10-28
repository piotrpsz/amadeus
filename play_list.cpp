#include "play_list.h"
#include "list_tree.h"
#include "list_files_table.h"
#include <QSplitter>
#include <QVBoxLayout>



PlayList::PlayList(QWidget *parent) : QWidget{parent},
    splitter_{new QSplitter{Qt::Horizontal}},
    lists_{new ListTree},
    files_{new ListFilesTable}
{
    splitter_->setHandleWidth(SPLITTER_HANDLE_WIDTH);
    splitter_->addWidget(lists_);
    splitter_->addWidget(files_);

    auto const main = new QVBoxLayout;
    main->addWidget(splitter_);
    setLayout(main);
}

void PlayList::showEvent(QShowEvent* e) {
    auto const width = size().width();
    auto const w0 = 25. * width / 100.;
    auto const w1 = width - splitter_->handleWidth() - w0;
    splitter_->setSizes({static_cast<int>(w0), static_cast<int>(w1)});
}
