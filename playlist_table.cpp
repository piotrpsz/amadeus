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
#include "playlist_table.h"
#include "shared/event.hh"
#include "shared/event_controller.hh"
#include <QDir>
#include <QMenu>
#include <QEvent>
#include <QAction>
#include <QFileInfo>
#include <QShowEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QContextMenuEvent>
#include <iostream>
#include <format>
using namespace std;

PlaylistTable::PlaylistTable(QWidget* const parent) : QTableWidget(parent) {
    setRowCount(0);
    setColumnCount(1);
    setEditTriggers(NoEditTriggers);
    setSelectionBehavior(SelectRows);
    setHorizontalHeaderItem(0, new QTableWidgetItem("Title"));
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(this, &QTableWidget::cellDoubleClicked, [&] (auto const row, auto const col){
        if (auto const selected_item = item(row, 0)) {
            auto const path = selected_item->data(PATH).toString();
            EventController::self().send(event::SongShot, path);
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

            select(item);
            update_parent();
        }
    });

    EventController::self()
        .append(this,
                event::ShowCurrentSelectedSongs,
                event::ShowPlaylistSongs,
                event::SelectionChanged,
                event::SongPlayed);

    update_content();
}

PlaylistTable::~PlaylistTable() {
    EventController::self().remove(this);
}

/********************************************************************
 *                                                                  *
 *               c o n t e x t M e n u E v e n t                    *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::contextMenuEvent(QContextMenuEvent* const event) {
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

/********************************************************************
 *                                                                  *
 *                  f o c u s O u t E v e n t                       *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::focusOutEvent(QFocusEvent* e) {
    if (int row = currentRow(); row != -1)
        saved_ = row;
}

/********************************************************************
 *                                                                  *
 *                  f o c u s I n E v e n t                         *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::focusInEvent(QFocusEvent* e) {
    if (saved_) {
        select(item(*saved_, 0));
        saved_ = {};
    }
}

/********************************************************************
 *                                                                  *
 *                     s h o w E v e n t                            *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::showEvent(QShowEvent* event) {
    update_content();
    setFocus();
}

void PlaylistTable::hideEvent(QHideEvent* event) {
    if (int row = currentRow(); row != -1)
        saved_ = row;
}



/********************************************************************
 *                                                                  *
 *               m o u s e P r e s s E v e n t                      *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::mousePressEvent(QMouseEvent* const event) {
    if (event->button() == Qt::RightButton)
        return;
    QTableWidget::mousePressEvent(event);
}

/********************************************************************
 *                                                                  *
 *                  c u s t o m E v e n t                           *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {

    case event::ShowCurrentSelectedSongs:
        cout << "PlaylistTable::ShowCurrentSelectedSongs\n" << flush;

        break;
    case event::ShowPlaylistSongs:
        cout << "PlaylistTable::ShowPlaylistSongs\n" << flush;
        break;

    // Event with a request to check/uncheck ALL items.
    case event::CheckingAllSongs:
        cout << "PlaylistTable::CheckingAllSongs\n" << flush;
        // if (auto const data = e->data(); !data.empty()) {
        //     auto const state = data[0].toBool() ? Qt::Checked : Qt::Unchecked;
        //     auto const n = rowCount();
        //     for (auto i = 0; i < n; ++i) {
        //         auto const row = item(i, 0);
        //         row->setCheckState(state);
        //         if (state == Qt::Checked)
        //             Selection::self().insert(row->data(PATH).toString());
        //         else
        //             Selection::self().erase(row->data(PATH).toString());
        //     }
        // }
        break;

    // Currently playing song.
    case event::SongPlayed:
        cout << "PlaylistTable::SongPlayed\n" << flush;
        // if (auto const data = e->data(); !data.empty())
        //     if (auto const it = item_for(data[0].toString()))
        //         select(it);
        break;
    }
}

void PlaylistTable::new_content_for(QString&& path) {
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

auto PlaylistTable::item_for(QString&& path) const noexcept
    -> QTableWidgetItem*
{
    auto const n = rowCount();
    for (auto i = 0; i < n; ++i) {
        auto it = item(i, 0);
        if (path == it->data(PATH).toString())
            return it;
    }
    return {};
}

void PlaylistTable::update_content() noexcept {
    clear_content();

    int row{};
    setRowCount(Selection::self().size());
    std::ranges::for_each(Selection::self(), [this, &row] (auto const path) {
        auto const qpath = QString::fromStdString(path);
        QFileInfo const fi{qpath};
        auto item = new QTableWidgetItem(fi.fileName());
        item->setData(PATH, fi.filePath());
        setItem(row++, 0, item);
    });
}

void PlaylistTable::update_parent() const noexcept {
    if (are_all_unchecked())
        EventController::self().send(event::NoSongsSelected, dir_);
    else if (are_all_checked())
        EventController::self().send(event::AllSongsSelected, dir_);
    else
        EventController::self().send(event::PartlySongsSelected, dir_);
}

bool PlaylistTable::are_all_checked() const noexcept {
    auto const n = rowCount();
    for (auto i = 0; i < n; ++i)
        if (item(i, 0)->checkState() != Qt::Checked)
            return false;
    return true;
}

bool PlaylistTable::are_all_unchecked() const noexcept {
    auto const n = rowCount();
    for (auto i = 0; i < n; ++i)
        if (item(i, 0)->checkState() != Qt::Unchecked)
            return false;
    return true;
}

void PlaylistTable::select(QTableWidgetItem* const item) {
    setFocus();
    scrollToItem(item);
    setCurrentItem(item);
}
