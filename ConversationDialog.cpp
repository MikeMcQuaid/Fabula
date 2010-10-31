#include "ConversationDialog.h"
#include "ui_ConversationDialog.h"

#include <QAbstractItemModel>

// TODO Get the columns from the Database class
enum EventColumn {
    IDColumn = 0,
    TypeColumn = 1,
    WriterColumn = 2,
    NameColumn = 3,
};

ConversationDialog::ConversationDialog(QWidget *parent) :
    SqlRelationalTableDialog(parent),
    ui(new Ui::ConversationDialog)
{
    ui->setupUi(this);

    m_columnComboBox.insert(TypeColumn, ui->typeComboBox);
    m_columnComboBox.insert(WriterColumn, ui->writerComboBox);
    m_columnLineEdit.insert(NameColumn, ui->nameLineEdit);

    setupWidgets();
}

QPushButton* ConversationDialog::okButton()
{
    return ui->buttonBox->button(QDialogButtonBox::Ok);
}

ConversationDialog::~ConversationDialog()
{
    delete ui;
}
