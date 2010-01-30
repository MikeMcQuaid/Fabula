#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSettings>
#include <QDesktopServices>

namespace Ui {
    class MainWindow;
}

class Database;
class QSqlRelationalTableModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void newFile();
    void openFile(QString fileName = QString());

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    Database *database;
    QSqlRelationalTableModel *eventsModel;
    QSqlRelationalTableModel *conversationsTableModel;
    QSettings settings;
    QDesktopServices desktopServices;
};

#endif // MAINWINDOW_H
