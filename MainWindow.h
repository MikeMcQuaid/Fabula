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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSettings>
#include <QDesktopServices>
#include <QModelIndex>

#include "SqlRelationalTableDialog.h"

namespace Ui {
    class MainWindow;
}

class Database;
class QAbstractItemView;
class QSqlRelationalTableModel;
class QSortFilterProxyModel;
class TableToTreeProxyModel;
class HideColumnsProxyModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum TreeItem {
        CharacterItem,
        ConversationItem
    };

    enum DialogMode {
        NewMode,
        EditMode
    };

public slots:
    void newFile();
    void openFile(QString fileName = QString());
    void filterOnConversation(const QModelIndex& index);
    void addEvent();
    void deleteEvent();
    void addConversation();
    void deleteConversation();
    void addCharacter();
    void deleteCharacter();
    void addOrEditTreeItem(DialogMode mode, TreeItem treeItem,
                           const QModelIndex &index = QModelIndex());
    void editViewItem(const QModelIndex &index,
                      SqlRelationalTableDialog *dialog,
                      DialogMode mode);
    void deleteViewItem(const QModelIndex &index, QSqlRelationalTableModel *model);
    void reloadTree();
    void reloadEvents();
    void editEvent(const QModelIndex &index);
    void editTreeItem(const QModelIndex &index);
    QModelIndex rootModelIndex(const QModelIndex &index);
    QAbstractItemModel* rootModel(QAbstractItemModel *model);
    void openPreferences();

protected:
    void changeEvent(QEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::MainWindow *ui;
    Database *database;

    QSettings settings;
    QDesktopServices desktopServices;
    QSortFilterProxyModel *eventsFilterModel;
    TableToTreeProxyModel *conversationsTreeModel;
    HideColumnsProxyModel *hideEventsTableColumnsModel;
};

#endif // MAINWINDOW_H
