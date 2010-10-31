#ifndef CONVERSATIONDIALOG_H
#define CONVERSATIONDIALOG_H

#include "SqlRelationalTableDialog.h"

namespace Ui {
    class ConversationDialog;
}

class ConversationDialog : public SqlRelationalTableDialog
{
    Q_OBJECT

public:
    explicit ConversationDialog(QWidget *parent = 0);
    ~ConversationDialog();

private:
    QPushButton* okButton();
    Ui::ConversationDialog *ui;
};

#endif // CONVERSATIONDIALOG_H
