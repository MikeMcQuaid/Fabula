#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
    class EventDialog;
}

class QSqlRelationalTableModel;
class QComboBox;

class EventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EventDialog(QWidget *parent = 0);
    ~EventDialog();
    void setModel(QSqlRelationalTableModel *model);
    void setRow(int row);
    void writeToModel();
private slots:
    void changedEventType(const QString &eventType);
private:
    void setComboBoxModel(QComboBox *comboBox, int modelColumn, const QString &relationalColumnName = QString("name"));
    Ui::EventDialog *ui;
    QSqlRelationalTableModel *m_model;
    int m_row;
};

#endif // EVENTDIALOG_H
