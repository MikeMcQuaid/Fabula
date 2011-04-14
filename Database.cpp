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
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QMultiMap>

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

bool Database::insertTableRow(Table table, const QStringList& values)
{
    QSqlTableModel model;
    model.setTable(tableName(table));
    QSqlDatabase database = QSqlDatabase::database();
    QSqlRecord record = database.record(model.tableName());

    Q_ASSERT(values.count() == record.count());
    for(int field=0; field < record.count(); ++field)
        record.setValue(field, values.at(field));

    bool inserted = model.insertRecord(-1, record);
    if (!inserted)
        qWarning() << QObject::tr("Unable to run insert data:")
                   << model.lastError().text();

    return inserted;
}

bool Database::insertTestData()
{
    QMap<Table, QStringList> testData;
    Table table;
    QStringList tableValues;

    table = Character;
    tableValues.append("1, Mike");
    tableValues.append("2, Bob");
    tableValues.append("3, James");
    tableValues.append("4, David");
    tableValues.append("5, Terence");
    testData.insert(table, tableValues);
    tableValues.clear();

    table = Writer;
    tableValues.append("1, Jonas");
    tableValues.append("2, Gelo");
    testData.insert(table, tableValues);
    tableValues.clear();

    table = Conversation;
    tableValues.append("1, 1, 1, First Meeting");
    tableValues.append("2, 2, 2, Drunken Reunion");
    testData.insert(table, tableValues);
    tableValues.clear();

    table = ConversationEvent;
    tableValues.append("1, 1, 1, 1");
    tableValues.append("2, 2, 2, 2");
    testData.insert(table, tableValues);
    tableValues.clear();

    table = Event;
    tableValues.append("1, 1, 1, 1, 1.mp4, Hey dude! How is it going?");
    tableValues.append("2, 1, 2, 2, 2.mp3, Fine day today?");
    tableValues.append("3, 1, 3, 1, 3.wav, Is your face always that colour?");
    tableValues.append("4, 1, 4, 2, 4.mp3, Why would you say that?");
    tableValues.append("5, 1, 5, 1, 5.mp3, I slap your face!");
    testData.insert(table, tableValues);
    tableValues.clear();

    table = ConversationType;
    tableValues.append("1, Interactive");
    tableValues.append("2, Overhead");
    tableValues.append("3, Subsequent");
    tableValues.append("4, AI Bark");
    testData.insert(table, tableValues);
    tableValues.clear();

    table = EventType;
    tableValues.append("1, Speech");
    tableValues.append("2, Logic");
    tableValues.append("3, Comment");
    testData.insert(table, tableValues);
    tableValues.clear();

    foreach(Table table, testData.keys())
        foreach(const QString &value, testData.value(table))
            if (!insertTableRow(table, value.split(", ")))
                return false;

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
    QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
}
