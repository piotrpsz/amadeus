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
// Created by Piotr Pszczółkowski on 8.11.2024.
// piotr@beesoft.pl

/*------- include files:
-------------------------------------------------------------------*/
#include "control_bar.h"
#include "shared/event.hh"
#include "shared/event_controller.hh"
#include "model/selection.h"
#include "model/song.h"
#include "tool.h"
#include <QUrl>
#include <QDir>
#include <QIcon>
#include <QLabel>
#include <QEvent>
#include <QSlider>
#include <QStyle>
#include <QFileInfo>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <span>
#include <iostream>
#include <format>


using namespace std;

ControlBar::ControlBar(QWidget* const parent)
    : QWidget{parent}
    , player_{new QMediaPlayer}
    , audio_output_{new QAudioOutput}
    , play_icon_{style()->standardIcon(QStyle::SP_MediaPlay)}
    , pause_icon_{style()->standardIcon(QStyle::SP_MediaPause)}
    , volume_icon_{style()->standardIcon(QStyle::SP_MediaVolume)}
    , volume_muted_icon_{style()->standardIcon(QStyle::SP_MediaVolumeMuted)}
    , volume_btn_{new QPushButton}
    , play_pause_btn_{new QPushButton()}
    , sound_slide_{new QSlider(Qt::Horizontal)}
    , performer_{new QLabel}
    , album_{new QLabel}
    , title_{new QLabel}
{
    title_->setToolTipDuration(DEFAULT_TIP_DURATION);

    // Sound volume slider settings
    sound_slide_->setMinimum(0);
    sound_slide_->setMaximum(100);
    sound_slide_->setToolTip("volume level");
    sound_slide_->setTickInterval(5);
    sound_slide_->setTickPosition(QSlider::TicksBothSides);
    sound_slide_->setToolTipDuration(DEFAULT_TIP_DURATION);
    connect(sound_slide_, &QSlider::sliderMoved, this, [this] (int const position) {
        audio_output_->setVolume(position/100.);
    });

    // audio (player & output) settings -----------------------------
    player_->setAudioOutput(audio_output_);
    // The playback position changed while the song was playing.
    // Information for the playback progress slider.
    connect(player_, &QMediaPlayer::positionChanged, this, [this](auto pos) {
        if (pos != previous_position_) {
            EventController::self().send(event::SongProgress, pos);
            previous_position_ = pos;
        }
    });
    // Information about the duration of the song.
    // Information for the playback progress slider (slider scaling).
    connect(player_, &QMediaPlayer::durationChanged, this, [this](auto pos) {
        if (pos != previous_duration_) {
            EventController::self().send(event::SongRange, pos);
            previous_duration_ = pos;
        }
    });
    // The song has finished playing.
    connect(player_, &QMediaPlayer::mediaStatusChanged, this, [this](auto status) {
        switch (status) {
        case QMediaPlayer::EndOfMedia:
            play_next();
            break;
        default:
        {}
        }
    });

    // Controller button settings -----------------------------------
    connect(audio_output_, &QAudioOutput::volumeChanged, this, [this] (double volume) {
        sound_slide_->setValue(100. * volume);
    });

    audio_output_->setVolume(DEFAULT_VOLUME/100.);

    // volume button/icon settings
    volume_btn_->setFlat(true);
    volume_btn_->setIcon(volume_icon_);
    volume_btn_->setToolTip(AUDIBLE_TIP);
    volume_btn_->setToolTipDuration(DEFAULT_TIP_DURATION);
    connect(volume_btn_, &QPushButton::clicked, this, [this] (auto _) {
        if ((muted_ = !muted_)) {
            volume_btn_->setIcon(volume_muted_icon_);
            volume_btn_->setToolTip(MUTE_TIP);
            sound_slide_->setEnabled(false);
        } else {
            volume_btn_->setIcon(volume_icon_);
            volume_btn_->setToolTip(AUDIBLE_TIP);
            sound_slide_->setEnabled(true);
        }
        audio_output_->setMuted(muted_);
    });

    // play/pause button/pause settings
    play_pause_btn_->setFlat(true);
    play_pause_btn_->setIcon(play_icon_);
    play_pause_btn_->setToolTip(PLAY_TIP);
    play_pause_btn_->setToolTipDuration(DEFAULT_TIP_DURATION);
    connect(play_pause_btn_, &QPushButton::clicked, this, [this] (auto _) {
        played_ = !played_;
        playback_changed();
    });

    auto const skip_backward_btn = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipBackward), "");
    skip_backward_btn->setFlat(true);
    skip_backward_btn->setToolTip("play previous");
    skip_backward_btn->setToolTipDuration(DEFAULT_TIP_DURATION);
    connect(skip_backward_btn, &QPushButton::clicked, this, [this] (auto _) {
        play_prev();
    });

    auto const skip_forward_btn = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipForward), "");
    skip_forward_btn->setFlat(true);
    skip_forward_btn->setToolTip("play next");
    skip_forward_btn->setToolTipDuration(DEFAULT_TIP_DURATION);
    connect(skip_forward_btn, &QPushButton::clicked, this, [this] (auto _) {
        play_next();
    });

    // Lyout with buttons to manage playback.
    auto const play_layout = new QHBoxLayout;
    play_layout->addWidget(skip_backward_btn);
    play_layout->addWidget(play_pause_btn_);
    play_layout->addWidget(skip_forward_btn);

    // Layout with information about the song.
    auto const info_layout = new QHBoxLayout;
    // info_layout->addWidget(new QLabel("Performer:"));
    info_layout->addWidget(performer_);
    // info_layout->addWidget(new QLabel("Album:"));
    info_layout->addWidget(album_);
    // info_layout->addWidget(new QLabel("Title:"));
    info_layout->addWidget(title_);

    // The Layout That Rules Them All.
    auto const layout = new QHBoxLayout;
    layout->addLayout(info_layout);
    layout->addStretch();
    layout->addWidget(volume_btn_);
    layout->addWidget(sound_slide_);
    layout->addSpacing(10);
    layout->addLayout(play_layout);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    EventController::self().append(this,
        event::SongShot,
        event::SongOneShot,
        event::StartSelectedPlayback,
        event::StartPlaylistPlayback,
        event::SongReprogress,
        event::SelectionChanged);
}

