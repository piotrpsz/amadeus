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
#include "dirs_tree.h"
#include "shared/event.hh"
#include "shared/event_controller.hh"
#include <QDir>
#include <QTimer>
#include <QFileInfo>
#include <QHeaderView>
#include <QTreeWidgetItem>
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
            EventController::instance().send(event::DirSelected, std::move(path));
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

    update_content("/home/piotr/Music");
    setCurrentItem(root_);
}



void DirsTree::update_content(QString const& path) {
    clear();
    root_ = new QTreeWidgetItem(this);
    root_->setText(0, "Performer");
    root_->setData(0, ID, 0);
    root_->setData(0, PID, -1);
    root_->setData(0, PATH, path);

    add_items_for(root_);
    root_->setExpanded(true);
}

void DirsTree::add_items_for(QTreeWidgetItem* const parent) {
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
                item->setData(0, PATH, fi.filePath());
                item->setData(0, PID, pid);
                item->setData(0, ID, ++did);
                add_items_for(item);
            }
        }
    }
}


