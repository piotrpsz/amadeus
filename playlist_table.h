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

/*------- forward declarations:
-------------------------------------------------------------------*/
class QEvent;
class QShowEvent;
class QMouseEvent;
class QContextMenuEvent;


class PlayListTable : public QTableWidget {
    Q_OBJECT
    enum {PATH = Qt::UserRole + 1, DIR};
    QString dir_{};
public:
    PlayListTable(QWidget* = nullptr);
    ~PlayListTable();
private:
    void showEvent(QShowEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void contextMenuEvent(QContextMenuEvent*) override;
    void customEvent(QEvent*) override;
    void new_content_for(QString&& path);

    void clear_content() noexcept {
        clearContents();
        setRowCount(0);
    }

    void update_content() noexcept;
    void update_parent() const noexcept;
    bool are_all_checked() const noexcept;
    bool are_all_unchecked() const noexcept;
    QTableWidgetItem* item_for(QString&& path) const noexcept;
};
