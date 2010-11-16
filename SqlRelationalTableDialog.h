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
    explicit SqlRelationalTableDialog(QWidget *parent = 0);
    void writeToModel();

    enum Mode {
        NewMode,
        EditMode
    };

    void setModelRow(QSqlRelationalTableModel *model, int row, Mode mode = EditMode);
public slots:
    void accept();
protected slots:
    void checkWriteReady();
protected:
    void writeToComboBox(QComboBox *comboBox);
    void writeFromComboBox(QComboBox *comboBox);
    void writeToLineEdit(QLineEdit *lineEdit);
    void writeFromLineEdit(QLineEdit *lineEdit);
    void writeToTextEdit(QTextEdit *textEdit);
    void writeFromTextEdit(QTextEdit *textEdit);
    void setupWidgets();
    void writeToWidgets();
    void writeFromWidgets();
    virtual QPushButton* okButton() = 0;

    QDataWidgetMapper *m_mapper;
    int m_row;
    Mode m_mode;
    QSqlRelationalTableModel *m_model;
    QSqlRelationalDelegate *m_delegate;
    QMap<int, QComboBox*> m_columnComboBox;
    QMap<int, QLineEdit*> m_columnLineEdit;
    QMap<int, QTextEdit*> m_columnTextEdit;
};

#endif // SQLRELATIONALTABLEDIALOG_H
