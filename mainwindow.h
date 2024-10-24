#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QSystemTrayIcon>
#include <QDialog>
// #include <QMainWindow>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QMediaPlayer;
class QAudioOutput;


class Window : public QDialog {
    Q_OBJECT
    QMediaPlayer* const player_;
    QAudioOutput* const audio_output_;
    QSystemTrayIcon* const tray_;
    QMenu* const tray_menu_;
    QIcon const app_icon_;
    QAction* const minimize_action_;
    QAction* const maximize_action_;
    QAction* const restore_action_;
    QAction* const quit_action_;
    QAction* const msg_action_;
    QAction* const play_action_;
    QAction* const pause_action_;
    // QAction* const stop_action_;
    bool first_time_{true};
public:
    Window();
    void setVisible(bool visible) override;
protected:
    void closeEvent(QCloseEvent*) override;
    void showEvent(QShowEvent*) override;
private:
    // void setIcon(int index);
    // void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void show_message();

private:
    // void createIconGroupBox();
    // void createMessageGroupBox();
    // void createActions();


    // QGroupBox *iconGroupBox;
    // QLabel *iconLabel;
    // QComboBox *iconComboBox;
    // QCheckBox *showIconCheckBox;

    // QGroupBox *messageGroupBox;
    // QLabel *typeLabel;
    // QLabel *durationLabel;
    // QLabel *durationWarningLabel;
    // QLabel *titleLabel;
    // QLabel *bodyLabel;
    // QComboBox *typeComboBox;
    // QSpinBox *durationSpinBox;
    // QLineEdit *titleEdit;
    // QTextEdit *bodyEdit;
    // QPushButton *showMessageButton;



};

/*
class MainWindow : public QMainWindow {
    Q_OBJECT

    QMediaPlayer* const player_;
    QAudioOutput* const audio_output_;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void showEvent(QShowEvent*) override;
};
*/
