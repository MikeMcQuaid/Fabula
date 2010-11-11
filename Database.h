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

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QMap>
#include <QSqlRelation>

static const QLatin1String CharactersTable("characters");
static const QLatin1String WritersTable("writers");
static const QLatin1String ConversationsTable("conversations");
static const QLatin1String ConversationsEventsTable("conversations_events");
static const QLatin1String EventsTable("events");
static const QLatin1String AudiofilesTable("audiofiles");
static const QLatin1String ConversationTypesTable("conversation_types");
static const QLatin1String EventTypesTable("event_types");

class Database : public QObject
{
Q_OBJECT
public:
    explicit Database(const QString &path, QObject *parent = 0);
    ~Database();
    QMap<int, QSqlRelation> tableRelations(const QLatin1String &table) const;

private:
    bool create();
    bool insertDummyData();
    void setupRelations();
    QMap<QLatin1String, QMap<int, QSqlRelation> > m_tableRelations;
};

#endif // DATABASE_H
