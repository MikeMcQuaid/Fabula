#include "SqlRelationalTableDialog.h"

#include <QComboBox>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlRelationalDelegate>
#include <QTextEdit>
#include <QDataWidgetMapper>
#include <QDialogButtonBox>

SqlRelationalTableDialog::SqlRelationalTableDialog(QWidget *parent) :
    QDialog(parent), m_mapper(new QDataWidgetMapper(this)), m_row(0),
    m_mode(EditMode), m_model(0), m_delegate(new QSqlRelationalDelegate(this)),
    m_okButton(0)
{
    m_mapper->setItemDelegate(m_delegate);
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

void SqlRelationalTableDialog::setModelRow(QSqlRelationalTableModel *model, int row, Mode mode) {
    m_row = row;
    m_mode = mode;

    Q_ASSERT(model);
    if (!model)
        return;

    m_model = model;
    m_mapper->setModel(model);
    m_mapper->setCurrentIndex(row);

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
    if (!delegateComboBox)
        return;
    m_delegate->setEditorData(delegateComboBox, index);

    QAbstractItemModel *abstractModel = delegateComboBox->model();
    QSqlTableModel *model = qobject_cast<QSqlTableModel*>(abstractModel);
    Q_ASSERT(model);
    if (!model)
        return;

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    comboBox->setModel(model);
    comboBox->setModelColumn(delegateComboBox->modelColumn());
    if (m_mode == EditMode)
        comboBox->setCurrentIndex(delegateComboBox->currentIndex());

    delete delegateComboBox;

    //m_mapper->addMapping(comboBox, comboBoxColumn);
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

    if (m_mode == EditMode)
        lineEdit->setText(delegateLineEdit->text());

    delete delegateLineEdit;

    //m_mapper->addMapping(lineEdit, lineEditColumn);
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

    if (m_mode == EditMode)
        textEdit->setText(delegateLineEdit->text());

    delete delegateLineEdit;

    //m_mapper->addMapping(textEdit, textEditColumn);
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
