

#include "playlist_widget.h"
#include "playlist_tree.h"
#include "playlist_table.h"
#include <QSplitter>
#include <QVBoxLayout>



PlaylistWidget::PlaylistWidget(QWidget *parent)
    : QWidget{parent}
    , splitter_{new QSplitter{Qt::Horizontal}}
    , lists_{new PlaylistTree}
    , files_{new PlaylistTable}
{
    splitter_->setHandleWidth(SPLITTER_HANDLE_WIDTH);
    splitter_->addWidget(lists_);
    splitter_->addWidget(files_);

    auto const main = new QVBoxLayout;
    main->addWidget(splitter_);
    setLayout(main);
}

void PlaylistWidget::showEvent(QShowEvent* e) {
    auto const width = size().width();
    auto const w0 = 25. * width / 100.;
    auto const w1 = width - splitter_->handleWidth() - w0;
    splitter_->setSizes({static_cast<int>(w0), static_cast<int>(w1)});
    files_->setFocus();
}
