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

SqlRelationalTableDialog::SqlRelationalTableDialog(QWidget *parent) :
    QDialog(parent), m_mapper(new QDataWidgetMapper(this)), m_row(0),
    m_model(0), m_okButton(0)
{
    m_mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
}

void SqlRelationalTableDialog::setupWidgets()
{
    QList<QDialogButtonBox*> buttonBoxes = findChildren<QDialogButtonBox*>();
    Q_ASSERT(!buttonBoxes.isEmpty());
    if (!buttonBoxes.isEmpty())
        m_okButton = buttonBoxes.first()->button(QDialogButtonBox::Ok);

    foreach(QComboBox *comboBox, m_columnComboBox) {
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkWriteReady()));
        connect(comboBox, SIGNAL(editTextChanged(QString)), this, SLOT(checkWriteReady()));
    }

    foreach(QLineEdit *lineEdit, m_columnLineEdit)
        connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkWriteReady()));

    foreach(QTextEdit *textEdit, m_columnTextEdit)
        connect(textEdit, SIGNAL(textChanged()), this, SLOT(checkWriteReady()));

    checkWriteReady();
}

void SqlRelationalTableDialog::accept()
{
    // Manually set the result first so it's propogated to the delegate before closing
    setResult(QDialog::Accepted);
    done(QDialog::Accepted);
}

void SqlRelationalTableDialog::setModelRow(QSqlRelationalTableModel *model, int row) {
    m_row = row;

    Q_ASSERT(model);
    if (!model)
        return;

    m_model = model;
    m_mapper->setModel(model);

    writeToWidgets();

    m_mapper->setCurrentIndex(row);
}

void SqlRelationalTableDialog::writeToModel() {
    bool submitted = m_mapper->submit();
    Q_ASSERT(submitted);
}

void SqlRelationalTableDialog::writeToWidgets() {
    foreach(QComboBox *comboBox, m_columnComboBox)
        writeToComboBox(comboBox);

    foreach(QLineEdit *lineEdit, m_columnLineEdit)
        writeToLineEdit(lineEdit);

    foreach(QTextEdit *textEdit, m_columnTextEdit)
        writeToTextEdit(textEdit);
}

void SqlRelationalTableDialog::writeToComboBox(QComboBox *comboBox) {
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

void SqlRelationalTableDialog::writeToLineEdit(QLineEdit *lineEdit) {
    const int lineEditColumn = m_columnLineEdit.key(lineEdit);
    m_mapper->addMapping(lineEdit, lineEditColumn);
}

void SqlRelationalTableDialog::writeToTextEdit(QTextEdit *textEdit) {
    const int textEditColumn = m_columnTextEdit.key(textEdit);
    m_mapper->addMapping(textEdit, textEditColumn, "plainText");
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
