#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QMap>

#include "SqlRelationalTableDialog.h"

class QPsuhButton;

namespace Ui {
    class EventDialog;
}

class EventDialog : public SqlRelationalTableDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = 0);
    ~EventDialog();

private slots:
    void changedEventType(const QString &eventType);
    void chooseAudioFile();

private:
    Ui::EventDialog *ui;
};

#endif // EVENTDIALOG_H
