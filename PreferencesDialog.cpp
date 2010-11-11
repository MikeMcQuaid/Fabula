#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include "Database.h"

#include <QDebug>
#include <QSqlTableModel>
#include <QSettings>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    QSqlTableModel *writersModel = new QSqlTableModel(this);
    writersModel->setTable(WritersTable);
    writersModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    writersModel->select();
    ui->writerComboBox->setModel(writersModel);
    ui->writerComboBox->setModelColumn(1);

    load();

    connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::open()
{
    load();
    QDialog::open();
}

void PreferencesDialog::load()
{
    QSettings settings;
    QString writer = settings.value("writer").toString();
    int writerIndex = ui->writerComboBox->findText(writer);
    ui->writerComboBox->setCurrentIndex(writerIndex);
}

void PreferencesDialog::save()
{
    QAbstractItemModel *abstractWritersModel = ui->writerComboBox->model();
    QSqlTableModel *writersModel = qobject_cast<QSqlTableModel*>(abstractWritersModel);
    Q_ASSERT(writersModel);
    if (!writersModel)
        return;

    const QString &writer = ui->writerComboBox->currentText();
    writersModel->submitAll();

    QSettings settings;
    settings.setValue("writer", writer);
}
