#pragma once

#include <QString>

class QWidget;

class shared {
public:
    static inline QString const ORGANIZATION{"Piotr Pszczółkowski"};
    static inline QString const DOMAIN{"beesoft.org"};
    static inline QString const VERSION{"0.1.0"};
    static inline QString const PROGRAM{"Amadeus"};
    static inline QString const SUBNAME{"music for you"};


    static void resize(QWidget*, int width, int height);

    static QString app_complete_name() {
        return PROGRAM + " - " + SUBNAME;
    }
};
