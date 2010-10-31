#ifndef SQLRELATIONALTABLEDIALOG_H
#define SQLRELATIONALTABLEDIALOG_H

#include <QDialog>

class QAbstractItemModel;

class SqlRelationalTableDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SqlRelationalTableDialog(QWidget *parent = 0);
    virtual void writeToModel() = 0;
    virtual void setModelRow(QAbstractItemModel *model, int row) = 0;
};

#endif // SQLRELATIONALTABLEDIALOG_H
