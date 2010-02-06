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
                             "id int not null primary key, "
                             "name varchar(20)");

    databaseStructure.insert("writers",
                             "id int not null primary key, "
                             "name varchar(20)");

    databaseStructure.insert("writers",
                             "id int not null primary key, "
                             "name varchar(20)");

    databaseStructure.insert("conversations",
                             "id int not null primary key, "
                             "type int not null, "
                             "writer_id int not null, "
                             "name varchar(20)");

    databaseStructure.insert("conversations_events",
                             "id int not null primary key, "
                             "conversation_id int not null, "
                             "event_id int not null, "
                             "sort int not null");

    databaseStructure.insert("events",
                             "id int not null primary key, "
                             "conversation_id int not null, "
                             "character_id int, "
                             "audiofile_id int, "
                             "text varchar(200)");

    databaseStructure.insert("audiofiles",
                             "id int not null primary key, "
                             "url varchar(50)");

    databaseStructure.insert("conversation_types",
                             "id int not null primary key, "
                             "name varchar(20)");

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
    sqlQuery.exec("insert into writers(id, name) values (1, 'Writer')");
    sqlQuery.exec("insert into writers(id, name) values (2, 'Writer2')");
    sqlQuery.exec("insert into conversations(id, type, writer_id, name) values (1, 1, 1, 'Conversation')");
    sqlQuery.exec("insert into conversations(id, type, writer_id, name) values (2, 2, 2, 'Conversation2')");
    sqlQuery.exec("insert into conversations_events(id, conversation_id, event_id, sort) values (1, 1, 1, 1)");
    sqlQuery.exec("insert into conversations_events(id, conversation_id, event_id, sort) values (2, 2, 2, 2)");
    sqlQuery.exec("insert into events(id, conversation_id, character_id, audiofile_id, text) values (1, 1, 1, 1, 'Event')");
    sqlQuery.exec("insert into events(id, conversation_id, character_id, audiofile_id, text) values (2, 2, 2, 2, 'Event2')");
    sqlQuery.exec("insert into audiofiles(id, url) values (1, 'AudioFile.mp4')");
    sqlQuery.exec("insert into audiofiles(id, url) values (2, 'AudioFile2.mp4')");
    sqlQuery.exec("insert into conversation_types(id, name) values (1, 'ConversationType')");
    sqlQuery.exec("insert into conversation_types(id, name) values (2, 'ConversationType2')");

    return true;
}

Database::~Database()
{
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}
