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
#include "TableToTreeProxyModel.h"
#include "PreferencesDialog.h"
#include "ConversationDialog.h"
#include "EventDialog.h"

#include <QFileDialog>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QFile>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    database(0),
    eventsModel(0),
    eventsFilterModel(0),
    conversationsTreeModel(0),
    conversationsTableModel(0)
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

    ui->conversationsView->installEventFilter(this);
    ui->eventsView->installEventFilter(this);

    connect(ui->conversationsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(filterOnConversation(QModelIndex)));

    eventsModel = new QSqlRelationalTableModel();
    eventsModel->setTable(EventsTable);
    eventsModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    // TODO: Get these columns from database
    eventsModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    eventsModel->setHeaderData(1, Qt::Horizontal, tr("Type"));
    eventsModel->setHeaderData(2, Qt::Horizontal, tr("Character"));
    eventsModel->setHeaderData(3, Qt::Horizontal, tr("Conversation"));
    eventsModel->setHeaderData(4, Qt::Horizontal, tr("Audio File"));
    eventsModel->setHeaderData(5, Qt::Horizontal, tr("Text"));

    QMap<int, QSqlRelation> eventsRelations = database->tableRelations(EventsTable);
    foreach(int column, eventsRelations.keys())
        eventsModel->setRelation(column, eventsRelations.value(column));

    eventsModel->select();

    eventsFilterModel = new QSortFilterProxyModel(this);
    eventsFilterModel->setSourceModel(eventsModel);

    ui->eventsView->setModel(eventsFilterModel);
    ui->eventsView->hideColumn(0);
    ui->eventsView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    ui->eventsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->eventsView, SIGNAL(activated(QModelIndex)), this, SLOT(editEvent(QModelIndex)));

    conversationsTableModel = new QSqlRelationalTableModel();
    conversationsTableModel->setTable(ConversationsTable);
    conversationsTableModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    QMap<int, QSqlRelation> conversationsRelations = database->tableRelations(ConversationsTable);
    foreach(int column, conversationsRelations.keys())
        conversationsTableModel->setRelation(column, conversationsRelations.value(column));

    conversationsTableModel->select();

    conversationsTreeModel = new TableToTreeProxyModel(this);
    QList<int> hideColumns;
    hideColumns << 0 << 1;
    conversationsTreeModel->setHideColumns(hideColumns);
    conversationsTreeModel->setSourceModel(eventsModel);
    ui->conversationsView->setModel(conversationsTreeModel);
    ui->conversationsView->sortByColumn(0, Qt::AscendingOrder);

    ui->conversationsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->conversationsView, SIGNAL(activated(QModelIndex)), this, SLOT(editConversation(QModelIndex)));

    connect(eventsModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(reloadConversations()));
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
    // TODO: Get these from the database
    static const int characterRow = 2;
    static const int conversationRow = 3;

    if (!eventsFilterModel || !index.isValid())
        return;

    QString filter = index.model()->data(index).toString();
    int row = 0;
    // TODO: Do this in a less hacky fashion
    if (index.parent().isValid())
        row = conversationRow;
    else
        row = characterRow;

    eventsFilterModel->setFilterKeyColumn(row);
    eventsFilterModel->setFilterFixedString(filter);
}

void MainWindow::addEvent()
{
    QModelIndex index = ui->eventsView->currentIndex();
    if (!index.isValid())
        index = ui->eventsView->model()->index(0, 0);
    editViewItem(index, new EventDialog(this),
                 SqlRelationalTableDialog::NewMode, eventsModel);
}

void MainWindow::editEvent(const QModelIndex &index)
{
    editViewItem(index, new EventDialog(this),
                 SqlRelationalTableDialog::EditMode, eventsModel);
}

