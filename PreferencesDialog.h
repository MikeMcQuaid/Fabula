#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

public slots:
    void open();
    void close();
    void load();
    void save();
    bool haveWriter();

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
