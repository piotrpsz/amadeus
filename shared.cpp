#include "shared.h"
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QSize>

/*------- local constants:
-------------------------------------------------------------------*/


void shared::resize(QWidget* const widget, int const width, int const height) {
    auto const screen = widget->window()->screen();
    auto const screen_width = screen->availableSize().rwidth();
    auto const screen_height = screen->availableSize().rheight();
    auto const width_percent = width/100.;
    auto const height_percent = height/100.;
    auto const widget_width = screen_width * width_percent;
    auto const widget_height = screen_height * height_percent;
    auto const dx = (screen_width - widget_width ) / 2;
    auto const dy = (screen_height - widget_height) / 2;

    widget->resize(QSize(widget_width, widget_height));
    widget->move(QPoint(dx, dy));
}
