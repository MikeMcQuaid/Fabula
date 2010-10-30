#include "EventDialog.h"
#include "ui_EventDialog.h"

#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QTableView>

// TODO Get the columns from the Database class
enum EventColumn {
    IDColumn = 0,
    TypeColumn = 1,
    ConversationColumn = 2,
    CharacterColumn = 3,
    AudioFileColumn = 4,
    TextColumn = 5
};

EventDialog::EventDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::EventDialog), m_row(0), m_result(0), m_delegate(new QSqlRelationalDelegate(this))
{
    ui->setupUi(this);

    m_columnToComboBoxMap.insert(TypeColumn, ui->typeComboBox);
    m_columnToComboBoxMap.insert(ConversationColumn, ui->conversationComboBox);
    m_columnToComboBoxMap.insert(CharacterColumn, ui->characterComboBox);
    m_columnToComboBoxMap.insert(AudioFileColumn, ui->audioFileComboBox);

    connect(ui->typeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedEventType(QString)));

    foreach(QComboBox *comboBox, m_columnToComboBoxMap) {
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkWriteReady()));
        connect(comboBox, SIGNAL(editTextChanged(QString)), this, SLOT(checkWriteReady()));
    }
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(checkWriteReady()));

    checkWriteReady();
}

void EventDialog::changedEventType(const QString &eventType)
{
    // TODO Maybe change the event entry box depending event type?
    ui->eventTypeGroupBox->setTitle(eventType);
}

void EventDialog::accept()
{
    // Manually set the result first so it's propogated to the delegate before closing
    setResult(QDialog::Accepted);
    done(QDialog::Accepted);
}

EventDialog::~EventDialog()
{
    delete ui;
}

void EventDialog::setModelRow(QSqlRelationalTableModel *model, int row) {
    m_row = row;
    m_model = model;

    foreach(QComboBox *comboBox, m_columnToComboBoxMap)
        setupComboBox(comboBox);

    setupTextEdit(ui->textEdit);
}

void EventDialog::writeToModel() {
    foreach(QComboBox *comboBox, m_columnToComboBoxMap)
        writeComboBox(comboBox);

    writeTextEdit(ui->textEdit);
}

void EventDialog::setupComboBox(QComboBox *comboBox) {
    Q_ASSERT(comboBox);
    if (!comboBox)
        return;

    const int comboBoxColumn = m_columnToComboBoxMap.key(comboBox);
    const QModelIndex index = m_model->index(m_row, comboBoxColumn);
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
    comboBox->setCurrentIndex(delegateComboBox->currentIndex());

    delete delegateComboBox;
}

void EventDialog::setupTextEdit(QTextEdit *textEdit) {
    const QModelIndex index = m_model->index(m_row, TextColumn);
    QWidget *delegateWidget = m_delegate->createEditor(this, QStyleOptionViewItem(), index);
    QLineEdit *delegateLineEdit = qobject_cast<QLineEdit*>(delegateWidget);
    Q_ASSERT(delegateLineEdit);
    if (!delegateLineEdit)
        return;
    m_delegate->setEditorData(delegateLineEdit, index);

    textEdit->setText(delegateLineEdit->text());

    delete delegateLineEdit;
}

void EventDialog::writeComboBox(QComboBox *comboBox) {
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

    const int comboBoxColumn = m_columnToComboBoxMap.key(comboBox);
    const QModelIndex index = m_model->index(m_row, comboBoxColumn);
    m_delegate->setModelData(comboBox, m_model, index);
}

void EventDialog::writeTextEdit(QTextEdit *textEdit) {
    QLineEdit *lineEdit = new QLineEdit(textEdit->toPlainText(), this);
    const QModelIndex index = m_model->index(m_row, TextColumn);
    m_delegate->setModelData(lineEdit, m_model, index);
    delete lineEdit;
}

void EventDialog::checkWriteReady() {
    bool isReadyToWrite = true;

    foreach(QComboBox *comboBox, m_columnToComboBoxMap)
        if (comboBox->currentIndex() == -1 || comboBox->currentText().isEmpty()) {
            isReadyToWrite = false;
            break;
        }

    if (isReadyToWrite && ui->textEdit->toPlainText().isEmpty())
        isReadyToWrite = false;

    QPushButton *ok = ui->buttonBox->button(QDialogButtonBox::Ok);
    Q_ASSERT(ok);
    if (ok)
        ok->setEnabled(isReadyToWrite);
}
