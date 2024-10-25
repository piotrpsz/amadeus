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
class QMediaPlayer;
class QAudioOutput;

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
    QString song_path_{};
public:
    explicit ControlBar(QWidget *parent = nullptr);

private:
    void set_song(QString const& path) noexcept;
};

