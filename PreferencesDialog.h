#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class QSqlTableModel;

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();
    static bool haveWriter();

public slots:
    void accept();

private:
    Ui::PreferencesDialog *ui;
    QSqlTableModel *writersModel;
};

#endif // PREFERENCESDIALOG_H
