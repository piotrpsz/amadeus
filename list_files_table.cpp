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
#include "model/selection.h"
#include "list_files_table.h"
#include "shared/event.hh"
#include "shared/event_controller.hh"
#include <QDir>
#include <QMenu>
#include <QEvent>
#include <QAction>
#include <QFileInfo>
#include <QMouseEvent>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QContextMenuEvent>
#include <fmt/core.h>

ListFilesTable::ListFilesTable(QWidget* const parent) : QTableWidget(parent) {
    setRowCount(0);
    setColumnCount(1);
    setEditTriggers(NoEditTriggers);
    setSelectionBehavior(SelectRows);
    setHorizontalHeaderItem(0, new QTableWidgetItem("Title"));

    connect(this, &QTableWidget::cellDoubleClicked, [&] (auto const row, auto const col){
        if (auto const selected_item = item(row, 0)) {
            auto const path = selected_item->data(PATH).toString();
            EventController::self().send(event::SongOneShot, path);
        }
    });
    connect(this, &QTableWidget::itemClicked, this, [this] (QTableWidgetItem* const item) {
        if (item) {
            auto checked = item->checkState();
            auto const path = item->data(PATH).toString();
            if (checked)
                Selection::self().insert(path);
            else
                Selection::self().erase(path);

            setCurrentItem(item);
            update_parent();
        }
    });

    // EventController::instance()
    //     .append(this,
    //             event::DirSelected,
    //             event::CheckingAllSongs);
}

ListFilesTable::~ListFilesTable() {
    EventController::self().remove(this);
}

void ListFilesTable::contextMenuEvent(QContextMenuEvent* const event) {
    auto const menu = new QMenu(this);
    auto const check_all_action = menu->addAction("Select all");
    auto const uncheck_all_action = menu->addAction("Unselect all");
    menu->addSeparator();
    auto const invert_action = menu->addAction("Invert selection");

    connect(check_all_action, &QAction::triggered, this, [this] (auto _) {
        for (auto i = 0; i < rowCount(); ++i) {
            auto const it = item(i, 0);
            if (it->checkState() != Qt::Checked) {
                it->setCheckState(Qt::Checked);
                Selection::self().insert(it->data(PATH).toString());
            }
        }
        update_parent();
    });

    connect(uncheck_all_action, &QAction::triggered, this, [this] (auto _) {
        for (auto i = 0; i < rowCount(); ++i) {
            auto const it = item(i, 0);
            if (it->checkState() != Qt::Unchecked) {
                it->setCheckState(Qt::Unchecked);
                Selection::self().erase(it->data(PATH).toString());
            }
        }
        update_parent();
    });
    connect(invert_action, &QAction::triggered, this, [this] (auto _) {
        for (auto i = 0; i < rowCount(); ++i) {
            auto const it = item(i, 0);
            if (it->checkState() == Qt::Checked) {
                it->setCheckState(Qt::Unchecked);
                Selection::self().erase(it->data(PATH).toString());
            }
            else {
                it->setCheckState(Qt::Checked);
                Selection::self().insert(it->data(PATH).toString());
            }
        }
    });

    menu->exec(event->globalPos());
    delete menu;
}

void ListFilesTable::mousePressEvent(QMouseEvent* const event) {
    if (event->button() == Qt::RightButton)
        return;
    QTableWidget::mousePressEvent(event);
}

// Handle my own events.
void ListFilesTable::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {

    // Event with information that a new album has been selected.
    case event::DirSelected:
        if (auto const data = e->data(); !data.empty()) {
            auto dir = data[0].toString();
            dir_ = dir;
            clear_content();
            new_content_for(std::move(dir));

        }
        break;

    // Event with a request to check/uncheck ALL items.
    case event::CheckingAllSongs:
        if (auto const data = e->data(); !data.empty()) {
            auto const state = data[0].toBool() ? Qt::Checked : Qt::Unchecked;
            auto const n = rowCount();
            for (auto i = 0; i < n; ++i) {
                auto const row = item(i, 0);
                row->setCheckState(state);
                if (state == Qt::Checked)
                    Selection::self().insert(row->data(PATH).toString());
                else
                    Selection::self().erase(row->data(PATH).toString());
            }
        }
        break;
    }
}

void ListFilesTable::new_content_for(QString&& path) {
    QDir const dir{path};
    auto const info_list = dir.entryInfoList();

    std::vector<QTableWidgetItem*> data{};
    data.reserve(info_list.size());
    for (auto const& fi : info_list) {
        if (!fi.isDir() && fi.isFile()) {
            auto const fname = fi.fileName();
            if (fname[0] !='.' && (fname.endsWith(".m4a") || fname.endsWith(".mp3"))) {
                auto item = new QTableWidgetItem(fname);
                auto const path = fi.filePath();
                if (Selection::self().contains(path))
                    item->setCheckState(Qt::Checked);
                else
                    item->setCheckState(Qt::Unchecked);
                item->setData(PATH, path);
                data.push_back(item);
            }
        }
    }

    int row = 0;
    setRowCount(data.size());
    std::ranges::for_each(data, [&] (auto item) {
        setItem(row++, 0, item);
    });
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void ListFilesTable::update_parent() const noexcept {
    if (are_all_unchecked())
        EventController::self().send(event::NoSongsSelected, dir_);
    else if (are_all_checked())
        EventController::self().send(event::AllSongsSelected, dir_);
    else
        EventController::self().send(event::PartlySongsSelected, dir_);
}

bool ListFilesTable::are_all_checked() const noexcept {
    auto const n = rowCount();
    for (auto i = 0; i < n; ++i)
        if (item(i, 0)->checkState() != Qt::Checked)
            return false;
    return true;
}

bool ListFilesTable::are_all_unchecked() const noexcept {
    auto const n = rowCount();
    for (auto i = 0; i < n; ++i)
        if (item(i, 0)->checkState() != Qt::Unchecked)
            return false;
    return true;
}

