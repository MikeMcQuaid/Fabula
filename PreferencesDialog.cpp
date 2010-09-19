#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QSqlTableModel>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    QSqlTableModel *writersModel = new QSqlTableModel(this);
    writersModel->setTable("writers");
    writersModel->select();
    ui->writerComboBox->setModel(writersModel);
    ui->writerComboBox->setModelColumn(1);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}
