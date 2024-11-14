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
// Created by Piotr Pszczółkowski on 25.10.2024.
// piotr@beesoft.pl

/*------- include files:
-------------------------------------------------------------------*/
#include "catalog_tree.h"
#include "shared/event.hh"
#include "shared/event_controller.hh"
#include <QDir>
#include <QTimer>
#include <QFileInfo>
#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <fmt/core.h>

DirsTree::DirsTree(QWidget* const parent) :
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
        if (auto item = currentItem(); item) {
            auto path{item->data(0, PATH).toString()};
            EventController::self().send(event::DirSelected, std::move(path));
            if (item->checkState(0) != Qt::PartiallyChecked)
                EventController::self().send(event::CheckingAllSongs, item->checkState(0) == Qt::Checked);
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

    update_content("/home/piotr/Music");
    setCurrentItem(root_);

    EventController::self().append(this,
        event::AllSongsSelected,
        event::NoSongsSelected,
        event::PartlySongsSelected);
}

DirsTree::~DirsTree() {
    EventController::self().remove(this);
}

// Handle my own events.
void DirsTree::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
    case event::AllSongsSelected:
        if (auto const data = e->data(); !data.empty())
            if (auto const item = item_for(data[0].toString()))
                item->setCheckState(0, Qt::Checked);
        break;
    case event::NoSongsSelected:
        if (auto const data = e->data(); !data.empty())
            if (auto const item = item_for(data[0].toString()))
                item->setCheckState(0, Qt::Unchecked);
        break;
    case event::PartlySongsSelected:
        if (auto const data = e->data(); !data.empty())
            if (auto const item = item_for(data[0].toString()))
                item->setCheckState(0, Qt::PartiallyChecked);
        break;
    }
}

void DirsTree::update_content(QString const& path) {
    clear();
    root_ = new QTreeWidgetItem(this);
    root_->setText(0, "Performer");
    root_->setData(0, ID, 0);
    root_->setData(0, PID, -1);
    root_->setData(0, PATH, path);

    add_items_for(root_);
    update_if_checkable();
    root_->setExpanded(true);
}

auto DirsTree::
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

auto DirsTree::
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

void DirsTree::update_if_checkable() const noexcept {
    if (root_) {
        auto const n = root_->childCount();
        if (n > 0) {
            for (auto i = 0; i < n; ++i) {
                auto const item = root_->child(i);
                if (0 == item->childCount())
                    // if item don't have childs is self checkable,
                    item->setCheckState(0, Qt::Unchecked);
            }
        }
    }
}
