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
#include <QMetaEnum>

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

bool Database::create()
{
    QMap<QString, QString> databaseStructure;
    databaseStructure.insert(tableName(Character),
                             "id integer primary key autoincrement, "
                             "name text unique");
    databaseStructure.insert(tableName(Writer),
                             "id integer primary key autoincrement, "
                             "name text unique");
    databaseStructure.insert(tableName(Conversation),
                             "id integer primary key autoincrement, "
                             "conversation_type_id integer not null, "
                             "writer_id integer not null, "
                             "name text unique");
    databaseStructure.insert(tableName(ConversationEvent),
                             "id integer primary key autoincrement, "
                             "conversation_id integer not null, "
                             "event_id integer not null, "
                             "sort integer not null");
    databaseStructure.insert(tableName(Event),
                             "id integer primary key autoincrement, "
                             "event_type_id integer not null, "
                             "character_id integer, "
                             "conversation_id integer not null, "
                             "audiofile text unique, "
                             "text text");
    databaseStructure.insert(tableName(ConversationType),
                             "id integer primary key autoincrement, "
                             "name text unique");
    databaseStructure.insert(tableName(EventType),
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

#ifdef QT_DEBUG
    return insertTestData();
#else
    return true;
#endif
}

bool Database::insertTestData()
{
    QStringList insertQueries;
    QString insert("insert into %1(id, %2) values (%3)");

    QString insertCharacter(insert.arg(tableName(Character)).arg("name"));
    insertQueries.append(insertCharacter.arg("1, 'Mike'"));
    insertQueries.append(insertCharacter.arg("2, 'Bob'"));
    insertQueries.append(insertCharacter.arg("3, 'James'"));
    insertQueries.append(insertCharacter.arg("4, 'David'"));
    insertQueries.append(insertCharacter.arg("5, 'Terence'"));

    QString insertWriter(insert.arg(tableName(Writer)).arg("name"));
    insertQueries.append(insertWriter.arg("1, 'Jonas'"));
    insertQueries.append(insertWriter.arg("2, 'Gelo'"));

    QString insertConversation(insert.arg(tableName(Conversation)).arg("conversation_type_id, writer_id, name"));
    insertQueries.append(insertConversation.arg("1, 1, 1, 'First Meeting'"));
    insertQueries.append(insertConversation.arg("2, 2, 2, 'Drunken Reunion'"));

    QString insertConversationEvent(insert.arg(tableName(ConversationEvent)).arg("conversation_id, event_id, sort"));
    insertQueries.append(insertConversationEvent.arg("1, 1, 1, 1"));
    insertQueries.append(insertConversationEvent.arg("2, 2, 2, 2"));

    QString insertEvent(insert.arg(tableName(Event)).arg("event_type_id, character_id, conversation_id, audiofile, text"));
    insertQueries.append(insertEvent.arg("1, 1, 1, 1, '1.mp4', 'Hey dude, how is it going?'"));
    insertQueries.append(insertEvent.arg("2, 1, 2, 2, '2.mp3', 'Fine day today, eh?'"));
    insertQueries.append(insertEvent.arg("3, 1, 3, 1, '3.wav', 'Is your face always that colour?'"));
    insertQueries.append(insertEvent.arg("4, 1, 4, 2, '4.mp3', 'Why would you say that?'"));
    insertQueries.append(insertEvent.arg("5, 1, 5, 1, '5.mp3', 'I slap your face!'"));

    QString insertConversationType(insert.arg(tableName(ConversationType)).arg("name"));
    insertQueries.append(insertConversationType.arg("1, 'Interactive'"));
    insertQueries.append(insertConversationType.arg("2, 'Overhead'"));
    insertQueries.append(insertConversationType.arg("3, 'Subsequent'"));
    insertQueries.append(insertConversationType.arg("4, 'AI Bark'"));

    QString insertEventType(insert.arg(tableName(EventType)).arg("name"));
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

QString Database::tableName(Table table)
{
    Q_ASSERT(staticMetaObject.enumeratorCount() == 1);
    QMetaEnum enumerator =
            staticMetaObject.enumerator(staticMetaObject.enumeratorOffset());
    QString tableName(enumerator.valueToKey(table));
    Q_ASSERT(!tableName.isEmpty());
    return tableName;
}

QMap<int, QSqlRelation> Database::tableRelations(Table table)
{
    static QMap<Table, QMap<int, QSqlRelation> > relations;
    if (relations.isEmpty()) {
        QMap<int, QSqlRelation> conversationsRelations;
        conversationsRelations.insert(1, QSqlRelation(tableName(ConversationType), "id", "name"));
        conversationsRelations.insert(2, QSqlRelation(tableName(Writer), "id", "name"));
        relations.insert(Conversation, conversationsRelations);

        QMap<int, QSqlRelation> conversationsEventsRelations;
        conversationsEventsRelations.insert(1, QSqlRelation(tableName(Conversation), "id", "name"));
        conversationsEventsRelations.insert(2, QSqlRelation(tableName(Event), "id", "text"));
        relations.insert(ConversationEvent, conversationsEventsRelations);

        QMap<int, QSqlRelation> eventsRelations;
        eventsRelations.insert(1, QSqlRelation(tableName(EventType), "id", "name"));
        eventsRelations.insert(2, QSqlRelation(tableName(Character), "id", "name"));
        eventsRelations.insert(3, QSqlRelation(tableName(Conversation), "id", "name"));
        relations.insert(Event, eventsRelations);
    }

    return relations.value(table);
}

Database::~Database()
{
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}