ControlBar::~ControlBar() {
    EventController::self().remove(this);
}

/********************************************************************
 *                                                                  *
 *                      s h o w E v e n t                           *
 *                                                                  *
 *******************************************************************/

void ControlBar::showEvent(QShowEvent* const event) {
    // Tylko ControlBar wie że w czasie gdy aplikacja
    // była ukryta nastąpiła zmiana utworu. Musimy powiadomić
    // resztę programu o tym fakcie.
    // Np. PlayListTable aby zmieniła zaznaczenie utworu.
    if (!song_path_.isEmpty())
        EventController::self().send(event::SongPlayed, song_path_);
}

/********************************************************************
 *                                                                  *
 *                  c u s t o m E v e n t                           *
 *                                                                  *
 *******************************************************************/

void ControlBar::customEvent(QEvent* const event) {
    auto const e = dynamic_cast<Event*>(event);
    switch (int(e->type())) {
    case event::SongShot:
        if (auto const data = e->data(); !data.empty()) {
            auto const path = data[0].toString();
            lock_guard<mutex> lg{mutex_};
            if (auto idx = song_idx(path); idx != -1) {
                idx_ = idx;
                set_song(songs_[idx_]);
            }
        }
        break;
    case event::SongOneShot:
        if (auto const data = e->data(); !data.empty()) {
            auto const path = data[0].toString();
            one_shot_ = true;
            lock_guard<mutex> lg{mutex_};
            // A song was selected from the collection.
            if (auto idx = song_idx(path); idx != -1) {
                idx_ = idx;
                set_song(songs_[idx_]);
                return;
            }
            // Aa song from outside the collection was selected
            if (played_) saved_idx_ = idx_;
            set_song(path);
        }
        break;
    case event::SongReprogress:
        if (auto const data = e->data(); !data.empty()) {
            auto const position = data[0].toULongLong();
            if (played_)
                player_->setPosition(position);
        }
        break;
    // User would like to start play selections.
    case event::StartSelectedPlayback: {
            lock_guard<mutex> lg{mutex_};
            if (!Selection::self().empty()) {
                songs_ = Selection::self().to_list();
                set_song(songs_[idx_ = 0]);
            }
        }
        break;
    // User would to start play the playlist.
        case event::StartPlaylistPlayback: {
            lock_guard<mutex> lg{mutex_};
            songs_.clear();
            if (auto const data = e->data(); !data.empty()) {
                auto const playlist_id = data[0].toUInt();
                auto const playlist_songs = Song::all_for(playlist_id);
                songs_.reserve(playlist_songs.size());
                std::ranges::for_each(playlist_songs, [this] (auto&& song) {
                    songs_ << song.qpath();
                });
                if (!songs_.isEmpty())
                    set_song(songs_[idx_ = 0]);
            }
        }
        break;
    case event::SelectionChanged: {
            lock_guard<mutex> lg{mutex_};
            songs_ = Selection::self().to_list();
        }
        break;
    }
}

