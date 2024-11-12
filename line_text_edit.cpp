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
// Created by Piotr Pszczółkowski on 10.11.2024.
// piotr@beesoft.pl

/*------- include files:
-------------------------------------------------------------------*/
#include "line_text_edit.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


LineTextEdit::LineTextEdit(QString const& prompt_text, QString previous_value, QWidget* const parent)
    : QDialog{parent}
    , editor_{new QLineEdit}
{
    auto prompt = new QLabel(prompt_text + ": ");
    if (!previous_value.isEmpty())
        editor_->setText(previous_value);

    auto const fields = new QHBoxLayout;
    fields->setContentsMargins(0, 10, 0, 10);
    fields->addWidget(prompt);
    fields->addWidget(editor_);

    auto const accept_btn = new QPushButton("Accept");
    auto const reject_btn = new QPushButton("Reject");

    auto const buttons = new QHBoxLayout;
    // buttons->setContentsMargins(5, 5, 5, 5);
    buttons->addWidget(reject_btn);
    buttons->addStretch();
    buttons->addWidget(accept_btn);

    auto const main = new QVBoxLayout;
    main->setContentsMargins(10, 10, 10, 10);
    main->addLayout(fields);
    main->addLayout(buttons);
    setLayout(main);

    setMinimumWidth(350);
    setMaximumWidth(350);
    setMinimumHeight(100);
    setMaximumHeight(100);

    connect(accept_btn, &QPushButton::clicked, this, [this] {
        if (editor_->text().trimmed().isEmpty()) {
            reject();
            return;
        }
        accept();
    });
    connect(reject_btn, &QPushButton::clicked, this, &QDialog::reject);
}

QString LineTextEdit::name() noexcept {
    return editor_->text().trimmed();
}
