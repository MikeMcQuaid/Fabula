#ifndef CHARACTERDIALOG_H
#define CHARACTERDIALOG_H

#include "SqlRelationalTableDialog.h"

namespace Ui {
    class CharacterDialog;
}

class CharacterDialog : public SqlRelationalTableDialog
{
    Q_OBJECT

public:
    explicit CharacterDialog(QSqlRelationalTableModel *model, QWidget *parent = 0);
    ~CharacterDialog();

private:
    Ui::CharacterDialog *ui;
};

#endif // CHARACTERDIALOG_H
