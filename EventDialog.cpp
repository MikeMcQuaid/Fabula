#include "EventDialog.h"
#include "ui_EventDialog.h"

#include <QDebug>
#include <QTableView>
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

    // TODO Get the columns from the Database class
    int column = 1;
    setComboBoxModel(ui->typeComboBox, column++);
    setComboBoxModel(ui->conversationComboBox, column++);
    setComboBoxModel(ui->characterComboBox, column++);
    setComboBoxModel(ui->audioFileComboBox, column++, "url");
    ui->textEdit->setText(m_model->data(m_model->index(m_row, column++)).toString());
}

void EventDialog::writeToModel() {
    qDebug() << ui->typeComboBox->currentText();
    qDebug() << ui->conversationComboBox->currentText();
    qDebug() << ui->characterComboBox->currentText();
    qDebug() << ui->audioFileComboBox->currentText();
    qDebug() << ui->textEdit->toPlainText();
    qDebug() << ui->conversationComboBox->model()->submit();
}

void EventDialog::setComboBoxModel(QComboBox *comboBox, int modelColumn, const QString& relationalColumnName) {
    QSqlTableModel *relationModel = m_model->relationModel(modelColumn);
    comboBox->setModel(relationModel);
    comboBox->setModelColumn(relationModel->fieldIndex(relationalColumnName));
}
