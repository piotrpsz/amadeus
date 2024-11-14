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
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QWidget>
#include <QIcon>
#include <QStringList>
#include <mutex>

/*------- forward eclarations:
-------------------------------------------------------------------*/
class QLabel;
class QSlider;
class QEvent;
class QShowEvent;
class QPushButton;
class QMediaPlayer;
class QAudioOutput;

/*------- ControlBar ::QWidget:
-------------------------------------------------------------------*/
class ControlBar : public QWidget {
    Q_OBJECT

    static inline QString const PLAY_TIP{"play"};
    static inline QString const PAUSE_TIP{"pause"};
    static inline QString const AUDIBLE_TIP{"audible"};
    static inline QString const MUTE_TIP{"mute"};
    static int const DEFAULT_TIP_DURATION{2000};
    static int const DEFAULT_VOLUME{40};

    QMediaPlayer* const player_;
    QAudioOutput* const audio_output_;
    QIcon play_icon_;
    QIcon pause_icon_;
    QIcon volume_icon_;
    QIcon volume_muted_icon_;
    QPushButton* const volume_btn_;
    QPushButton* const play_pause_btn_;
    QSlider* const sound_slide_;
    QLabel* const performer_;
    QLabel* const album_;
    QLabel* const title_;

    bool played_{};
    bool muted_{};
    bool one_shot_{};
    QString song_path_{};
    QStringList songs_{};
    int idx_ = -1;
    int saved_idx_ = -1;
    qint64 previous_position_{};
    qint64 previous_duration_{};
    std::mutex mutex_{};
public:
    explicit ControlBar(QWidget *parent = nullptr);
    ~ControlBar();
private:
    void play_next() noexcept;
    void play_prev() noexcept;
    void set_song(QString const& path) noexcept;
    void playback_changed() const noexcept;
    void showEvent(QShowEvent*) override;
    void customEvent(QEvent*) override;

    int song_idx(QString const& qpath) const noexcept {
        auto const path =qpath.toStdString();
        for (auto i = 0; i < songs_.size(); ++i)
            if (songs_[i] == path)
                return i;
        return -1;
    }
};

