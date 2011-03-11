#ifndef SQLRELATIONALTABLEDIALOG_H
#define SQLRELATIONALTABLEDIALOG_H

#include <QDialog>
#include <QMap>

class QComboBox;
class QDataWidgetMapper;
class QLineEdit;
class QSqlRelationalDelegate;
class QSqlRelationalTableModel;
class QTextEdit;

class SqlRelationalTableDialog : public QDialog
{
    Q_OBJECT
public:
    int row() const;
    QSqlRelationalTableModel* model() const;
public slots:
    void setRow(int row);
    void accept();
protected slots:
    void checkWriteReady();
protected:
    explicit SqlRelationalTableDialog(QSqlRelationalTableModel *model, QWidget *parent = 0);
    void mapComboBox(QComboBox *comboBox);
    void setupWidgets();

    QDataWidgetMapper *m_mapper;
    QSqlRelationalTableModel *m_model;
    QMap<int, QComboBox*> m_columnComboBox;
    QMap<int, QLineEdit*> m_columnLineEdit;
    QMap<int, QTextEdit*> m_columnTextEdit;
    QPushButton *m_okButton;
};

#endif // SQLRELATIONALTABLEDIALOG_H
