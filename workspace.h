#pragma once

#include <QTabWidget>

class Catalog;
class PlayList;

class Workspace : public QTabWidget {
    Q_OBJECT
    Catalog* const catalog_;
    PlayList* const play_list_;
public:
    Workspace(QWidget* = nullptr);
};
