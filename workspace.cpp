#include "workspace.h"
#include "catalog.h"
#include "playlist_widget.h"
#include <QSlider>

Workspace::Workspace(QWidget* const parent) :
    QTabWidget(parent),
    catalog_{new Catalog},
    playlist_widget_{new PlaylistWidget}
{
    setTabPosition(South);
    addTab(catalog_, "Catalog");
    addTab(playlist_widget_, "Play lists");
}
