#include "ConversationDialog.h"
#include "ui_ConversationDialog.h"

#include <QAbstractItemModel>

// TODO Get the columns from the Database class
enum ConversationsColumn {
    IDColumn = 0,
    TypeColumn = 1,
    WriterColumn = 2,
    NameColumn = 3,
};

ConversationDialog::ConversationDialog(QSqlRelationalTableModel *model, QWidget *parent) :
    SqlRelationalTableDialog(model, parent),
    ui(new Ui::ConversationDialog)
{
    ui->setupUi(this);

    m_columnComboBox.insert(TypeColumn, ui->typeComboBox);
    m_columnComboBox.insert(WriterColumn, ui->writerComboBox);
    m_columnLineEdit.insert(NameColumn, ui->nameLineEdit);

    setupWidgets();
}

ConversationDialog::~ConversationDialog()
{
    delete ui;
}
