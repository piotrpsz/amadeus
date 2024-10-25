
/*------- include files:
-------------------------------------------------------------------*/
#include "control_bar.h"
#include <QUrl>
#include <QDir>
#include <QIcon>
#include <QLabel>
#include <QSlider>
#include <QStyle>
#include <QFileInfo>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <span>
#include <fmt/core.h>

ControlBar::ControlBar(QWidget *parent) :
    QWidget{parent},
    player_{new QMediaPlayer},
    audio_output_{new QAudioOutput},
    play_icon_{style()->standardIcon(QStyle::SP_MediaPlay)},
    pause_icon_{style()->standardIcon(QStyle::SP_MediaPause)},
    volume_icon_{style()->standardIcon(QStyle::SP_MediaVolume)},
    volume_muted_icon_{style()->standardIcon(QStyle::SP_MediaVolumeMuted)},
    volume_btn_{new QPushButton},
    play_pause_btn_{new QPushButton()},
    sound_slide_{new QSlider(Qt::Horizontal)},
    performer_{new QLabel},
    album_{new QLabel},
    title_{new QLabel}

{
    title_->setToolTipDuration(DEFAULT_TIP_DURATION);

    // sound volume slider settings
    sound_slide_->setMinimum(0);
    sound_slide_->setMaximum(100);
    sound_slide_->setToolTip("volume level");
    sound_slide_->setTickInterval(5);
    sound_slide_->setTickPosition(QSlider::TicksBothSides);
    sound_slide_->setToolTipDuration(DEFAULT_TIP_DURATION);
    connect(sound_slide_, &QSlider::sliderMoved, this, [this] (int const position) {
        audio_output_->setVolume(position/100.);
    });

    // audio (player & output) settings
    player_->setAudioOutput(audio_output_);
    connect(player_, &QMediaPlayer::positionChanged, [](auto pos) {
        // TODO handle changed position
        // fmt::print(stderr, "{}\n", pos);
    });

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
        if ((played_ = !played_)) {
            play_pause_btn_->setIcon(pause_icon_);
            play_pause_btn_->setToolTip(PAUSE_TIP);
            player_->play();
        }
        else {
            play_pause_btn_->setIcon(play_icon_);
            play_pause_btn_->setToolTip(PLAY_TIP);
            player_->pause();
        }
    });

    auto const skip_backward_btn = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipBackward), "");
    skip_backward_btn->setFlat(true);
    skip_backward_btn->setToolTip("play previous");
    skip_backward_btn->setToolTipDuration(DEFAULT_TIP_DURATION);

    auto const skip_forward_btn = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipForward), "");
    skip_forward_btn->setFlat(true);
    skip_forward_btn->setToolTip("play next");
    skip_forward_btn->setToolTipDuration(DEFAULT_TIP_DURATION);

    // Sound slider settings.
    // sound_slide_->setMinimum(0);
    // sound_slide_->setMaximum(100);
    // sound_slide_->setToolTip("volume level");
    // sound_slide_->setToolTipDuration(DEFAULT_TIP_DURATION);

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

    set_song("/home/piotr/Music/Achim Reichel/Melancholie und Sturmflut/05 Aloha Heja He.m4a");
}

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
    player_->setSource(QUrl::fromLocalFile(path));
}
