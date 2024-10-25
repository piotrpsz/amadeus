#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <QSystemTrayIcon>
#include <QDialog>

/*------- forward declarations:
-------------------------------------------------------------------*/
class QMenu;
class QLabel;
class QAction;
class ControlBar;
class Workspace;


class Window : public QDialog {
    Q_OBJECT
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
    ControlBar* const controlbar_;
    Workspace* const workspace_;
    bool first_time_{true};
public:
    Window();
    void setVisible(bool visible) override;
private:
    void closeEvent(QCloseEvent*) override;
    void showEvent(QShowEvent*) override;
    void show_message();
};
