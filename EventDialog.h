#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QMap>

#include "SqlRelationalTableDialog.h"

namespace Ui {
    class EventDialog;
}

class QAbstractItemModel;
class QComboBox;
class QSqlRelationalDelegate;
class QSqlRelationalTableModel;
class QTextEdit;

class EventDialog : public SqlRelationalTableDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = 0);
    ~EventDialog();
    void setModelRow(QAbstractItemModel *model, int row);
    void writeToModel();
public slots:
    virtual void accept();
private slots:
    void changedEventType(const QString &eventType);
    void checkWriteReady();
private:
    void setupComboBox(QComboBox *comboBox);
    void writeComboBox(QComboBox *comboBox);
    void setupTextEdit(QTextEdit *textEdit);
    void writeTextEdit(QTextEdit *textEdit);

    Ui::EventDialog *ui;
    QSqlRelationalTableModel *m_model;
    QMap<int, QComboBox*> m_columnToComboBoxMap;
    int m_row;
    int m_result;
    QSqlRelationalDelegate *m_delegate;
};

#endif // EVENTDIALOG_H
