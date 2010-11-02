#include "SqlRelationalTableDialog.h"

#include <QComboBox>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlRelationalDelegate>
#include <QTextEdit>

SqlRelationalTableDialog::SqlRelationalTableDialog(QWidget *parent) :
    QDialog(parent), m_row(0), m_model(0),
    m_delegate(new QSqlRelationalDelegate(this))
{
}

void SqlRelationalTableDialog::setupWidgets()
{
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

    writeToWidgets();
}

void SqlRelationalTableDialog::writeToModel() {
    writeFromWidgets();
}


void SqlRelationalTableDialog::writeToWidgets() {
    foreach(QComboBox *comboBox, m_columnComboBox)
        writeToComboBox(comboBox);

    foreach(QLineEdit *lineEdit, m_columnLineEdit)
        writeToLineEdit(lineEdit);

    foreach(QTextEdit *textEdit, m_columnTextEdit)
        writeToTextEdit(textEdit);
}

void SqlRelationalTableDialog::writeFromWidgets() {
    foreach(QComboBox *comboBox, m_columnComboBox)
        writeFromComboBox(comboBox);

    foreach(QLineEdit *lineEdit, m_columnLineEdit)
        writeFromLineEdit(lineEdit);

    foreach(QTextEdit *textEdit, m_columnTextEdit)
        writeFromTextEdit(textEdit);
}

void SqlRelationalTableDialog::writeToComboBox(QComboBox *comboBox) {
    Q_ASSERT(comboBox);
    if (!comboBox)
        return;

    const int comboBoxColumn = m_columnComboBox.key(comboBox);
    QModelIndex index = m_model->index(m_row, comboBoxColumn);
    Q_ASSERT(index.isValid());
    if (!index.isValid())
        return;

    QWidget *delegateWidget = m_delegate->createEditor(this, QStyleOptionViewItem(), index);
    QComboBox *delegateComboBox = qobject_cast<QComboBox*>(delegateWidget);
    Q_ASSERT(delegateComboBox);
    if (!delegateComboBox) {
        qDebug() << delegateWidget->metaObject()->className();
        return;
    }
    m_delegate->setEditorData(delegateComboBox, index);

    QAbstractItemModel *abstractModel = delegateComboBox->model();
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(abstractModel);
    Q_ASSERT(model);
    if (!model)
        return;

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    comboBox->setModel(model);
    comboBox->setModelColumn(delegateComboBox->modelColumn());
    comboBox->setCurrentIndex(delegateComboBox->currentIndex());

    delete delegateComboBox;
}

void SqlRelationalTableDialog::writeToLineEdit(QLineEdit *lineEdit) {
    const int lineEditColumn = m_columnLineEdit.key(lineEdit);
    QModelIndex index = m_model->index(m_row, lineEditColumn);
    if (!index.isValid())
        return;
    QWidget *delegateWidget = m_delegate->createEditor(this, QStyleOptionViewItem(), index);
    QLineEdit *delegateLineEdit = qobject_cast<QLineEdit*>(delegateWidget);
    Q_ASSERT(delegateLineEdit);
    if (!delegateLineEdit)
        return;
    m_delegate->setEditorData(delegateLineEdit, index);

    lineEdit->setText(delegateLineEdit->text());

    delete delegateLineEdit;
}

void SqlRelationalTableDialog::writeToTextEdit(QTextEdit *textEdit) {
    const int textEditColumn = m_columnTextEdit.key(textEdit);
    QModelIndex index = m_model->index(m_row, textEditColumn);
    if (!index.isValid())
        return;
    QWidget *delegateWidget = m_delegate->createEditor(this, QStyleOptionViewItem(), index);
    QLineEdit *delegateLineEdit = qobject_cast<QLineEdit*>(delegateWidget);
    Q_ASSERT(delegateLineEdit);
    if (!delegateLineEdit)
        return;
    m_delegate->setEditorData(delegateLineEdit, index);

    textEdit->setText(delegateLineEdit->text());

    delete delegateLineEdit;
}

void SqlRelationalTableDialog::writeFromComboBox(QComboBox *comboBox) {
    Q_ASSERT(comboBox);
    if (!comboBox)
        return;

    QSqlTableModel *relationModel = qobject_cast<QSqlTableModel*>(comboBox->model());
    Q_ASSERT(relationModel);
    if (!relationModel)
        return;

    // Need to get text before writing new items as the combobox
    // index can be reset afterwards.
    const QString &comboBoxText = comboBox->currentText();
    relationModel->submitAll();
    const int comboBoxRow = comboBox->findText(comboBoxText);
    comboBox->setCurrentIndex(comboBoxRow);

    // A hacky but successful way to clear the model's relation cache.
    const QSqlTableModel::EditStrategy editStrategy = m_model->editStrategy();
    Q_ASSERT(editStrategy != QSqlTableModel::OnManualSubmit);
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->submitAll();
    m_model->setEditStrategy(editStrategy);

    const int comboBoxColumn = m_columnComboBox.key(comboBox);
    const QModelIndex index = m_model->index(m_row, comboBoxColumn);
    m_delegate->setModelData(comboBox, m_model, index);
}

void SqlRelationalTableDialog::writeFromLineEdit(QLineEdit *lineEdit) {
    const int lineEditColumn = m_columnLineEdit.key(lineEdit);
    const QModelIndex index = m_model->index(m_row, lineEditColumn);
    m_delegate->setModelData(lineEdit, m_model, index);
}

void SqlRelationalTableDialog::writeFromTextEdit(QTextEdit *textEdit) {
    QLineEdit *lineEdit = new QLineEdit(textEdit->toPlainText(), this);
    const int textEditColumn = m_columnTextEdit.key(textEdit);
    const QModelIndex index = m_model->index(m_row, textEditColumn);
    m_delegate->setModelData(lineEdit, m_model, index);
    delete lineEdit;
}

void SqlRelationalTableDialog::checkWriteReady() {
    QPushButton *ok = okButton();
    Q_ASSERT(ok);
    if (!ok)
        return;

    foreach(QComboBox *comboBox, m_columnComboBox) {
        if (comboBox->currentIndex() == -1 || comboBox->currentText().isEmpty()) {
            ok->setEnabled(false);
            return;
        }
    }

    foreach(QLineEdit *lineEdit, m_columnLineEdit) {
        if (lineEdit->text().isEmpty()) {
            ok->setEnabled(false);
            return;
        }
    }

    foreach(QTextEdit *textEdit, m_columnTextEdit) {
        if (textEdit->toPlainText().isEmpty()) {
            ok->setEnabled(false);
            return;
        }
    }

    ok->setEnabled(true);
}