/********************************************************************
 *                                                                  *
 *                      p l a y _ n e x t                           *
 *                                                                  *
 *******************************************************************/

void ControlBar::play_next() noexcept {
    lock_guard<mutex> lg{mutex_};

    if (songs_.empty())
        return;

    if (saved_idx_ > -1) {
        idx_ = saved_idx_;
        set_song(songs_[idx_]);
        saved_idx_ = -1;
        return;
    }

    if ((idx_ + 1) < songs_.size())
        set_song(songs_[++idx_]);
}

/********************************************************************
 *                                                                  *
 *                      p l a y _ p r e v                           *
 *                                                                  *
 *******************************************************************/

void ControlBar::play_prev() noexcept {
    lock_guard<mutex> lg{mutex_};

    if (songs_.empty())
        return;

    if((idx_ - 1) >= 0)
        set_song(songs_[--idx_]);
}

/********************************************************************
 *                                                                  *
 *                      p l a y _ s o n g                           *
 *                                                                  *
 *******************************************************************/

void ControlBar::set_song(QString const& path) noexcept {
    auto items = path.split(QDir::separator());
    if (items.size() < 4)
        return;

    std::vector<QString> data{};
    data.reserve(items.size());
    std::ranges::for_each(items, [&data] (auto&& item) {
        data.push_back(std::move(item));
    });
    std::span<QString> span{data};

    auto title = span.last(1)[0];
    if (auto idx = title.lastIndexOf('.'); idx != -1)
        title = title.chopped(title.size() - idx);
    title_->setText(QString("Title: <b><font color=#ffc66d>%1</font></b>")
                        .arg(title));

    span = span.subspan(0, span.size() - 1);
    album_->setText(QString("Album: <b><font color=#5aab73>%1</font></b>")
                        .arg(span.last(1)[0]));
    span = span.subspan(0, span.size() - 1);
    performer_->setText(QString("Performer: <b><font color=#2aacb8>%1</font></b>")
                            .arg(span.last(1)[0]));
    title_->setToolTip(path);

    // Set player.
    song_path_ = path;
    player_->setSource(QUrl::fromLocalFile(path));
    player_->play();
    played_ = true;
    playback_changed();
    EventController::self().send(event::SongPlayed, path);
}

void ControlBar::playback_changed() const noexcept {
    if (played_) {
        play_pause_btn_->setIcon(pause_icon_);
        play_pause_btn_->setToolTip(PAUSE_TIP);
        player_->play();
        return;
    }
    play_pause_btn_->setIcon(play_icon_);
    play_pause_btn_->setToolTip(PLAY_TIP);
    player_->pause();
}
