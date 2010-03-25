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
class SqlTreeModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void newFile();
    void openFile(QString fileName = QString());
    void filterOnConversation(const QModelIndex& index);
    void addEvent();
    void deleteEvent();
    void addToConversationTree();
    void removeFromConversationTree();
    void reloadEvents();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    Database *database;
    QSqlRelationalTableModel *eventsModel;
    QSettings settings;
    QDesktopServices desktopServices;
    SqlTreeModel *conversationsModel;
};

#endif // MAINWINDOW_H
