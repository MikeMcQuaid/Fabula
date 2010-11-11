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

#ifdef QT_DEBUG
    bool debugMode = true;
#else
    bool debugMode = false;
#endif

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

    setupRelations();
}

bool Database::create()
{
    QMap<QString, QString> databaseStructure;

    databaseStructure.insert(CharactersTable,
                             "id integer primary key autoincrement, "
                             "name text unique");

    databaseStructure.insert(WritersTable,
                             "id integer primary key autoincrement, "
                             "name text unique");

    databaseStructure.insert(ConversationsTable,
                             "id integer primary key autoincrement, "
                             "conversation_type_id integer not null, "
                             "writer_id integer not null, "
                             "name text unique");

    databaseStructure.insert(ConversationsEventsTable,
                             "id integer primary key autoincrement, "
                             "conversation_id integer not null, "
                             "event_id integer not null, "
                             "sort integer not null");

    databaseStructure.insert(EventsTable,
                             "id integer primary key autoincrement, "
                             "event_type_id integer not null, "
                             "conversation_id integer not null, "
                             "character_id integer, "
                             "audiofile_id integer, "
                             "text text");

    databaseStructure.insert(AudiofilesTable,
                             "id integer primary key autoincrement, "
                             "url text");

    databaseStructure.insert(ConversationTypesTable,
                             "id integer primary key autoincrement, "
                             "name text unique");

    databaseStructure.insert(EventTypesTable,
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

    if (debugMode)
        return insertDummyData();

    return true;
}

bool Database::insertDummyData()
{
    QStringList insertQueries;
    QString insert("insert into %1(id, %2) values (%3)");

    QString insertCharacter(insert.arg(CharactersTable).arg("name"));
    insertQueries.append(insertCharacter.arg("1, 'Mike'"));
    insertQueries.append(insertCharacter.arg("2, 'Bob'"));
    insertQueries.append(insertCharacter.arg("3, 'James'"));
    insertQueries.append(insertCharacter.arg("4, 'David'"));
    insertQueries.append(insertCharacter.arg("5, 'Terence'"));

    QString insertWriter(insert.arg(WritersTable).arg("name"));
    insertQueries.append(insertWriter.arg("1, 'Jonas'"));
    insertQueries.append(insertWriter.arg("2, 'Gelo'"));

    QString insertConversation(insert.arg(ConversationsTable).arg("conversation_type_id, writer_id, name"));
    insertQueries.append(insertConversation.arg("1, 1, 1, 'First Meeting'"));
    insertQueries.append(insertConversation.arg("2, 2, 2, 'Drunken Reunion'"));

    QString insertConversationEvent(insert.arg(ConversationsEventsTable).arg("conversation_id, event_id, sort"));
    insertQueries.append(insertConversationEvent.arg("1, 1, 1, 1"));
    insertQueries.append(insertConversationEvent.arg("2, 2, 2, 2"));

    QString insertEvent(insert.arg(EventsTable).arg("event_type_id, conversation_id, character_id, audiofile_id, text"));
    insertQueries.append(insertEvent.arg("1, 1, 1, 1, 1, 'Hey dude, how is it going?'"));
    insertQueries.append(insertEvent.arg("2, 1, 2, 2, 2, 'Fine day today, eh?'"));
    insertQueries.append(insertEvent.arg("3, 1, 1, 3, 1, 'Is your face always that colour?'"));
    insertQueries.append(insertEvent.arg("4, 1, 2, 4, 1, 'Why would you say that?'"));
    insertQueries.append(insertEvent.arg("5, 1, 1, 5, 1, 'I slap your face!'"));

    QString insertAudiofile(insert.arg(AudiofilesTable).arg("url"));
    insertQueries.append(insertAudiofile.arg("1, 'Recording.mp4'"));
    insertQueries.append(insertAudiofile.arg("2, 'Recording2.wav'"));

    QString insertConversationType(insert.arg(ConversationTypesTable).arg("name"));
    insertQueries.append(insertConversationType.arg("1, 'Interactive'"));
    insertQueries.append(insertConversationType.arg("2, 'Overhead'"));
    insertQueries.append(insertConversationType.arg("3, 'Subsequent'"));
    insertQueries.append(insertConversationType.arg("4, 'AI Bark'"));

    QString insertEventType(insert.arg(EventTypesTable).arg("name"));
    insertQueries.append(insertEventType.arg("1, 'Speech'"));
    insertQueries.append(insertEventType.arg("2, 'Logic'"));
    insertQueries.append(insertEventType.arg("3, 'Comment'"));

    QSqlQuery sqlQuery;
    foreach(const QString &query, insertQueries) {
        if (!sqlQuery.exec(query)) {
            qWarning() << tr("Unable to run insertion: '%1'").arg(query);
            qWarning() << sqlQuery.lastError().text();
            return false;
        }
    }

    return true;
}

QMap<int, QSqlRelation> Database::tableRelations(const QLatin1String &table) const
{
    return m_tableRelations.value(table);
}

void Database::setupRelations()
{
    QMap<int, QSqlRelation> conversationsRelations;
    conversationsRelations.insert(1, QSqlRelation(ConversationTypesTable, "id", "name"));
    conversationsRelations.insert(2, QSqlRelation(WritersTable, "id", "name"));
    m_tableRelations.insert(ConversationsTable, conversationsRelations);

    QMap<int, QSqlRelation> conversationsEventsRelations;
    conversationsEventsRelations.insert(1, QSqlRelation(ConversationsTable, "id", "name"));
    conversationsEventsRelations.insert(2, QSqlRelation(EventsTable, "id", "text"));
    m_tableRelations.insert(ConversationsEventsTable, conversationsEventsRelations);

    QMap<int, QSqlRelation> eventsRelations;
    eventsRelations.insert(1, QSqlRelation(EventTypesTable, "id", "name"));
    eventsRelations.insert(2, QSqlRelation(ConversationsTable, "id", "name"));
    eventsRelations.insert(3, QSqlRelation(CharactersTable, "id", "name"));
    eventsRelations.insert(4, QSqlRelation(AudiofilesTable, "id", "url"));
    m_tableRelations.insert(EventsTable, eventsRelations);
}

Database::~Database()
{
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}
