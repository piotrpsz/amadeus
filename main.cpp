#include "sqlite/sqlite.h"
#include <QApplication>
#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <fmt/core.h>
#include "mainwindow.h"
#include "model/playlist.h"
#include "model/songs.h"
#include "tool.h"

bool create_commands(SQLite const& db) {
    // return Playlist::create_table();
    return Playlist::create_table() && Song::create_table();
}

bool open_or_create_database() {
    auto const database_dir = tool::home_dir() + '/' + ".beesoft";
    if (tool::create_dirs(database_dir)) {
        auto const database_path = database_dir + '/' + "amadeus.sqlite";

        // Try to open database.
        // if (SQLite::instance().open(database_path))
        //     return true;

        return SQLite::instance().create(database_path, [] (SQLite const& db) {
            return create_commands(db);
        }, true);
    }
    return {};
}

int main(int argc, char *argv[]) {
    QCoreApplication::setApplicationName(tool::PROGRAM);
    QCoreApplication::setApplicationVersion(tool::VERSION);
    QCoreApplication::setOrganizationName(tool::ORGANIZATION);
    QCoreApplication::setOrganizationDomain(tool::DOMAIN);

    QApplication a(argc, argv);

    if (!open_or_create_database()) {
        fmt::print(stderr, "Can't open/create database filr\n");
        return -1;
    }

    // a.setApplicationName("Amadeus");

    // QTranslator translator;
    // const QStringList uiLanguages = QLocale::system().uiLanguages();
    // for (const QString &locale : uiLanguages) {
    //     const QString baseName = "amadeus_" + QLocale(locale).name();
    //     if (translator.load(":/i18n/" + baseName)) {
    //         a.installTranslator(&translator);
    //         break;
    //     }
    // }
    Window w;
    w.show();
    return a.exec();
}
