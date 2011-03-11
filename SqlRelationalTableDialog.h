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
    void writeToModel();

    enum Mode {
        NewMode,
        EditMode
    };

    void setModelRow(QSqlRelationalTableModel *model, int row);
public slots:
    void accept();
protected slots:
    void checkWriteReady();
protected:
    explicit SqlRelationalTableDialog(QWidget *parent = 0);

    void writeToComboBox(QComboBox *comboBox);
    void writeToLineEdit(QLineEdit *lineEdit);
    void writeToTextEdit(QTextEdit *textEdit);
    void setupWidgets();
    void writeToWidgets();
    void writeFromWidgets();

    QDataWidgetMapper *m_mapper;
    int m_row;
    QSqlRelationalTableModel *m_model;
    QMap<int, QComboBox*> m_columnComboBox;
    QMap<int, QLineEdit*> m_columnLineEdit;
    QMap<int, QTextEdit*> m_columnTextEdit;
    QPushButton *m_okButton;
};

#endif // SQLRELATIONALTABLEDIALOG_H
