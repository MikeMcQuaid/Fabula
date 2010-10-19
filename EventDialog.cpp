#include "EventDialog.h"
#include "ui_EventDialog.h"

#include <QDebug>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QSqlError>

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
    QDialog(parent),
    ui(new Ui::EventDialog)
{
    ui->setupUi(this);

    m_columnToComboBoxMap.insert(TypeColumn, ui->typeComboBox);
    m_columnToComboBoxMap.insert(ConversationColumn, ui->conversationComboBox);
    m_columnToComboBoxMap.insert(CharacterColumn, ui->characterComboBox);
    m_columnToComboBoxMap.insert(AudioFileColumn, ui->audioFileComboBox);

    m_columnToRelationNameMap.insert(TypeColumn, "name");
    m_columnToRelationNameMap.insert(ConversationColumn, "name");
    m_columnToRelationNameMap.insert(CharacterColumn, "name");
    m_columnToRelationNameMap.insert(AudioFileColumn, "url");

    connect(ui->typeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedEventType(QString)));
}

void EventDialog::changedEventType(const QString &eventType)
{
    // TODO Change the event entry box depending event type
    ui->eventTypeGroupBox->setTitle(eventType);
}

EventDialog::~EventDialog()
{
    delete ui;
}

void EventDialog::setModelRow(QSqlRelationalTableModel *model, int row) {
    m_row = row;
    m_model = model;

    foreach(QComboBox *comboBox, m_columnToComboBoxMap)
        setupComboBoxModel(comboBox);

    ui->textEdit->setText(m_model->data(m_model->index(m_row, TextColumn)).toString());
}

void EventDialog::writeToModel() {
    foreach(QComboBox *comboBox, m_columnToComboBoxMap)
        writeComboBoxModel(comboBox);

    const QModelIndex textIndex = m_model->index(m_row, TextColumn);
    const bool wroteData = m_model->setData(textIndex, ui->textEdit->toPlainText());
    Q_ASSERT(wroteData);
    m_model->select();
}

void EventDialog::setupComboBoxModel(QComboBox *comboBox) {
    Q_ASSERT(comboBox);
    if (!comboBox)
        return;

    const int comboBoxColumn = m_columnToComboBoxMap.key(comboBox);
    QSqlTableModel *relationModel = m_model->relationModel(comboBoxColumn);
    Q_ASSERT(relationModel);
    if (!relationModel)
        return;

    relationModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    comboBox->setModel(relationModel);
    const QString &relationName = m_columnToRelationNameMap.value(comboBoxColumn);
    comboBox->setModelColumn(relationModel->fieldIndex(relationName));
    const QModelIndex modelIndex = m_model->index(m_row, comboBoxColumn);
    const QString &currentValue = m_model->data(modelIndex).toString();
    const int currentIndex = comboBox->findText(currentValue);
    comboBox->setCurrentIndex(currentIndex);
}

void EventDialog::writeComboBoxModel(QComboBox *comboBox) {
    Q_ASSERT(comboBox);
    if (!comboBox)
        return;

    // Write any new items in the combobox
    QSqlTableModel *relationModel = qobject_cast<QSqlTableModel*>(comboBox->model());
    Q_ASSERT(relationModel);
    if (!relationModel)
        return;

    // Get the column index before it's reset by the submitAll
    const QModelIndex relationIdIndex = relationModel->index(comboBox->currentIndex(), 0);
    const int relationId = relationModel->data(relationIdIndex).toInt();

    relationModel->submitAll();

    // Set the combobox value in the main model
    const int comboBoxColumn = m_columnToComboBoxMap.key(comboBox);
    const QModelIndex modelIndex = m_model->index(m_row, comboBoxColumn);
    const bool wroteData = m_model->setData(modelIndex, relationId);
    Q_ASSERT(wroteData);
}