void MainWindow::deleteEvent()
{
    QMessageBox::StandardButton result =
            QMessageBox::question(this,
                                  "Delete confirmation",
                                  "Are you sure you wish to delete the current event?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes)
        deleteViewItem(ui->eventsView->currentIndex(), eventsModel);
}

void MainWindow::addConversation()
{
    QModelIndex index = ui->conversationsView->currentIndex();
    if (!index.isValid())
        index = ui->conversationsView->model()->index(0, 0);
    editConversationItem(SqlRelationalTableDialog::NewMode, index);
}

void MainWindow::editConversation(const QModelIndex &index)
{
    editConversationItem(SqlRelationalTableDialog::EditMode, index);
}

void MainWindow::editConversationItem(SqlRelationalTableDialog::Mode mode, const QModelIndex &index)
{
    QModelIndex eventsTableIndex = rootModelIndex(index);

    // TODO: Get columns from Database
    // TODO: Make this work for the character too
    const int eventsTableConversationColumn = 3;
    const int conversationsTableNameColumn = 3;
    QModelIndex eventsConversationIndex =
            eventsModel->index(eventsTableIndex.row(), eventsTableConversationColumn);
    const QString &conversation = eventsConversationIndex.data().toString();

    QModelIndex firstConversation =
            conversationsTableModel->index(0, conversationsTableNameColumn);
    QModelIndexList conversationIndexes =
            conversationsTableModel->match(firstConversation, Qt::DisplayRole, conversation);

    Q_ASSERT(!conversationIndexes.isEmpty());
    if (conversationIndexes.isEmpty())
        return;

    QModelIndex conversationIndex = conversationIndexes.first();

    editViewItem(conversationIndex, new ConversationDialog(this),
                 mode, conversationsTableModel);
}

void MainWindow::deleteConversation()
{
    Q_ASSERT(false);
    QMessageBox::StandardButton result =
            QMessageBox::question(this,
                                  "Delete confirmation",
                                  "Are you sure you wish to delete the current conversation?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes)
        deleteViewItem(ui->conversationsView->currentIndex(), conversationsTableModel);
}

void MainWindow::editViewItem(const QModelIndex &index, SqlRelationalTableDialog *dialog,
                              SqlRelationalTableDialog::Mode mode, QSqlRelationalTableModel *model)
{
    Q_ASSERT(model);
    if (!model)
        return;

    Q_ASSERT(model->editStrategy() == QSqlTableModel::OnFieldChange);

    const QModelIndex &rootIndex = rootModelIndex(index);
    Q_ASSERT(rootIndex.isValid());

    const int row = rootIndex.row();

    Q_ASSERT(dialog);
    if (!dialog)
        return;

    dialog->setWindowModality(Qt::WindowModal);
    dialog->setModelRow(model, row, mode);
    int result = dialog->exec();
    if (result == QDialog::Accepted) {
        if (mode == SqlRelationalTableDialog::NewMode) {
            bool rowWasInserted = model->insertRow(row);
            Q_ASSERT(rowWasInserted);
            if (!rowWasInserted)
                return;
        }
        dialog->writeToModel();
        model->submit();
    }
}

void MainWindow::deleteViewItem(const QModelIndex &index, QSqlRelationalTableModel *model)
{
    Q_ASSERT(model);
    if (!model)
        return;

    const QModelIndex &rootIndex = rootModelIndex(index);

    bool rowWasRemoved = model->removeRow(rootIndex.row());
    Q_ASSERT(rowWasRemoved);
    if (!rowWasRemoved)
        return;
    model->submit();
}

void MainWindow::reloadConversations()
{
    if (conversationsTableModel)
        conversationsTableModel->select();
}

void MainWindow::reloadEvents()
{
    if (eventsModel)
        eventsModel->select();
}

QModelIndex MainWindow::rootModelIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return QModelIndex();

    QModelIndex rootIndex(index);
    const QSortFilterProxyModel *proxyModel = 0;
    while ((proxyModel = qobject_cast<const QSortFilterProxyModel*>(rootIndex.model())))
        rootIndex = proxyModel->mapToSource(rootIndex);
    return rootIndex;
}

QAbstractItemModel* MainWindow::rootModel(QAbstractItemModel *model)
{
    QSortFilterProxyModel *proxyModel = 0;

    while ((proxyModel = qobject_cast<QSortFilterProxyModel*>(model)))
        model = proxyModel->sourceModel();
    return model;
}

MainWindow::~MainWindow()
{
    delete ui;
    // Delete here rather than using this object as parent so we can ensure the
    // database is deleted last.
    delete eventsModel;
    delete database;
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    switch (event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        // TODO: Perhaps enable backspace on OSX too?
        if (keyEvent->matches(QKeySequence::Delete)) {
            if (object == ui->eventsView) {
                ui->actionDelete_Event->trigger();
                return true;
            }
            if (object == ui->conversationsView) {
                ui->actionDelete_Conversation->trigger();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(object, event);
}
