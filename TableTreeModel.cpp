/*  This file is part of Fabula.
    Copyright (C) 2010 Mike Arthur <mike@mikearthur.co.uk>

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

#include "TableTreeModel.h"

#include <QStringList>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMultiMap>

SqlTreeItem::SqlTreeItem(const QString &data, const QString &table, SqlTreeItem *parent, qint64 id)
    : m_table(table), m_data(data), m_parent(parent), m_id(id), m_dirty(false)
{
}

SqlTreeItem::~SqlTreeItem()
{
    if (m_parent)
        m_parent->m_children.removeAll(this);
    qDeleteAll(m_children);
}

SqlTreeItem *SqlTreeItem::child(int row)
{
    return m_children.value(row);
}

void SqlTreeItem::appendChild(SqlTreeItem* child)
{
    m_children.append(child);
}

int SqlTreeItem::childCount() const
{
    return m_children.count();
}

int SqlTreeItem::row() const
{
    if (m_parent)
        return m_parent->m_children.indexOf(const_cast<SqlTreeItem*>(this));

    return 0;
}

const QString &SqlTreeItem::table() const
{
    return m_table;
}

const QString &SqlTreeItem::data() const
{
    return m_data;
}

qint64 SqlTreeItem::id() const
{
    return m_id;
}

void SqlTreeItem::setId(qint64 id)
{
    m_id = id;
}

SqlTreeItem *SqlTreeItem::parent()
{
    return m_parent;
}

void SqlTreeItem::setData(const QString &data)
{
    m_data = data;
    m_dirty = true;
}

bool SqlTreeItem::dirty() const
{
    return m_dirty;
}

void SqlTreeItem::setDirty(bool dirty)
{
    m_dirty = dirty;
}

const QList<SqlTreeItem*>& SqlTreeItem::children() const
{
    return m_children;
}

SqlTreeModel::SqlTreeModel(QObject *parent)
    : QAbstractItemModel(parent), m_rootItem(0)
{
    loadData();
}

void SqlTreeModel::reset()
{
    beginResetModel();
    tables.clear();
    delete m_rootItem;
    loadData();
    endResetModel();
}

void SqlTreeModel::loadData()
{
    tables.append("characters");
    tables.append("conversations");

    m_rootItem = new SqlTreeItem("Conversations");

    QSqlQuery query;
    QMultiMap<QString, QString> charactersConversations;
    QMap<QString, qint64> charactersIds;
    QMap<QString, qint64> conversationsIds;

    // TODO: Make this generic and recursive
    query.exec("select characters.id, conversations.id, characters.name, conversations.name from events "
               "inner join characters on events.character_id = characters.id "
               "inner join conversations on events.conversation_id = conversations.id");
    // TODO: Use indexOf("fieldname")
    while (query.next()) {
        const qint64 characterId = query.value(0).toLongLong();
        const qint64 conversationId = query.value(1).toLongLong();
        const QString characterName = query.value(2).toString();
        const QString conversationName = query.value(3).toString();
        charactersConversations.insert(characterName, conversationName);
        charactersIds.insert(characterName, characterId);
        conversationsIds.insert(conversationName, conversationId);
    }

    foreach(const QString& characterName, charactersConversations.keys()) {
        const qint64 characterId = charactersIds.value(characterName);
        SqlTreeItem* character = new SqlTreeItem(characterName, "characters", m_rootItem, characterId);
        m_rootItem->appendChild(character);
        foreach(const QString& conversationName, charactersConversations.values(characterName)) {
            const qint64 conversationId = conversationsIds.value(conversationName);
            SqlTreeItem* conversation = new SqlTreeItem(conversationName, "conversations", character, conversationId);
            character->appendChild(conversation);
        }
    }
}

SqlTreeModel::~SqlTreeModel()
{
    delete m_rootItem;
}

QModelIndex SqlTreeModel::index(int row, int column, const QModelIndex &parent)
        const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    SqlTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<SqlTreeItem*>(parent.internalPointer());

    SqlTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex SqlTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    SqlTreeItem *childItem = static_cast<SqlTreeItem*>(index.internalPointer());
    SqlTreeItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SqlTreeModel::rowCount(const QModelIndex &parent) const
{
    SqlTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<SqlTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int SqlTreeModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant SqlTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    SqlTreeItem *item = static_cast<SqlTreeItem*>(index.internalPointer());

    return item->data();
}

bool SqlTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role != Qt::EditRole)
        return false;

    if (value.toString().isEmpty())
        return false;

    SqlTreeItem *item = static_cast<SqlTreeItem*>(index.internalPointer());
    const int id = item->id();

    if (id != SqlTreeItem::INVALID_ID) {
        foreach (SqlTreeItem* characterItem, m_rootItem->children()) {
            if (id == characterItem->id())
                characterItem->setData(value.toString());
            foreach (SqlTreeItem* conversationItem, characterItem->children()) {
                if (id == conversationItem->id())
                    conversationItem->setData(value.toString());
            }
        }
    }
    else
        item->setData(value.toString());

    emit dataChanged(index, index);
    return true;
}

bool SqlTreeModel::insertRow(int, const QModelIndex &parent)
{
    SqlTreeItem *parentItem = static_cast<SqlTreeItem*>(parent.internalPointer());
    if (!parentItem)
        parentItem = m_rootItem;

    beginInsertRows(parent, 0, parentItem->childCount());
    const QString newItemTable = getChildTable(parentItem->table());

    SqlTreeItem* newItem = new SqlTreeItem("New Item", newItemTable, parentItem);
    newItem->setDirty(true);
    parentItem->appendChild(newItem);
    endInsertRows();
    return true;
}

QString SqlTreeModel::getChildTable(const QString& parentTable)
{
    QString previousTable;
    foreach(const QString& table, tables) {
        if (parentTable == previousTable)
            return table;
        previousTable = table;
    }
    return QString();
}


Qt::ItemFlags SqlTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant SqlTreeModel::headerData(int, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootItem->data();

    return QVariant();
}

bool SqlTreeModel::submit()
{
    QList<SqlTreeItem*> newCharacters;
    QList<SqlTreeItem*> newConversations;
    QList<SqlTreeItem*> updatedCharacters;
    QList<SqlTreeItem*> updatedConversations;

    foreach (SqlTreeItem* characterItem, m_rootItem->children()) {
        if (characterItem->data() == "New Item")
            continue;

        if (characterItem->dirty()) {
            if (characterItem->id() == SqlTreeItem::INVALID_ID)
                newCharacters.append(characterItem);
            else
                updatedCharacters.append(characterItem);
        }
        foreach (SqlTreeItem* conversationItem, characterItem->children()) {
            if (conversationItem->data() == "New Item")
                continue;

            if (conversationItem->dirty()) {
                if (conversationItem->id() == SqlTreeItem::INVALID_ID)
                    newConversations.append(conversationItem);
                else
                    updatedConversations.append(conversationItem);
            }
        }
    }

    QSqlQuery query;
    bool querySuccess = false;

    foreach (SqlTreeItem* newCharacter, newCharacters) {
        query.prepare("insert into characters(name) values(:name)");
        query.bindValue(":name", newCharacter->data());
        querySuccess = query.exec();
        if (querySuccess) {
            newCharacter->setId(query.lastInsertId().toInt());
            newCharacter->setDirty(false);
        }
        else
            qWarning() << query.lastQuery() << query.lastError();
    }

    foreach (SqlTreeItem* newConversation, newConversations) {
        query.prepare("insert into conversations(name) values(:name)");
        query.bindValue(":name", newConversation->data());
        querySuccess = query.exec();
        if (querySuccess) {
            newConversation->setId(query.lastInsertId().toInt());
            newConversation->setDirty(false);
        }
        else
            qWarning() << query.lastQuery() << query.lastError();
    }

    // TODO: Check we update at least one row

    foreach (SqlTreeItem* updatedCharacter, updatedCharacters) {
        query.prepare("update characters set name=:name where id=:id");
        query.bindValue(":name", updatedCharacter->data());
        query.bindValue(":id", updatedCharacter->id());
        querySuccess = query.exec();
        if (querySuccess)
            updatedCharacter->setDirty(false);
        else
            qWarning() << query.lastQuery() << query.lastError();
    }

    foreach (SqlTreeItem* updatedConversation, updatedConversations) {
        query.prepare("update conversations set name=:name where id=:id");
        query.bindValue(":name", updatedConversation->data());
        query.bindValue(":id", updatedConversation->id());
        querySuccess = query.exec();
        if (querySuccess)
            updatedConversation->setDirty(false);
        else
            qWarning() << query.lastQuery() << query.lastError();
    }

    emit submitted();

    return true;
}

