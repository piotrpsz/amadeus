#include "mainwindow.h"
#include "control_bar.h"
#include "workspace.h"
#include "shared.h"
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QShowEvent>
#include <QCloseEvent>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QSystemTrayIcon>
#include <fmt/core.h>

// Kraken token.
// eJyrVkpMTk4tLg7Jz07NU7JSMrIwTjUyNUsxMEhNTjUzN0k0TzNJSku2MDdOS7M0N0xKTktMTEo2T1OqBQAZ2hIa

Window::Window(): QDialog(),
    controlbar_{new ControlBar},
    workspace_{new Workspace},
    tray_{new QSystemTrayIcon(this)},
    tray_menu_{new QMenu},
    app_icon_{":/img/speaker"},
    minimize_action_{new QAction("Minimize")},
    maximize_action_{new QAction("Maximize")},
    restore_action_{new QAction{"Restore"}},
    quit_action_{new QAction("Quit")},
    msg_action_{new QAction("Message")},
    play_action_{new QAction("Play")},
    pause_action_{new QAction("Pause")}
{
    setWindowTitle(shared::app_complete_name());

    connect(minimize_action_, &QAction::triggered, this, &QWidget::hide);
    connect(maximize_action_, &QAction::triggered, this, &QWidget::showMaximized);
    connect(restore_action_ , &QAction::triggered, this, &QWidget::showNormal);
    connect(quit_action_    , &QAction::triggered, qApp, &QCoreApplication::quit);

    connect(msg_action_     , &QAction::triggered, this, [this] (auto _) {
        show_message();
    });

    // Create menu
    tray_menu_->addAction(minimize_action_);
    tray_menu_->addAction(maximize_action_);
    tray_menu_->addAction(restore_action_);
    tray_menu_->addSeparator();
    tray_menu_->addAction(play_action_);
    tray_menu_->addAction(pause_action_);
    tray_menu_->addAction(msg_action_);
    tray_menu_->addSeparator();
    tray_menu_->addAction(quit_action_);
    tray_->setContextMenu(tray_menu_);

    // Set app the same icon and for tray and for window.
    tray_->setIcon(app_icon_);
    tray_->setToolTip("Amadeus");
    setWindowIcon(app_icon_);


    // When user click tray icon with left mouse button then show/hide main window.
    connect(tray_, &QSystemTrayIcon::activated, this, [this] (auto _) {
        isVisible() ? hide() : show();
    });

    // layout of window
    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addWidget(controlbar_);
    main_layout->addWidget(workspace_);
    setLayout(main_layout);

    tray_->show();
    shared::resize(this, 75, 75);
}

void Window::setVisible(bool const visible) {
    minimize_action_->setEnabled(visible);
    maximize_action_->setEnabled(!isMaximized());
    restore_action_->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

/// We ignore close event.
/// User should explicity close app from tray-menu.
void Window::closeEvent(QCloseEvent* const event) {
    if (!event->spontaneous() || !isVisible())
        return;

    if (tray_->isVisible()) {
        if (first_time_) {
            // this message is displayed only one time (when starting the app)
            first_time_ = false;
            QMessageBox::information(this, shared::PROGRAM,
                                "The program will keep running in the "
                                "system tray. To terminate the program, "
                                "choose <b>Quit</b> in the context menu "
                                "of the system tray entry.");
        }
        hide();
        event->ignore();
    }
}

void Window::show_message() {
    // tray_->showMessage("Tytul", player_->source().path(), app_icon_, 15 * 1000);
}

void Window::showEvent(QShowEvent* e) {
}
