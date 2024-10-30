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
#include <fmt/core.h>

PlayListTree::PlayListTree(QWidget* const parent) :
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
        // if (auto item = currentItem(); item) {
        //     auto path{item->data(0, PATH).toString()};
        //     EventController::self().send(event::DirSelected, std::move(path));
        //     if (item->checkState(0) != Qt::PartiallyChecked)
        //         EventController::self().send(event::CheckingAllSongs, item->checkState(0) == Qt::Checked);
        // }
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
    setCurrentItem(current_);

    EventController::self().append(this,
        event::None);
}

// Context menu call.
void PlayListTree::contextMenuEvent(QContextMenuEvent* const event) {
    auto const item = currentItem();
    if (item == nullptr || item == root_) return;
    if (item == current_ && Selection::self().empty()) return;

    auto const menu = std::make_shared<QMenu>(this);
    auto const play_action = menu->addAction("Play");
    connect(play_action, &QAction::triggered, this, [this](auto _) {
        EventController::self().send(event::StartSelectedPlayback);
    });
    if (item == current_) {
        auto const create_playlist = menu->addAction("Create a playlist from selected songs");
        connect(create_playlist, &QAction::triggered, this, [this](auto _) {
            fmt::print(stderr, "create action\n");
        });
    }
    else {
        auto const rename_playlist = menu->addAction("Change playlist name");
        connect(rename_playlist, &QAction::triggered, this, [this](auto _) {
            fmt::print(stderr, "rename action\n");
        });
    }
    menu->exec(event->globalPos());
}

void PlayListTree::mousePressEvent(QMouseEvent* const event) {
    // if (event->button() == Qt::RightButton) {
    //     fmt::print(stderr, "right mouse button\n");
    //     return;
    // }
    QTreeWidget::mousePressEvent(event);
}

PlayListTree::~PlayListTree() {
    EventController::self().remove(this);
}

// Handle my own events.
void PlayListTree::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
    case event::SelectionChanged:
        // update_content();
        break;
    }
}

void PlayListTree::update_content() {
    clear();
    current_ = new QTreeWidgetItem(this);
    current_->setText(0, "Current selections");
    current_->setData(0, ID, -1);
    current_->setData(0, PID, -1);


    root_ = new QTreeWidgetItem(this);
    root_->setText(0, "Playlists");
    root_->setData(1, ID, 0);
    root_->setData(0, PID, -1);

    // add_items_for(root_);
    root_->setExpanded(true);
}

auto PlayListTree::
add_items_for(QTreeWidgetItem* const parent)
-> void {
    auto parent_path = parent->data(0, PATH).toString();
    QDir const dir{parent_path};
    auto const content = dir.entryInfoList();

    int const pid = parent->data(0, ID).toInt();
    int did = pid;
    for (auto const& fi : content) {
        if (fi.isDir()) {
            auto const fname = fi.fileName();
            if (fname[0] !='.') {
                auto const item = new QTreeWidgetItem(parent);
                item->setText(0, fname);
                if (pid != 0)
                    item->setCheckState(0, Qt::Unchecked);
                item->setData(0, PATH, fi.filePath());
                item->setData(0, PID, pid);
                item->setData(0, ID, ++did);
                add_items_for(item);
            }
        }
    }
}

auto PlayListTree::
item_for(QString&& path) const
-> QTreeWidgetItem* {
    QTreeWidgetItemIterator it(root_);
    while (*it) {
        QTreeWidgetItem* const item = *it;
        if (item->data(0, PATH).toString() == path)
            return item;
        ++it;
    }
    return {};
}

