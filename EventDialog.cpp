#include "EventDialog.h"
#include "ui_EventDialog.h"

#include <QSqlRelationalTableModel>

EventDialog::EventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventDialog)
{
    ui->setupUi(this);
    // TODO: Hmm, is there a way of getting these columns nicely?
    // Check the QSqlRelationItemDelegate perhaps.
    //
    //ui->typeComboBox->setModelColumn(1);
    //ui->conversationComboBox->setModelColumn(2);
    //ui->characterComboBox->setModelColumn(3);
    //ui->audioFileComboBox->setModelColumn(4);

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

void EventDialog::setRow(int row) {
    m_row = row;
}

void EventDialog::setModel(QSqlRelationalTableModel *model) {
    m_model = model;
    ui->typeComboBox->setModel(m_model);
    ui->conversationComboBox->setModel(m_model);
    ui->characterComboBox->setModel(m_model);
    ui->audioFileComboBox->setModel(m_model);
}

void EventDialog::writeToModel() {
    //TODO
}
