#ifndef CONTROL_BAR_H
#define CONTROL_BAR_H

#include <QWidget>

class ControlBar : public QWidget {
    Q_OBJECT
public:
    explicit ControlBar(QWidget *parent = nullptr);

signals:
};

#endif // CONTROL_BAR_H
