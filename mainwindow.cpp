#include "mainwindow.h"
#include "shared.h"
#include <QUrl>
#include <QIcon>
#include <QShowEvent>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QApplication>
#include <QSystemTrayIcon>
#include <fmt/core.h>


#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>

// to jest do usunięcia
// Kraken token.
// eJyrVkpMTk4tLg7Jz07NU7JSMrIwTjUyNUsxMEhNTjUzN0k0TzNJSku2MDdOS7M0N0xKTktMTEo2T1OqBQAZ2hIa

Window::Window(): QDialog(),
    player_{new QMediaPlayer},
    audio_output_{new QAudioOutput},
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

    player_->setAudioOutput(audio_output_);

    connect(player_, &QMediaPlayer::positionChanged, [](auto pos) {
        //fmt::print(stderr, "{}\n", pos);
    });

    connect(minimize_action_, &QAction::triggered, this, &QWidget::hide);
    connect(maximize_action_, &QAction::triggered, this, &QWidget::showMaximized);
    connect(restore_action_ , &QAction::triggered, this, &QWidget::showNormal);
    connect(quit_action_    , &QAction::triggered, qApp, &QCoreApplication::quit);

    connect(msg_action_     , &QAction::triggered, this, [this] (auto _) {
        show_message();
    });
    connect(play_action_    , &QAction::triggered, player_, [this] (auto _) {
        player_->play();
    });
    connect(pause_action_    , &QAction::triggered, this, [this] (auto _) {
        player_->pause();
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

    // connect(tray_, &QSystemTrayIcon::messageClicked, this, &Window::messageClicked);
    connect(tray_, &QSystemTrayIcon::activated, this, [this] (auto _) {
        isVisible() ? hide() : show();
    });

    QVBoxLayout *mainLayout = new QVBoxLayout;
    // mainLayout->addWidget(iconGroupBox);
    // mainLayout->addWidget(messageGroupBox);
    setLayout(mainLayout);

    // iconComboBox->setCurrentIndex(1);
    tray_->show();


    // resize(400, 300);
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

// void Window::iconActivated(QSystemTrayIcon::ActivationReason reason) {
//     switch (reason) {
//     case QSystemTrayIcon::Trigger:
//     case QSystemTrayIcon::DoubleClick:
//         // iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1) % iconComboBox->count());
//         break;
//     case QSystemTrayIcon::MiddleClick:
//         show_message();
//         break;
//     default:

//         ;
//     }
// }

void Window::show_message() {
    tray_->showMessage("Tytul", player_->source().path(), app_icon_, 15 * 1000);
}

// void Window::messageClicked()
// {
//     QMessageBox::information(nullptr, tr("Systray"),
//                              tr("Sorry, I already gave what help I could.\n"
//                                 "Maybe you should try asking a human?"));
// }

// void Window::createIconGroupBox() {
//     iconGroupBox = new QGroupBox(tr("Tray Icon"));

//     iconLabel = new QLabel("Icon:");

//     iconComboBox = new QComboBox;
//     iconComboBox->addItem(QIcon(":/images/bad.png"), tr("Bad"));
//     iconComboBox->addItem(QIcon(":/images/heart.png"), tr("Heart"));
//     iconComboBox->addItem(QIcon(":/images/trash.png"), tr("Trash"));

//     showIconCheckBox = new QCheckBox(tr("Show icon"));
//     showIconCheckBox->setChecked(true);

//     QHBoxLayout *iconLayout = new QHBoxLayout;
//     iconLayout->addWidget(iconLabel);
//     iconLayout->addWidget(iconComboBox);
//     iconLayout->addStretch();
//     iconLayout->addWidget(showIconCheckBox);
//     iconGroupBox->setLayout(iconLayout);
// }

// void Window::createMessageGroupBox() {
    // messageGroupBox = new QGroupBox(tr("Balloon Message"));

    // typeLabel = new QLabel(tr("Type:"));

    // typeComboBox = new QComboBox;

    // typeComboBox->addItem(tr("None"), QSystemTrayIcon::NoIcon);
    // typeComboBox->addItem(style()->standardIcon(QStyle::SP_MessageBoxInformation), tr("Information"), QSystemTrayIcon::Information);
    // typeComboBox->addItem(style()->standardIcon(QStyle::SP_MessageBoxWarning), tr("Warning"), QSystemTrayIcon::Warning);
    // typeComboBox->addItem(style()->standardIcon(QStyle::SP_MessageBoxCritical), tr("Critical"), QSystemTrayIcon::Critical);
    // typeComboBox->addItem(QIcon(), tr("Custom icon"), -1);
    // typeComboBox->setCurrentIndex(1);

    // durationLabel = new QLabel(tr("Duration:"));

    // durationSpinBox = new QSpinBox;
    // durationSpinBox->setRange(5, 60);
    // durationSpinBox->setSuffix(" s");
    // durationSpinBox->setValue(15);

    // durationWarningLabel = new QLabel(tr("(some systems might ignore this hint)"));
    // durationWarningLabel->setIndent(10);

    // titleLabel = new QLabel(tr("Title:"));

    // titleEdit = new QLineEdit(tr("Cannot connect to network"));

    // bodyLabel = new QLabel(tr("Body:"));

    // bodyEdit = new QTextEdit;
    // bodyEdit->setPlainText(tr("Don't believe me. Honestly, I don't have a "
    //                           "clue.\nClick this balloon for details."));

    // showMessageButton = new QPushButton(tr("Show Message"));
    // showMessageButton->setDefault(true);

    // QGridLayout *messageLayout = new QGridLayout;
    // messageLayout->addWidget(typeLabel, 0, 0);
    // messageLayout->addWidget(typeComboBox, 0, 1, 1, 2);
    // messageLayout->addWidget(durationLabel, 1, 0);
    // messageLayout->addWidget(durationSpinBox, 1, 1);
    // messageLayout->addWidget(durationWarningLabel, 1, 2, 1, 3);
    // messageLayout->addWidget(titleLabel, 2, 0);
    // messageLayout->addWidget(titleEdit, 2, 1, 1, 4);
    // messageLayout->addWidget(bodyLabel, 3, 0);
    // messageLayout->addWidget(bodyEdit, 3, 1, 2, 4);
    // messageLayout->addWidget(showMessageButton, 5, 4);
    // messageLayout->setColumnStretch(3, 1);
    // messageLayout->setRowStretch(4, 1);
    // messageGroupBox->setLayout(messageLayout);
// }

/*
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , player_{new QMediaPlayer}
    , audio_output_{new QAudioOutput}
{

    auto stat = QSystemTrayIcon::isSystemTrayAvailable();
    fmt::print("systray available: {}\n", stat);

    auto icon = QIcon(":/img/speaker");
    QApplication::setWindowIcon(icon);

    player_->setAudioOutput(audio_output_);
    connect(player_, &QMediaPlayer::positionChanged, [](auto pos) {
        // fmt::print(stderr, "{}\n", pos);
    });
}

MainWindow::~MainWindow() {}
*/

void Window::showEvent(QShowEvent* e) {
    QString path = "/home/piotr/Music/Achim Reichel/Melancholie und Sturmflut/05 Aloha Heja He.m4a";
    player_->setSource(QUrl::fromLocalFile(path));
    audio_output_->setVolume(30);
}
