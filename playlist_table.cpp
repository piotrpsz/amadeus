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
#include "model/song.h"
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
// #include <format>
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

    EventController::self()
        .append(this,
                event::ShowCurrentSelectedSongs,
                event::ShowPlaylistSongs,
                event::SelectionChanged,
                event::SongPlayed);
}

PlaylistTable::~PlaylistTable() {
    EventController::self().remove(this);
}

/********************************************************************
 *                                                                  *
 *                  f o c u s O u t E v e n t                       *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::focusOutEvent(QFocusEvent* e) {
    // cout << "PlaylistTable::focusOutEvent\n" << flush;
    if (auto const item = currentItem())
        saved_[current_playlist_id_] = item->data(PATH).toString();
}

/********************************************************************
 *                                                                  *
 *                  f o c u s I n E v e n t                         *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::focusInEvent(QFocusEvent* e) {
    // cout << "PlaylistTable::focusInEvent\n" << flush;
}

/********************************************************************
 *                                                                  *
 *                     s h o w E v e n t                            *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::showEvent(QShowEvent* event) {
    // cout << format("PlaylistTable::showEvent saved: {}\n", *saved_) << flush;
    // if (saved_) {
    //     select(item(*saved_, 0));
    //     saved_ = {};
    // }
}

void PlaylistTable::hideEvent(QHideEvent* event) {
    // cout << "PlaylistTable::hideEvent\n" << flush;
    // if (int row = currentRow(); row != -1)
    //     saved_ = row;
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

    // Show songs currently selected.
    case event::ShowCurrentSelectedSongs:
        content_for_selections();
        break;

    // Show songs for passed playlist.
    case event::ShowPlaylistSongs:
        if (auto const data = e->data(); !data.empty()) {
            auto const playlist_id = data[0].toUInt();
            content_for_playlist(playlist_id);
        }
        break;

    // Currently playing song.
    case event::SongPlayed:
         if (auto const data = e->data(); !data.empty())
            if (auto const it = item_for(data[0].toString()))
                select(it);
        break;
    }
}

/********************************************************************
 *                                                                  *
 *           c o n t e n t _ f o r _ s e l e c t i o n s            *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::content_for_selections() noexcept {
    clear_content();

    if (!Selection::self().empty()) {
        int row{};
        setRowCount(Selection::self().size());
        std::ranges::for_each(Selection::self(), [this, &row] (auto const& path) {
            QFileInfo const fi{ QString::fromStdString(path)};
            auto const item = new QTableWidgetItem(fi.fileName());
            item->setData(PATH, fi.filePath());
            setItem(row++, 0, item);
        });
        current_playlist_id_ = 0;
        update_selected();
    }
}

/********************************************************************
 *                                                                  *
 *             c o n t e n t _ f o r _ p l a y l i s t              *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::content_for_playlist(uint playlist_id) noexcept {
    clear_content();

    if (auto songs = Song::for_pid(playlist_id); !songs.empty()) {
        int row{};
        setRowCount(songs.size());
        std::ranges::for_each(songs, [this, &row] (auto&& song){
            QFileInfo const fi{song.qpath()};
            auto const item = new QTableWidgetItem(fi.fileName());
            item->setData(PATH, fi.filePath());
            setItem(row++, 0, item);
        });
        current_playlist_id_ = playlist_id;
        update_selected();
    }
}

/********************************************************************
 *                                                                  *
 *                  u p d a t e _ s e l e c t e d                   *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::update_selected() noexcept {
    if (auto const it = saved_.find(current_playlist_id_); it != saved_.end()) {
        select(item_for(std::move(it->second)));
        saved_.erase(it);
        return;
    }
    if (rowCount())
        selectRow(0);
}

/********************************************************************
 *                                                                  *
 *                          s e l e c t                             *
 *                                                                  *
 *******************************************************************/

void PlaylistTable::select(QTableWidgetItem* const item) {
    if (item) {
        scrollToItem(item);
        setCurrentItem(item);
        setFocus();
        return;
    }
    selectRow(0);
    setFocus();
}

/********************************************************************
 *                                                                  *
 *                        i t e m _ f o r                           *
 *                                                                  *
 *******************************************************************/

QTableWidgetItem* PlaylistTable::item_for(QString&& path) const noexcept {
    for (auto i = 0; i < rowCount(); ++i)
        if (auto const it = item(i, 0); (it->data(PATH) == path))
            return it;
    return {};
}
