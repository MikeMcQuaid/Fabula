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

#include "Database.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMap>
#include <QStringList>
#include <QDebug>

Database::Database(const QString &path, QObject *parent) :
        QObject(parent)
{
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        qWarning() << tr("Unable to find Qt SQLite plugin.");
        return;
    }

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(path);
    if (!database.open()) {
        qWarning() << tr("Unable to open database at '%1'.").arg(path);
        return;
    }

    if (database.tables().isEmpty()) {
        if (!create()) {
            qWarning() << tr("Unable to create all database tables");
            return;
        }
    }
}

// TODO: Add conversation global variable prerequisites

bool Database::create()
{
    QMap<QString, QString> databaseStructure;

    databaseStructure.insert("characters",
                             "id integer primary key autoincrement, "
                             "name text unique");

    databaseStructure.insert("writers",
                             "id integer primary key autoincrement, "
                             "name text unique");

    databaseStructure.insert("conversations",
                             "id integer primary key autoincrement, "
                             "type integer not null, "
                             "writer_id integer not null, "
                             "name text unique");

    databaseStructure.insert("conversations_events",
                             "id integer primary key autoincrement, "
                             "conversation_id integer not null, "
                             "event_id integer not null, "
                             "sort integer not null");

    databaseStructure.insert("events",
                             "id integer primary key autoincrement, "
                             "event_type_id integer not null, "
                             "conversation_id integer not null, "
                             "character_id integer, "
                             "audiofile_id integer, "
                             "text text");

    databaseStructure.insert("audiofiles",
                             "id integer primary key autoincrement, "
                             "url text");

    databaseStructure.insert("conversation_types",
                             "id integer primary key autoincrement, "
                             "name text unique");

    databaseStructure.insert("event_types",
                             "id integer primary key autoincrement, "
                             "name text unique");

    QSqlQuery sqlQuery;
    foreach(const QString &tableName, databaseStructure.keys()) {
        QString query = "create table %1(%2)";
        query = query.arg(tableName);
        query = query.arg(databaseStructure.value(tableName));
        if (!sqlQuery.exec(query)) {
            qWarning() << tr("Unable to create '%1' table:").arg(tableName);
            qWarning() << sqlQuery.lastError().text();
            return false;
        }
    }

    // TODO: Temporary import of dummy data for testing
    sqlQuery.exec("insert into characters(id, name) values (1, 'Character')");
    sqlQuery.exec("insert into characters(id, name) values (2, 'Character2')");
    sqlQuery.exec("insert into characters(id, name) values (3, 'Character3')");
    sqlQuery.exec("insert into characters(id, name) values (4, 'Character4')");
    sqlQuery.exec("insert into characters(id, name) values (4, 'Character5')");
    sqlQuery.exec("insert into writers(id, name) values (1, 'Writer')");
    sqlQuery.exec("insert into writers(id, name) values (2, 'Writer2')");
    sqlQuery.exec("insert into conversations(id, type, writer_id, name) values (1, 1, 1, 'Conversation')");
    sqlQuery.exec("insert into conversations(id, type, writer_id, name) values (2, 2, 2, 'Conversation2')");
    sqlQuery.exec("insert into conversations_events(id, conversation_id, event_id, sort) values (1, 1, 1, 1)");
    sqlQuery.exec("insert into conversations_events(id, conversation_id, event_id, sort) values (2, 2, 2, 2)");
    sqlQuery.exec("insert into events(id, event_type_id, conversation_id, character_id, audiofile_id, text) values (1, 1, 1, 1, 1, 'There once was a man named Bob1')");
    sqlQuery.exec("insert into events(id, event_type_id, conversation_id, character_id, audiofile_id, text) values (2, 1, 2, 2, 2, 'There once was a man named Bob2')");
    sqlQuery.exec("insert into events(id, event_type_id, conversation_id, character_id, audiofile_id, text) values (3, 1, 1, 3, 1, 'There once was a man named Bob3')");
    sqlQuery.exec("insert into events(id, event_type_id, conversation_id, character_id, audiofile_id, text) values (4, 1, 2, 4, 1, 'There once was a man named Bob4')");
    sqlQuery.exec("insert into events(id, event_type_id, conversation_id, character_id, audiofile_id, text) values (5, 1, 1, 5, 1, 'There once was a man named Bob5')");
    sqlQuery.exec("insert into audiofiles(id, url) values (1, 'AudioFile.mp4')");
    sqlQuery.exec("insert into audiofiles(id, url) values (2, 'AudioFile2.mp4')");
    sqlQuery.exec("insert into conversation_types(id, name) values (1, 'ConversationType')");
    sqlQuery.exec("insert into conversation_types(id, name) values (2, 'ConversationType2')");
    sqlQuery.exec("insert into event_types(id, name) values (1, 'Speech')");
    sqlQuery.exec("insert into event_types(id, name) values (2, 'Logic')");
    sqlQuery.exec("insert into event_types(id, name) values (3, 'Comment')");

    return true;
}

Database::~Database()
{
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}
