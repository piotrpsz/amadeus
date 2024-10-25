#pragma once

#include <QWidget>
#include <QIcon>

class QSlider;
class QPushButton;

class ControlBar : public QWidget {
    Q_OBJECT

    static inline QString const PLAY_TIP{"play"};
    static inline QString const PAUSE_TIP{"pause"};
    static int const DEFAULT_TIP_DURATION{2000};

    QIcon play_icon_;
    QIcon pause_icon_;
    QPushButton* const play_pause_btn_;
    QSlider* const sound_slide_;
    bool played_{};
public:
    explicit ControlBar(QWidget *parent = nullptr);


};
