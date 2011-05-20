#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include "Database.h"

#include <QDebug>
#include <QSqlTableModel>
#include <QSettings>
#include <QSqlError>
#include <QLineEdit>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    setWindowModality(Qt::WindowModal);

    writersModel = new QSqlTableModel(this);
    writersModel->setTable(Database::tableName(Database::Writer));
    writersModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    writersModel->select();
    ui->writerComboBox->setModel(writersModel);
    ui->writerComboBox->setModelColumn(1);

    const QVariant writerVariant = QSettings().value("writer");
    ui->writerNameLabel->setVisible(!writerVariant.isValid());
    if (writerVariant.isValid()) {
        const QString& writer = writerVariant.toString();
        int writerIndex = ui->writerComboBox->findText(writer);
        ui->writerComboBox->setCurrentIndex(writerIndex);
    }
    else
        ui->writerComboBox->lineEdit()->setText(qgetenv("USER"));
}

PreferencesDialog::~PreferencesDialog()
{
    delete writersModel;
    delete ui;
}

void PreferencesDialog::accept()
{
    const QString &writer = ui->writerComboBox->currentText();
    Q_ASSERT(!writer.isEmpty());
    if (writer.isEmpty())
        return;

    bool success = writersModel->submitAll();
    if (!success)
        qWarning() << writersModel->lastError().text();
    Q_ASSERT(success);
    QSettings().setValue("writer", writer);

    QDialog::accept();
}

bool PreferencesDialog::haveWriter()
{
    return QSettings().value("writer").isValid();
}
