#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Database.h"
#include "TableTreeModel.h"

#include <QFileDialog>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QSqlRelationalDelegate>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    database(0),
    eventsModel(0),
    conversationsTableModel(0)
{
    ui->setupUi(this);

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));

    QString fileName = settings.value("database").toString();

    if (fileName.isEmpty()) {
        newFile();
    }
    else {
        openFile(fileName);
    }

    eventsModel = new QSqlRelationalTableModel(this);
    eventsModel->setTable("events");
    eventsModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    eventsModel->setRelation(1, QSqlRelation("conversations", "id", "name"));
    eventsModel->setRelation(2, QSqlRelation("characters", "id", "name"));
    eventsModel->setRelation(3, QSqlRelation("audiofiles", "id", "url"));

    eventsModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    eventsModel->setHeaderData(1, Qt::Horizontal, tr("Conversation"));
    eventsModel->setHeaderData(2, Qt::Horizontal, tr("Character"));
    eventsModel->setHeaderData(3, Qt::Horizontal, tr("Audio File"));
    eventsModel->setHeaderData(4, Qt::Horizontal, tr("Text"));

    eventsModel->select();

    ui->eventsView->setModel(eventsModel);
    ui->eventsView->setItemDelegate(new QSqlRelationalDelegate(ui->eventsView));
    ui->eventsView->hideColumn(0);
    ui->eventsView->resizeColumnsToContents();

    TreeModel *conversationsTreeModel = new TreeModel(this);

    ui->conversationsView->setModel(conversationsTreeModel);
}

void MainWindow::newFile()
{
    QString fileName = QFileDialog::getSaveFileName(
            this, tr("New File"),
            desktopServices.storageLocation(QDesktopServices::DocumentsLocation),
            tr("Fabula Files (*.fabula)"));
    openFile(fileName);
}

void MainWindow::openFile(QString fileName)
{
    if (fileName.isEmpty()) {
        fileName = QFileDialog::getOpenFileName(
                this, tr("Open File"),
                desktopServices.storageLocation(QDesktopServices::DocumentsLocation),
                tr("Fabula Files (*.fabula)"));
    }
    if (!fileName.isEmpty()) {
        if (database) {
            delete database;
            database = 0;
        }
        database = new Database(fileName, this);
        settings.setValue("database", fileName);
        if (eventsModel) {
            eventsModel->select();
        }
        if (conversationsTableModel) {
            conversationsTableModel->select();
        }
        setWindowTitle(QString("%1 - Fabula").arg(fileName));
        ui->centralWidget->setEnabled(true);
    }
    else {
        if (!database) {
            ui->centralWidget->setEnabled(false);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
