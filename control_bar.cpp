#include "control_bar.h"
#include <QIcon>
#include <QLabel>
#include <QSlider>
#include <QStyle>
#include <QPushButton>
#include <QHBoxLayout>

// "/home/piotr/Music/Achim Reichel/Melancholie und Sturmflut/05 Aloha Heja He.m4a"

ControlBar::ControlBar(QWidget *parent) :
    QWidget{parent},
    play_icon_{style()->standardIcon(QStyle::SP_MediaPlay)},
    pause_icon_{style()->standardIcon(QStyle::SP_MediaPause)},
    play_pause_btn_{new QPushButton()},
    sound_slide_{new QSlider(Qt::Horizontal)},
    performer_{new QLabel},
    album_{new QLabel},
    title_{new QLabel}

{
    performer_->setText("Achim Reichel");
    album_->setText("Melancholie und Sturmflut");
    title_->setText("Melancholie und Sturmflut/05 Aloha Heja He");

    play_pause_btn_->setFlat(true);
    play_pause_btn_->setIcon(play_icon_);
    play_pause_btn_->setToolTip(PLAY_TIP);
    play_pause_btn_->setToolTipDuration(DEFAULT_TIP_DURATION);
    connect(play_pause_btn_, &QPushButton::clicked, this, [this] {
        played_ = !played_;
        play_pause_btn_->setIcon(played_ ? pause_icon_ : play_icon_);
        play_pause_btn_->setToolTip(played_ ? PAUSE_TIP : PLAY_TIP);
    });

    auto const audio_btn = new QPushButton(style()->standardIcon(QStyle::SP_MediaVolume), "");
    audio_btn->setFlat(true);
    audio_btn->setDisabled(true);

    auto const skip_backward_btn = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipBackward), "");
    skip_backward_btn->setFlat(true);
    skip_backward_btn->setToolTip("play previous");
    skip_backward_btn->setToolTipDuration(DEFAULT_TIP_DURATION);

    auto const skip_forward_btn = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipForward), "");
    skip_forward_btn->setFlat(true);
    skip_forward_btn->setToolTip("play next");
    skip_forward_btn->setToolTipDuration(DEFAULT_TIP_DURATION);


    sound_slide_->setMinimum(0);
    sound_slide_->setMaximum(100);
    sound_slide_->setToolTip("volume level");
    sound_slide_->setToolTipDuration(DEFAULT_TIP_DURATION);

    auto const play_layout = new QHBoxLayout;
    play_layout->addWidget(skip_backward_btn);
    play_layout->addWidget(play_pause_btn_);
    play_layout->addWidget(skip_forward_btn);

    auto const info_layout = new QHBoxLayout;
    info_layout->addWidget(new QLabel("Performer:"));
    info_layout->addWidget(performer_);
    info_layout->addWidget(new QLabel("Album:"));
    info_layout->addWidget(album_);
    info_layout->addWidget(new QLabel("Title:"));
    info_layout->addWidget(title_);

    auto const layout = new QHBoxLayout;
    layout->addLayout(info_layout);
    layout->addStretch();
    layout->addWidget(audio_btn);
    layout->addWidget(sound_slide_);
    layout->addSpacing(10);
    layout->addLayout(play_layout);
    setLayout(layout);
}
