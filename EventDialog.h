#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>

namespace Ui {
    class EventDialog;
}

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = 0);
    ~EventDialog();
private slots:
    void changedEventType(const QString &eventType);
private:
    Ui::EventDialog *ui;
};

#endif // EVENTDIALOG_H
