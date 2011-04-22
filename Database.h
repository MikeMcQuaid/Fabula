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
#include <QStringList>

class Database : public QObject
{
Q_OBJECT
public:
    explicit Database(const QString &path, QObject *parent = 0);
    ~Database();
    enum Table {
        Character,
        Writer,
        Conversation,
        ConversationEvent,
        Event,
        ConversationType,
        EventType
    };
    Q_ENUMS(Table)

    static QString tableName(Table table);
    static QMap<int, QSqlRelation> tableRelations(Table table);

private:
    bool create();
    bool insertTestData();
    bool insertTableRow(Table table, const QStringList& values);
    QSqlField createField(QString name, QVariant::Type type,
                          bool required = true, bool autoValue = false,
                          int length = 0);

};

#endif // DATABASE_H
