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
#include <QSortFilterProxyModel>

class QSqlRelationalTableModelDebug : public QSqlRelationalTableModel
{
public:
    virtual QString selectStatement() { return QSqlRelationalTableModel::selectStatement(); }
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    database(0),
    eventsModel(0),
    conversationsTableModel(0)
{
    ui->setupUi(this);

    ui->statusBar->hide();

    ui->splitter->setStretchFactor(1, 3);

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->actionAdd_Event, SIGNAL(triggered()), this, SLOT(addEvent()));
    connect(ui->actionDelete_Event, SIGNAL(triggered()), this, SLOT(deleteEvent()));
    connect(ui->actionAdd_Conversation, SIGNAL(triggered()), this, SLOT(addToConversationTree()));
    connect(ui->actionDelete_Conversation, SIGNAL(triggered()), this, SLOT(removeFromConversationTree()));
    connect(ui->actionAdd_Character, SIGNAL(triggered()), this, SLOT(addToConversationTree()));
    connect(ui->actionDelete_Character, SIGNAL(triggered()), this, SLOT(removeFromConversationTree()));

    connect(ui->conversationsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(filterOnConversation(QModelIndex)));

    QString fileName = settings.value("database").toString();

    if (fileName.isEmpty()) {
        newFile();
    }
    else {
        openFile(fileName);
    }

    eventsModel = new QSqlRelationalTableModelDebug();
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

    eventsFilter = new QSortFilterProxyModel(this);
    eventsFilter->setSourceModel(eventsModel);
    eventsFilter->setDynamicSortFilter(true);

    ui->eventsView->setModel(eventsModel);
    ui->eventsView->setItemDelegate(new QSqlRelationalDelegate(ui->eventsView));
    //ui->eventsView->hideColumn(0);
    ui->eventsView->resizeColumnsToContents();
    ui->eventsView->setWordWrap(true);

    conversationsTreeModel = new TreeModel(this);
    ui->conversationsView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->conversationsView->setUniformRowHeights(true);
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
        database = new Database(fileName);
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

void MainWindow::filterOnConversation(const QModelIndex& index)
{
    QString filter;
    if (index.parent().isValid())
        //FIXME: Nasty table names
        filter = QString("relTblAl_1.name='%1'").arg(conversationsTreeModel->data(index).toString());
    else
        //FIXME: Nasty table names
        filter = QString("relTblAl_2.name='%1'").arg(conversationsTreeModel->data(index).toString());
    eventsModel->setFilter(filter);
}

void MainWindow::addEvent()
{
    eventsModel->insertRow(ui->eventsView->currentIndex().row()+1);
}

void MainWindow::deleteEvent()
{
    eventsModel->removeRow(ui->eventsView->currentIndex().row());
}

void MainWindow::addToConversationTree()
{
    conversationsTreeModel->insertRow(ui->conversationsView->currentIndex().row(),
                                      ui->conversationsView->currentIndex().parent());
}

void MainWindow::removeFromConversationTree()
{
    conversationsTreeModel->removeRow(ui->conversationsView->currentIndex().row(),
                                      ui->conversationsView->currentIndex().parent());
}

MainWindow::~MainWindow()
{
    delete ui;
    // Delete here rather than using this object as parent so we can ensure the
    // database is deleted last.
    delete eventsModel;
    delete database;
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
