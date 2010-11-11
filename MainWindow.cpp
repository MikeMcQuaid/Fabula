/*  This file is part of Fabula.
    Copyright (C) 2010 Mike McQuaid <mike@mikemcquaid.com>

    Fabula is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fabula is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Fabula. If not, see <http://www.gnu.org/licenses/>.
*/

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Database.h"
#include "SqlTreeModel.h"
#include "PreferencesDialog.h"
#include "ConversationDialog.h"
#include "EventDialog.h"
#include "DialogDelegate.h"

#include <QFileDialog>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QFile>
#include <QDebug>
#include <QSortFilterProxyModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    database(0),
    eventsModel(0),
    conversationsTreeModel(0)
{
    ui->setupUi(this);

    QString fileName = settings.value("database").toString();

    if (fileName.isEmpty() || !QFile(fileName).exists()) {
        newFile();
    }
    else {
        openFile(fileName);
    }

    ui->statusBar->hide();

    ui->splitter->setStretchFactor(1, 1);

    PreferencesDialog *preferences = new PreferencesDialog(this);

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->actionAdd_Event, SIGNAL(triggered()), this, SLOT(addEvent()));
    connect(ui->actionDelete_Event, SIGNAL(triggered()), this, SLOT(deleteEvent()));
    connect(ui->actionAdd_Conversation, SIGNAL(triggered()), this, SLOT(addConversation()));
    connect(ui->actionDelete_Conversation, SIGNAL(triggered()), this, SLOT(deleteConversation()));
    connect(ui->actionPreferences, SIGNAL(triggered()), preferences, SLOT(open()));

    connect(ui->conversationsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(filterOnConversation(QModelIndex)));

    eventsModel = new QSqlRelationalTableModel();
    eventsModel->setTable(EventsTable);
    eventsModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    eventsModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    eventsModel->setHeaderData(1, Qt::Horizontal, tr("Type"));
    eventsModel->setHeaderData(2, Qt::Horizontal, tr("Conversation"));
    eventsModel->setHeaderData(3, Qt::Horizontal, tr("Character"));
    eventsModel->setHeaderData(4, Qt::Horizontal, tr("Audio File"));
    eventsModel->setHeaderData(5, Qt::Horizontal, tr("Text"));

    QMap<int, QSqlRelation> eventsRelations = database->tableRelations(EventsTable);
    foreach(int column, eventsRelations.keys())
        eventsModel->setRelation(column, eventsRelations.value(column));

    eventsModel->select();

    ui->eventsView->setModel(eventsModel);
    ui->eventsView->hideColumn(0);
    ui->eventsView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    DialogDelegate *eventDelegate = new DialogDelegate(this);
    ui->eventsView->setItemDelegate(eventDelegate);

    conversationsTreeModel = new SqlTreeModel(this);
    ui->conversationsView->setModel(conversationsTreeModel);

    connect(conversationsTreeModel, SIGNAL(submitted()), this, SLOT(reloadEvents()));
    connect(eventsModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(reloadConversations()));

    conversationsTableModel = new QSqlRelationalTableModel();
    conversationsTableModel->setTable(ConversationsTable);

    QMap<int, QSqlRelation> conversationsRelations = database->tableRelations(ConversationsTable);
    foreach(int column, conversationsRelations.keys())
        conversationsTableModel->setRelation(column, conversationsRelations.value(column));

    conversationsTableModel->select();
}

void MainWindow::newFile()
{
    QString fileName = QFileDialog::getSaveFileName(
            this, tr("New File"),
            desktopServices.storageLocation(QDesktopServices::DocumentsLocation),
            tr("Fabula Files (*.fabula)"));
    if (QFile::exists(fileName))
        QFile::remove(fileName);
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
        reloadConversations();
        reloadEvents();
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
    static const int conversationRow = 2;
    static const int characterRow = 3;

    if (!conversationsTreeModel || !eventsModel)
        return;

    QString filter;
    int row = 0;
    if (index.parent().isValid())
        row = conversationRow;
    else
        row = characterRow;
    const QString name = conversationsTreeModel->data(index).toString();
    // The "relTblAl_" is from the QSqlRelationalTableModel source.
    // This was needed as otherwise it's not obvious how to filter without breaking relations.
    filter = QString("relTblAl_%1.name='%2'").arg(row).arg(name);
    eventsModel->setFilter(filter);
}

void MainWindow::addEvent()
{
    addToView(ui->eventsView, new EventDialog(this));
}

void MainWindow::deleteEvent()
{
    deleteFromView(ui->eventsView);
}

void MainWindow::addConversation()
{
    addToView(ui->conversationsView, new ConversationDialog(this), conversationsTableModel);
}

void MainWindow::deleteConversation()
{
    deleteFromView(ui->conversationsView, conversationsTableModel);
}

void MainWindow::addToView(QAbstractItemView *view, SqlRelationalTableDialog *dialog, QSqlRelationalTableModel *model)
{
    // TODO set sensible row in this case
    int row = 0;

    if (!model) {
        model = qobject_cast<QSqlRelationalTableModel*>(view->model());
        Q_ASSERT(model);
        if (!model)
            return;
        row = view->currentIndex().row();
    }

    Q_ASSERT(dialog);
    if (!dialog)
        return;

    dialog->setWindowModality(Qt::WindowModal);
    dialog->setModelRow(model, row);
    int result = dialog->exec();
    if (result == QDialog::Accepted) {
        bool rowWasInserted = model->insertRow(row);
        Q_ASSERT(rowWasInserted);
        if (!rowWasInserted)
            return;
        dialog->writeToModel();
        model->submit();
    }
}

void MainWindow::deleteFromView(QAbstractItemView *view, QSqlRelationalTableModel *model)
{
    if (!model)
        model = qobject_cast<QSqlRelationalTableModel*>(view->model());
        Q_ASSERT(model);
        if (!model)
            return;

    bool rowWasRemoved = model->removeRow(view->currentIndex().row());
    Q_ASSERT(rowWasRemoved);
    if (!rowWasRemoved)
        return;
    model->submit();
}

void MainWindow::reloadConversations()
{
    if (!conversationsTreeModel || !ui->conversationsView)
        return;

    QModelIndex index = ui->conversationsView->currentIndex();
    const QString &table = conversationsTreeModel->data(index, SqlTreeModel::TableRole).toString();
    // TODO Only try to set new characters, not a conversation within it.
    if (table != CharactersTable) {
        qDebug() << table;
        return;
    }

    const QString &character = conversationsTreeModel->data(index, Qt::DisplayRole).toString();
    conversationsTreeModel->reset();
    // Reset the index to search from the beginning of the model
    index = conversationsTreeModel->index(0, 0);
    QModelIndexList indexes = conversationsTreeModel->match(index, Qt::DisplayRole, character);

    // TODO Re-add assert when works consistently
    //Q_ASSERT(!indexes.isEmpty());
    if (indexes.isEmpty())
        return;

    ui->conversationsView->setCurrentIndex(indexes.first());
}

void MainWindow::reloadEvents()
{
    if (eventsModel)
        eventsModel->select();
    if (ui->conversationsView)
        filterOnConversation(ui->conversationsView->currentIndex());
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
