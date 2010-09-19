#include "EventDialog.h"
#include "ui_EventDialog.h"

#include <QSqlTableModel>

EventDialog::EventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventDialog)
{
    ui->setupUi(this);
    QSqlTableModel *eventTypesModel = new QSqlTableModel(this);
    eventTypesModel->setTable("event_types");
    eventTypesModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    eventTypesModel->select();
    ui->eventTypeComboBox->setModel(eventTypesModel);
    ui->eventTypeComboBox->setModelColumn(1);

    connect(ui->eventTypeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedEventType(QString)));
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
