#pragma once

#include <QDialog>
#include <QString>

class QLineEdit;

class LineTextEdit : public QDialog {
    Q_OBJECT
    QLineEdit* const editor_;
public:
    LineTextEdit(QString const& prompt, QString const previous_value = "", QWidget* = nullptr);
    QString value() noexcept;
};
