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
    explicit ConversationDialog(QSqlRelationalTableModel *model, QWidget *parent = 0);
    ~ConversationDialog();

private:
    Ui::ConversationDialog *ui;
};

#endif // CONVERSATIONDIALOG_H
