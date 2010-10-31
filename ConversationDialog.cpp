#include "ConversationDialog.h"
#include "ui_ConversationDialog.h"

#include <QAbstractItemModel>

ConversationDialog::ConversationDialog(QWidget *parent) :
    SqlRelationalTableDialog(parent),
    ui(new Ui::ConversationDialog)
{
    ui->setupUi(this);
}

ConversationDialog::~ConversationDialog()
{
    delete ui;
}

void ConversationDialog::writeToModel()
{

}

void ConversationDialog::setModelRow(QAbstractItemModel *model, int row)
{
    Q_UNUSED(model);
    Q_UNUSED(row);
}
