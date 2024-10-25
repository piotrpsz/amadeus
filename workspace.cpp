#include "workspace.h"
#include "catalog.h"
#include "play_list.h"
#include <QSlider>

Workspace::Workspace(QWidget* const parent) :
    QTabWidget(parent),
    catalog_{new Catalog},
    play_list_{new PlayList}
{
    setTabPosition(South);
    addTab(catalog_, "Catalog");
    addTab(play_list_, "Play lists");
}
