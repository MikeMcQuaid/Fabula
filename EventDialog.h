#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QMap>

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
    void setModelRow(QSqlRelationalTableModel *model, int row);
    void writeToModel();
private slots:
    void changedEventType(const QString &eventType);
private:
    void setupComboBoxModel(QComboBox *comboBox);
    void writeComboBoxModel(QComboBox *comboBox);
    Ui::EventDialog *ui;
    QSqlRelationalTableModel *m_model;
    QMap<int, QComboBox*> m_columnToComboBoxMap;
    QMap<int, QString> m_columnToRelationNameMap;
    int m_row;
};

#endif // EVENTDIALOG_H
