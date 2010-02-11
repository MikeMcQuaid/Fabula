#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSettings>
#include <QDesktopServices>
#include <QModelIndex>

namespace Ui {
    class MainWindow;
}

class Database;
class QSqlRelationalTableModel;
class QSortFilterProxyModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void newFile();
    void openFile(QString fileName = QString());
    void filterOnConversation(const QModelIndex& index);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    Database *database;
    QSqlRelationalTableModel *eventsModel;
    QSqlRelationalTableModel *conversationsTableModel;
    QSortFilterProxyModel *eventsFilter;
    QSettings settings;
    QDesktopServices desktopServices;
};

#endif // MAINWINDOW_H
