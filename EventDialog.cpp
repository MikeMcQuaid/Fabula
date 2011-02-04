#include "EventDialog.h"
#include "ui_EventDialog.h"

#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QFileDialog>

// TODO Get the columns from the Database class
enum EventsColumn {
    IDColumn = 0,
    TypeColumn = 1,
    CharacterColumn = 2,
    ConversationColumn = 3,
    AudioFileColumn = 4,
    TextColumn = 5
};

EventDialog::EventDialog(QWidget *parent) :
    SqlRelationalTableDialog(parent), ui(new Ui::EventDialog)
{
    ui->setupUi(this);

    m_columnComboBox.insert(TypeColumn, ui->typeComboBox);
    m_columnComboBox.insert(CharacterColumn, ui->characterComboBox);
    m_columnComboBox.insert(ConversationColumn, ui->conversationComboBox);
    m_columnLineEdit.insert(AudioFileColumn, ui->audioFileLineEdit);
    m_columnTextEdit.insert(TextColumn, ui->textEdit);

    connect(ui->typeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedEventType(QString)));
    connect(ui->audioFilePushButton, SIGNAL(clicked()), this, SLOT(chooseAudioFile()));

    setupWidgets();
}

void EventDialog::changedEventType(const QString &eventType)
{
    // TODO Maybe change the event entry box depending event type?
    ui->eventTypeGroupBox->setTitle(eventType);
}

void EventDialog::chooseAudioFile()
{
    QString audioFile = QFileDialog::getOpenFileName(this);
    if (audioFile.isNull())
        return;

    ui->audioFileLineEdit->setText(audioFile);
}

EventDialog::~EventDialog()
{
    delete ui;
}
