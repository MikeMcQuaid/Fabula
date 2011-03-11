#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QMap>

#include "SqlRelationalTableDialog.h"

namespace Ui {
    class EventDialog;
}

namespace Phonon {
    class MediaObject;
}

class EventDialog : public SqlRelationalTableDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QSqlRelationalTableModel *model, QWidget *parent = 0);
    ~EventDialog();

private slots:
    void changedEventType(const QString &eventType);
    void chooseAudioFile();
    void playAudioFile();
    void audioFileChanged(const QString &audioFile);

private:
    Ui::EventDialog *ui;
    Phonon::MediaObject *audioFilePlayer;
};

#endif // EVENTDIALOG_H
