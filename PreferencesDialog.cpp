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
    writersModel->setTable(Database::tableName(Database::Writer));
    //writersModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    writersModel->select();
    ui->writerComboBox->setModel(writersModel);
    ui->writerComboBox->setModelColumn(1);

    setWindowModality(Qt::WindowModal);

    load();

    connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

PreferencesDialog::~PreferencesDialog()
{
    save();
    delete ui;
}

void PreferencesDialog::open()
{
    load();
    QDialog::open();
}

void PreferencesDialog::close()
{
    save();
    QDialog::close();
}

void PreferencesDialog::load()
{
    const QVariant writerVariant = QSettings().value("writer");
    ui->writerNameLabel->setVisible(!writerVariant.isValid());
    if (!writerVariant.isValid())
        return;
    const QString& writer = writerVariant.toString();
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
    if (writer.isEmpty())
        return;

    writersModel->submitAll();
    QSettings().setValue("writer", writer);
}

bool PreferencesDialog::haveWriter()
{
    return true;
    //return QSettings().value("writer").isValid();
}
