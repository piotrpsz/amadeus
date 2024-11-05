#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QWidget>
#include <QIcon>

/*------- forward eclarations:
-------------------------------------------------------------------*/
class QLabel;
class QSlider;
class QEvent;
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
    bool one_shot_{};
    QString song_path_{};
    std::vector<std::string> songs_{};
    int idx_ = -1;
    int saved_idx = -1;
public:
    explicit ControlBar(QWidget *parent = nullptr);
    ~ControlBar();
private:
    void play_next() noexcept;
    void play_prev() noexcept;
    void set_song(QString const& path) noexcept;
    void playback_changed() const noexcept;
    void customEvent(QEvent*) override;

    int song_idx(QString const& qpath) const noexcept {
        auto const path =qpath.toStdString();
        for (auto i = 0; i < songs_.size(); ++i)
            if (songs_[i] == path)
                return i;
        return -1;
    }
};

