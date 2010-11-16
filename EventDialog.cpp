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
    SqlRelationalTableDialog(parent), ui(new Ui::EventDialog)
{
    ui->setupUi(this);

    m_columnComboBox.insert(TypeColumn, ui->typeComboBox);
    m_columnComboBox.insert(ConversationColumn, ui->conversationComboBox);
    m_columnComboBox.insert(CharacterColumn, ui->characterComboBox);
    m_columnLineEdit.insert(AudioFileColumn, ui->audioFileLineEdit);
    m_columnTextEdit.insert(TextColumn, ui->textEdit);

    connect(ui->typeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedEventType(QString)));

    setupWidgets();
}

void EventDialog::changedEventType(const QString &eventType)
{
    // TODO Maybe change the event entry box depending event type?
    ui->eventTypeGroupBox->setTitle(eventType);
}

QPushButton* EventDialog::okButton()
{
    return ui->buttonBox->button(QDialogButtonBox::Ok);
}

EventDialog::~EventDialog()
{
    delete ui;
}
