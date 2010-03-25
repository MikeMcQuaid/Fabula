#include "TableTreeModel.h"

#include <QStringList>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMultiMap>

TreeItem::TreeItem(const QString &data, TreeItem *parent, qint64 id)
    : m_data(data), m_parent(parent), m_id(id), m_dirty(false)
{
}

TreeItem::~TreeItem()
{
    if (m_parent)
        m_parent->m_children.removeAll(this);
    qDeleteAll(m_children);
}

TreeItem *TreeItem::child(int row)
{
    return m_children.value(row);
}

void TreeItem::appendChild(TreeItem* child)
{
    m_children.append(child);
}

int TreeItem::childCount() const
{
    return m_children.count();
}

int TreeItem::row() const
{
    if (m_parent)
        return m_parent->m_children.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

const QString &TreeItem::data() const
{
    return m_data;
}

qint64 TreeItem::id() const
{
    return m_id;
}

void TreeItem::setId(qint64 id)
{
    m_id = id;
}

TreeItem *TreeItem::parent()
{
    return m_parent;
}

void TreeItem::setData(const QString &data)
{
    m_data = data;
    m_dirty = true;
}

bool TreeItem::dirty() const
{
    return m_dirty;
}

void TreeItem::setDirty(bool dirty)
{
    m_dirty = dirty;
}

const QList<TreeItem*>& TreeItem::children() const
{
    return m_children;
}

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    root = new TreeItem("Conversations");

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
        TreeItem* character = new TreeItem(characterName, root, characterId);
        root->appendChild(character);
        foreach(const QString& conversationName, charactersConversations.values(characterName)) {
            const qint64 conversationId = conversationsIds.value(conversationName);
            TreeItem* conversation = new TreeItem(conversationName, character, conversationId);
            character->appendChild(conversation);
        }
    }
}

TreeModel::~TreeModel()
{
    delete root;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
        const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == root)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex&) const
{
    return 1;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << value.toString();
    if (!index.isValid())
        return false;

    if (role != Qt::EditRole)
        return false;

    if (value.toString().isEmpty())
        return false;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    item->setData(value.toString());

    emit dataChanged(index, index);
    return true;
}

bool TreeModel::insertRow(int, const QModelIndex &parent)
{
    TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
    if (!parentItem)
        parentItem = root;

    beginInsertRows(parent, 0, parentItem->childCount());
    TreeItem* newItem = new TreeItem("New Item", parentItem);
    newItem->setDirty(true);
    parentItem->appendChild(newItem);
    endInsertRows();
    return true;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant TreeModel::headerData(int, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return root->data();

    return QVariant();
}

bool TreeModel::submit()
{
    QList<TreeItem*> newCharacters;
    QList<TreeItem*> newConversations;
    QList<TreeItem*> updatedCharacters;
    QList<TreeItem*> updatedConversations;

    foreach (TreeItem* characterItem, root->children()) {
        if (characterItem->data() == "New Item")
            continue;

        if (characterItem->dirty()) {
            qDebug() << "DIRTYCHARACTER:" << characterItem->data();
            if (characterItem->id() == TreeItem::INVALID_ID)
                newCharacters.append(characterItem);
            else
                updatedCharacters.append(characterItem);
        }
        foreach (TreeItem* conversationItem, characterItem->children()) {
            if (conversationItem->data() == "New Item")
                continue;

            if (conversationItem->dirty()) {
                qDebug() << "DIRTYCONVERSATION:" << conversationItem->data();
                if (conversationItem->id() == TreeItem::INVALID_ID)
                    newConversations.append(conversationItem);
                else
                    updatedConversations.append(conversationItem);
            }
        }
    }

    QSqlQuery query;
    bool querySuccess = false;

    foreach (TreeItem* newCharacter, newCharacters) {
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

    foreach (TreeItem* newConversation, newConversations) {
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

    foreach (TreeItem* updatedCharacter, updatedCharacters) {
        qDebug() << "UPDATING:" << updatedCharacter->data();
        query.prepare("update characters set name=:name where id=:id");
        query.bindValue(":name", updatedCharacter->data());
        query.bindValue(":id", updatedCharacter->id());
        querySuccess = query.exec();
        qDebug() << query.executedQuery() << updatedCharacter->data() << updatedCharacter->id();
        if (querySuccess)
            updatedCharacter->setDirty(false);
        else
            qWarning() << query.lastQuery() << query.lastError();
    }

    foreach (TreeItem* updatedConversation, updatedConversations) {
        qDebug() << "UPDATING:" << updatedConversation->data();
        query.prepare("update conversations set name=:name where id=:id");
        query.bindValue(":name", updatedConversation->data());
        query.bindValue(":id", updatedConversation->id());
        querySuccess = query.exec();
        qDebug() << query.executedQuery();
        if (querySuccess)
            updatedConversation->setDirty(false);
        else
            qWarning() << query.lastQuery() << query.lastError();
    }

    return true;
}

