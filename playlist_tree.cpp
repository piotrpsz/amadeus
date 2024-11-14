// MIT License
//
// Copyright (c) 2024 Piotr Pszczółkowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Created by Piotr Pszczółkowski on 28.10.2024.
// piotr@beesoft.pl

/*------- include files:
-------------------------------------------------------------------*/
#include "playlist_tree.h"
#include "shared/event.hh"
#include "shared/event_controller.hh"
#include "model/selection.h"
#include "model/playlist.h"
#include "line_text_edit.h"
#include <iostream>
// #include <format>
#include <memory>
#include <QDir>
#include <QMenu>
#include <QTimer>
#include <QAction>
#include <QFileInfo>
#include <QHeaderView>
#include <QMouseEvent>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
using namespace std;

PlaylistTree::PlaylistTree(QWidget* const parent) :
    QTreeWidget(parent),
    timer_{new QTimer}
{
    // !!! Timer configuration !!!
    // The timer is the sender of an event with information
    // that the selected directory has changed. But the timer will
    // send an event if 500 milliseconds (half a second) have passed since the change.
    // If the change occurs too quickly, the timer is reset and starts counting again.
    // This prevents the file table from being updated too quickly and frequently.
    timer_->setInterval(500);
    timer_->setSingleShot(true);
    connect(timer_, &QTimer::timeout, this, [this]() {
        if (auto const item = currentItem()) {
            if (item == playlists_)
                return;
            if (item == current_selections_) {
                EventController::self().send(event::ShowCurrentSelectedSongs);
                return;
            }
            auto const playlist_id = item->data(0, ID).toULongLong();
            EventController::self().send(event::ShowPlaylistSongs, playlist_id);
        }
    });

    auto p = palette();
    p.setColor(QPalette::Base, QColor{60, 60, 60, 255});
    setAutoFillBackground(true);
    setPalette(p);

    header()->hide();
    setColumnCount(1);
    setHorizontalScrollMode(ScrollPerPixel);

    // When you change the selected item, we reset the timer..
    connect(this, &QTreeWidget::currentItemChanged, this, [this](auto, auto) {
        timer_->stop();
        timer_->start();
    });
    connect(this, &QTreeWidget::itemClicked, this, [this] (auto item, auto _) {
        setCurrentItem(item);
        emit currentItemChanged(item, item);
    });

    update_content();
    setCurrentItem(current_selections_);

    EventController::self().append(this,
        event::NewPlaylistAdded);
}

// Context menu call.
void PlaylistTree::contextMenuEvent(QContextMenuEvent* const event) {
    auto const item = currentItem();
    if (item == nullptr || item == playlists_) return;
    if (item == current_selections_ && Selection::self().empty()) return;

    auto const menu = std::make_shared<QMenu>(this);
    auto const play_action = menu->addAction("Play");
    connect(play_action, &QAction::triggered, this, [this, item](auto _) {
        if (item == current_selections_)
            EventController::self().send(event::StartSelectedPlayback);
        else
            EventController::self().send(event::StartPlaylistPlayback, item->data(0, ID));
    });
    if (item == current_selections_) {
        auto const create_playlist = menu->addAction("Create a playlist from selected songs");
        connect(create_playlist, &QAction::triggered, this, [this](auto _) {
            auto const dialog = make_unique<LineTextEdit>("Plylist name");
            if (dialog->exec() == QDialog::Accepted) {
                 Selection::self().save_as_playlist(dialog->name().toStdString());
            }
        });
    }
    else {
        auto const rename_playlist = menu->addAction("Change playlist name");
        connect(rename_playlist, &QAction::triggered, this, [this](auto _) {
        });
    }
    menu->exec(event->globalPos());
}

void PlaylistTree::mousePressEvent(QMouseEvent* const event) {
    // if (event->button() == Qt::RightButton) {
    //     fmt::print(stderr, "right mouse button\n");
    //     return;
    // }
    QTreeWidget::mousePressEvent(event);
}

