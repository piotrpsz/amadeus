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
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QTableWidget>
#include <optional>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QEvent;
class QHideEvent;
class QShowEvent;
class QMouseEvent;
class QTableWidgetItem;
class QContextMenuEvent;


/*------- PlaylistTable ::QTableWidget:
-------------------------------------------------------------------*/
class PlaylistTable : public QTableWidget {
    Q_OBJECT
    enum {PATH = Qt::UserRole + 1, DIR};
    QString dir_{};
    std::optional<int> saved_{};
public:
    PlaylistTable(QWidget* = nullptr);
    ~PlaylistTable();
private:
    void showEvent(QShowEvent*) override;
    void hideEvent(QHideEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    // void contextMenuEvent(QContextMenuEvent*) override;
    void customEvent(QEvent*) override;

    void clear_content() noexcept {
        // Remove content (and only content, no settings)
        clearContents();
        setRowCount(0);
    }

    void content_for_selections() noexcept;
    void content_for_playlist(uint playlist_id) noexcept;
    QTableWidgetItem* item_for(QString&& path) const noexcept;

    void focusInEvent(QFocusEvent*) override;
    void focusOutEvent(QFocusEvent*) override;
    void select(QTableWidgetItem*);
};
