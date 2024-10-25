#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QWidget>
#include <QIcon>

/*------- forward eclarations:
-------------------------------------------------------------------*/
class QLabel;
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
    QLabel* const performer_;
    QLabel* const album_;
    QLabel* const title_;

    bool played_{};
    QString song_path_{};
public:
    explicit ControlBar(QWidget *parent = nullptr);

private:
    void set_song(QString const& path) noexcept;
};
