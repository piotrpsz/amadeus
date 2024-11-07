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
// Created by Piotr Pszczółkowski on 7.11.2024.
// piotr@beesoft.pl

/*------- include files:
-------------------------------------------------------------------*/
#include "progress.h"
#include "shared/event_controller.hh"
#include <format>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>

using namespace std;;

Progress::Progress(QWidget *parent)
    : QWidget{parent}
    , slider_{new QSlider(Qt::Horizontal)}
    , passed_{new QLabel}
    , left_{new QLabel}
{
    slider_->setMinimum(0);
    slider_->setMaximum(1000);
    slider_->setValue(0);
    slider_->setSingleStep(1);
    slider_->setToolTip("Song playback progress");
    slider_->setToolTipDuration(3000);
    connect(slider_, &QSlider::sliderMoved, this, [](auto value) {
        EventController::self().send(event::SongReprogress, value);
    });

    auto const main = new QHBoxLayout;
    main->addWidget(passed_);
    main->addWidget(slider_);
    main->addWidget(left_);
    setLayout(main);

    EventController::self().append(this, event::SongRange, event::SongProgress);
}

Progress::~Progress() {
    EventController::self().remove(this);
}

// Handle my own events.
void Progress::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
    case event::SongRange:
        if (auto const data = e->data(); data.size() == 1) {
            auto const max = data[0].toULongLong();

            passed_->setText("0s");
            left_->setText(format_time(max));
            slider_->setMaximum(max);
            slider_->setMinimum(0);
            slider_->setValue(0);
            slider_->setSingleStep(1);
        }
        break;
    case event::SongProgress:
        if (auto const data = e->data(); data.size() == 1) {
            auto const position = data[0].toULongLong();
            passed_->setText(format_time(position));
            left_->setText(format_time(slider_->maximum() - position));
            slider_->setValue(position);
        }
        break;
    }
}

QString Progress::format_time(qint64 ms) noexcept {
    if (auto sec = ms / 1000) {
        if (auto min = sec / 60) {
            return QString::fromStdString(format("{:02}:{:02}", min, sec % 60));
        }
        return QString::fromStdString(format("{:02}:{:02}", 0, sec));
    }
    return QString("0s");
}
