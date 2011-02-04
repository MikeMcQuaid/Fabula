#include "CharacterDialog.h"
#include "ui_CharacterDialog.h"

// TODO Get the columns from the Database class
enum CharacterColumn {
    IDColumn = 0,
    NameColumn = 1
};

CharacterDialog::CharacterDialog(QWidget *parent) :
    SqlRelationalTableDialog(parent),
    ui(new Ui::CharacterDialog)
{
    ui->setupUi(this);

    m_columnLineEdit.insert(NameColumn, ui->nameLineEdit);

    setupWidgets();
}

CharacterDialog::~CharacterDialog()
{
    delete ui;
}
