#include "line_text_edit.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

LineTextEdit::LineTextEdit(QString const& prompt_text, QString previous_value, QWidget* const parent)
    : QDialog{parent}
    , editor_{new QLineEdit}
{
    auto prompt = new QLabel(prompt_text + ": ");
    if (!previous_value.isEmpty())
        editor_->setText(previous_value);

    auto const fields = new QHBoxLayout;
    fields->setContentsMargins(5, 10, 5, 10);
    fields->addWidget(prompt);
    fields->addWidget(editor_);

    auto const accept_btn = new QPushButton("Accept");
    auto const reject_btn = new QPushButton("Reject");

    auto const buttons = new QHBoxLayout;
    buttons->setContentsMargins(5, 10, 5, 10);
    buttons->addWidget(reject_btn);
    buttons->addStretch();
    buttons->addWidget(accept_btn);

    auto const main = new QVBoxLayout;
    main->setContentsMargins(0, 0, 0, 0);
    main->addLayout(fields);
    main->addLayout(buttons);
    setLayout(main);
}
