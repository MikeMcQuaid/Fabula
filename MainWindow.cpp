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
#include "TableTreeModel.h"
#include "PreferencesDialog.h"
#include "EventDialog.h"
#include "EventDelegate.h"

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
    conversationsModel(0)
{
    ui->setupUi(this);

    QString fileName = settings.value("database").toString();

    if (fileName.isEmpty()) {
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
    connect(ui->actionAdd_Conversation, SIGNAL(triggered()), this, SLOT(addToConversationTree()));
    connect(ui->actionDelete_Conversation, SIGNAL(triggered()), this, SLOT(removeFromConversationTree()));
    connect(ui->actionPreferences, SIGNAL(triggered()), preferences, SLOT(open()));

    connect(ui->conversationsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(filterOnConversation(QModelIndex)));

    eventsModel = new QSqlRelationalTableModel();
    eventsModel->setTable("events");
    eventsModel->setEditStrategy(QSqlTableModel::OnFieldChange);

    eventsModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    eventsModel->setHeaderData(1, Qt::Horizontal, tr("Type"));
    eventsModel->setRelation(1, QSqlRelation("event_types", "id", "name"));
    eventsModel->setHeaderData(2, Qt::Horizontal, tr("Conversation"));
    eventsModel->setRelation(2, QSqlRelation("conversations", "id", "name"));
    eventsModel->setHeaderData(3, Qt::Horizontal, tr("Character"));
    eventsModel->setRelation(3, QSqlRelation("characters", "id", "name"));
    eventsModel->setHeaderData(4, Qt::Horizontal, tr("Audio File"));
    eventsModel->setRelation(4, QSqlRelation("audiofiles", "id", "url"));
    eventsModel->setHeaderData(5, Qt::Horizontal, tr("Text"));

    eventsModel->select();

    ui->eventsView->setModel(eventsModel);
    ui->eventsView->hideColumn(0);
    ui->eventsView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    EventDelegate *eventDelegate = new EventDelegate(this);
    ui->eventsView->setItemDelegate(eventDelegate);

    conversationsModel = new SqlTreeModel(this);
    ui->conversationsView->setModel(conversationsModel);

    connect(conversationsModel, SIGNAL(submitted()), this, SLOT(reloadEvents()));
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
        reloadEvents();
        if (conversationsModel)
            conversationsModel->reset();
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

    if (!conversationsModel || !eventsModel)
        return;

    QString filter;
    int row = 0;
    if (index.parent().isValid())
        row = conversationRow;
    else
        row = characterRow;
    const QString name = conversationsModel->data(index).toString();
    // The "relTblAl_" is from the QSqlRelationalTableModel source.
    // This was needed as otherwise it's not obvious how to filter without breaking relations.
    filter = QString("relTblAl_%1.name='%2'").arg(row).arg(name);
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
    conversationsModel->insertRow(ui->conversationsView->currentIndex().row(),
                                      ui->conversationsView->currentIndex().parent());
}

void MainWindow::removeFromConversationTree()
{
    conversationsModel->removeRow(ui->conversationsView->currentIndex().row(),
                                      ui->conversationsView->currentIndex().parent());
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
