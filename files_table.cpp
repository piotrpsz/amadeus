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
#include "files_table.h"
#include "shared/event.hh"
#include "shared/event_controller.hh"
#include <QDir>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <fmt/core.h>

FilesTable::FilesTable(QWidget* const parent) : QTableWidget(parent) {
    setRowCount(0);
    setColumnCount(1);
    setEditTriggers(NoEditTriggers);
    setSelectionBehavior(SelectRows);
    setHorizontalHeaderItem(0, new QTableWidgetItem("Title"));

    connect(this, &QTableWidget::cellDoubleClicked, [&] (auto const row, auto const col){
        if (auto const selected_item = item(row, 0)) {
            auto const path = selected_item->data(PATH).toString();
            EventController::instance().send(event::SongOneShot, path);
        }
    });
    connect(this, &QTableWidget::itemClicked, this, [this] (auto item) {
        fmt::print(stderr, "table item changed\n");
        setCurrentItem(item);
        emit currentItemChanged(item, item);
    });

    EventController::instance()
        .append(this,
                event::DirSelected,
                event::CheckingAllSongs);
}

FilesTable::~FilesTable() {
    EventController::instance().remove(this);
}

void FilesTable::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
    case event::DirSelected:
        if (auto const data = e->data(); !data.empty()) {
            clear_content();
            new_content_for(data[0].toString());
        }
        break;
    case event::CheckingAllSongs:
        if (auto const data = e->data(); !data.empty()) {
            auto const state = data[0].toBool() ? Qt::Checked : Qt::Unchecked;
            auto const n = rowCount();
            for (auto i = 0; i < n; ++i) {
                auto const row = item(i, 0);
                row->setCheckState(state);
                selected_.insert(row->data(PATH).toString());
            }
        }
        break;
    }
}

void FilesTable::new_content_for(QString&& path) {
    QDir const dir{path};
    auto const info_list = dir.entryInfoList();

    std::vector<QTableWidgetItem*> data{};
    data.reserve(info_list.size());
    for (auto const& fi : info_list) {
        if (!fi.isDir() && fi.isFile()) {
            auto const fname = fi.fileName();
            if (fname[0] !='.' && (fname.endsWith(".m4a") || fname.endsWith(".mp3"))) {
                auto item = new QTableWidgetItem(fname);
                item->setCheckState(Qt::Unchecked);
                item->setData(PATH, fi.filePath());
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
