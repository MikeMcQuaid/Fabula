#include "EventDialog.h"
#include "ui_EventDialog.h"

#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QFileDialog>
#include <Phonon>

// TODO Get the columns from the Database class
enum EventsColumn {
    IDColumn = 0,
    TypeColumn = 1,
    CharacterColumn = 2,
    ConversationColumn = 3,
    AudioFileColumn = 4,
    TextColumn = 5
};

EventDialog::EventDialog(QSqlRelationalTableModel *model, QWidget *parent) :
    SqlRelationalTableDialog(model, parent), ui(new Ui::EventDialog), audioFilePlayer(0)
{
    ui->setupUi(this);

    m_columnComboBox.insert(TypeColumn, ui->typeComboBox);
    m_columnComboBox.insert(CharacterColumn, ui->characterComboBox);
    m_columnComboBox.insert(ConversationColumn, ui->conversationComboBox);
    m_columnLineEdit.insert(AudioFileColumn, ui->audioFileLineEdit);
    m_columnTextEdit.insert(TextColumn, ui->textEdit);

    connect(ui->typeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changedEventType(QString)));
    connect(ui->audioFileBrowseButton, SIGNAL(clicked()), this, SLOT(chooseAudioFile()));
    connect(ui->audioFilePlayButton, SIGNAL(clicked()), this, SLOT(playAudioFile()));
    connect(ui->audioFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(audioFileChanged(QString)));

    setupWidgets();

    //audioFilePlayer = new Phonon::MediaObject(this);
    //Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    //Phonon::createPath(audioFilePlayer, audioOutput);
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
    if (audioFilePlayer)
        audioFilePlayer->setCurrentSource(audioFile);
}

void EventDialog::audioFileChanged(const QString &audioFile)
{
    if (audioFilePlayer)
        audioFilePlayer->setCurrentSource(audioFile);
}

void EventDialog::playAudioFile()
{
    audioFilePlayer->play();
}

EventDialog::~EventDialog()
{
    delete ui;
}