PlaylistTree::~PlaylistTree() {
    EventController::self().remove(this);
}

// Handle my own events.
void PlaylistTree::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
    case event::NewPlaylistAdded:
        update_content();
        if (auto const data = e->data(); !data.empty()) {
            auto name = data[0].toString();
            cout << name.toStdString() << '\n' << flush;
            auto const item = item_for(std::move(name));
            scrollToItem(item);
            setCurrentItem(item);
        }
        break;
    }
}

/********************************************************************
*                                                                   *
*                   u p d a t e _ c o n t e n t                     *
*                                                                   *
********************************************************************/

void PlaylistTree::update_content() {
    clear();

    current_selections_ = new QTreeWidgetItem(this);
    current_selections_->setText(0, "Current selections");
    current_selections_->setData(0, ID, -1);
    current_selections_->setData(0, PID, -1);

    playlists_ = new QTreeWidgetItem(this);
    playlists_->setText(0, "Playlists");
    playlists_->setData(1, ID, 0);
    playlists_->setData(0, PID, -1);

    update_playlists();
    playlists_->setExpanded(true);
}

/********************************************************************
*                                                                   *
*                  u p d a t e _ p l a y l i s t s                  *
*                                                                   *
********************************************************************/

auto PlaylistTree::
update_playlists()
-> void
{
    // Read data from table 'playlist'.
    auto const playlist_items = Playlist::all();
    if (playlist_items.empty()) return;

    auto const pid = playlists_->data(0, ID);
    std::ranges::for_each(playlist_items, [this, pid] (auto playlist) {
        auto const item = new QTreeWidgetItem(playlists_);
        item->setText(0, playlist.qname());
        item->setData(0, PID, pid);
        item->setData(0, ID, playlist.qid());
    });

}

/********************************************************************
*                                                                   *
*                         s h o w E v e n t                         *
*                                                                   *
********************************************************************/

void PlaylistTree::showEvent(QShowEvent* event) {
    // cout << "PlaylistTree::showEvent\n" << flush;

    QTreeWidgetItem* item{};

    if (saved_item_) {
        item = saved_item_;
        saved_item_ = nullptr;
    } else if (auto const current_item = currentItem()) {
        item = current_item;
    }

    if (item) {
        // The 'playlists_' tree item itself does not contain songs
        // (it contains playlists that contain songs).
        if (item == playlists_)
            return;

        // Display currently selected songs in the song table.
        if (item == current_selections_) {
            EventController::self().send(event::ShowCurrentSelectedSongs);
            return;
        }

        // A playlist has been selected. Display its songs.
        auto const playlist_id = item->data(0, ID).toULongLong();
        EventController::self().send(event::ShowPlaylistSongs, playlist_id);
    }
}

/********************************************************************
*                                                                   *
*                         h i d e E v e n t                         *
*                                                                   *
********************************************************************/

void PlaylistTree::hideEvent(QHideEvent* event) {
    // cout << "PlaylistTree::hideEvent\n" << flush;
}

void PlaylistTree::focusInEvent(QFocusEvent*) {
    // cout << "PlaylistTree::focusInEvent\n" << flush;
}

void PlaylistTree::focusOutEvent(QFocusEvent*) {
    // cout << "PlaylistTree::focusOutEvent\n" << flush;
}

/********************************************************************
*                                                                   *
*                         i t e m _ f o r                           *
*                                                                   *
********************************************************************/

auto PlaylistTree::
item_for(QString&& path) const
-> QTreeWidgetItem*
{
    QTreeWidgetItemIterator it(playlists_);
    while (*it) {
        QTreeWidgetItem* const item = *it;
        if (item->text(0) == path)
            return item;
        ++it;
    }
    return {};
}


void PlaylistTree::prepare_selection_to_play() const noexcept {

    StartSelectedPlayback:
}
