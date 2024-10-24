
#include <QApplication>
#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include "mainwindow.h"
#include "shared.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setApplicationName(shared::PROGRAM);
    QCoreApplication::setApplicationVersion(shared::VERSION);
    QCoreApplication::setOrganizationName(shared::ORGANIZATION);
    QCoreApplication::setOrganizationDomain(shared::DOMAIN);

    QApplication a(argc, argv);



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
