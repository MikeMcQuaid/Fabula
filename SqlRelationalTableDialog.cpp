#include "SqlRelationalTableDialog.h"

#include <QComboBox>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlRelationalDelegate>
#include <QTextEdit>
#include <QDataWidgetMapper>
#include <QDialogButtonBox>
#include <QSqlError>

SqlRelationalTableDialog::SqlRelationalTableDialog(QSqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent), m_mapper(new QDataWidgetMapper(this)), m_model(model), m_okButton(0)
{
    Q_ASSERT(model);

    setWindowModality(Qt::WindowModal);

    m_mapper->setModel(model);
    m_mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

void SqlRelationalTableDialog::setRow(int row)
{
    m_mapper->setCurrentIndex(row);
}

QSqlRelationalTableModel* SqlRelationalTableDialog::model() const
{
    return m_model;
}

void SqlRelationalTableDialog::setupWidgets()
{
    QList<QDialogButtonBox*> buttonBoxes = findChildren<QDialogButtonBox*>();
    Q_ASSERT(!buttonBoxes.isEmpty());
    if (!buttonBoxes.isEmpty())
        m_okButton = buttonBoxes.first()->button(QDialogButtonBox::Ok);

    foreach(QComboBox *comboBox, m_columnComboBox) {
        mapComboBox(comboBox);

        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkWriteReady()));
        connect(comboBox, SIGNAL(editTextChanged(QString)), this, SLOT(checkWriteReady()));
    }

    foreach(QLineEdit *lineEdit, m_columnLineEdit) {
        const int lineEditColumn = m_columnLineEdit.key(lineEdit);
        m_mapper->addMapping(lineEdit, lineEditColumn);

        connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkWriteReady()));
    }

    foreach(QTextEdit *textEdit, m_columnTextEdit) {
        const int textEditColumn = m_columnTextEdit.key(textEdit);
        m_mapper->addMapping(textEdit, textEditColumn, "plainText");

        connect(textEdit, SIGNAL(textChanged()), this, SLOT(checkWriteReady()));
    }

    checkWriteReady();
}

void SqlRelationalTableDialog::accept()
{
    bool submitted = m_mapper->submit();
    Q_UNUSED(submitted);
    Q_ASSERT(submitted);
    QDialog::accept();
}

void SqlRelationalTableDialog::mapComboBox(QComboBox *comboBox) {
    const int comboBoxColumn = m_columnComboBox.key(comboBox);
    // This workaround is needed because QDataWidgetMapper doesn't handle
    // QSqlRelationalTableModels properly yet.
    QSqlTableModel *relationModel = m_model->relationModel(comboBoxColumn);
    const QString &relationColumnName = m_model->relation(comboBoxColumn).displayColumn();
    int relationColumn = relationModel->fieldIndex(relationColumnName);
    comboBox->setModel(relationModel);
    comboBox->setModelColumn(relationColumn);
    m_mapper->addMapping(comboBox, comboBoxColumn);
}

void SqlRelationalTableDialog::checkWriteReady() {
    Q_ASSERT(m_okButton);
    if (!m_okButton)
        return;

    foreach(QComboBox *comboBox, m_columnComboBox) {
        if (comboBox->currentIndex() == -1 || comboBox->currentText().isEmpty()) {
            m_okButton->setEnabled(false);
            return;
        }
    }

    foreach(QLineEdit *lineEdit, m_columnLineEdit) {
        if (lineEdit->text().isEmpty()) {
            m_okButton->setEnabled(false);
            return;
        }
    }

    foreach(QTextEdit *textEdit, m_columnTextEdit) {
        if (textEdit->toPlainText().isEmpty()) {
            m_okButton->setEnabled(false);
            return;
        }
    }

    m_okButton->setEnabled(true);
}
