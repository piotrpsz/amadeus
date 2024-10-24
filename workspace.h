#pragma once

#include <QTabWidget>

class Workspace : public QTabWidget {
    Q_OBJECT
public:
    Workspace(QWidget* = nullptr);
};
