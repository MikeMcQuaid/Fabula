#include "TableTreeModel.h"

#include <QStringList>
#include <QDebug>
#include <QSqlQuery>
#include <QMultiMap>

TreeItem::TreeItem(const QString &data, TreeItem *parent)
{
    m_data = data;
    m_parent = parent;
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

TreeItem *TreeItem::parent()
{
    return m_parent;
}

void TreeItem::setData(const QString &data)
{
    m_data = data;
}

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    root = new TreeItem("Conversations");

    QSqlQuery query;
    QMultiMap<QString, QString> charactersConversations;

    query.exec("select characters.name, conversations.name from events "
               "inner join characters on events.character_id = characters.id "
               "inner join conversations on events.conversation_id = conversations.id");
    while (query.next()) {
        const QString characterName = query.value(0).toString();
        const QString conversationName = query.value(1).toString();
        charactersConversations.insert(characterName, conversationName);
    }

    foreach(const QString& characterName, charactersConversations.keys()) {
        TreeItem* character = new TreeItem(characterName, root);
        root->appendChild(character);
        foreach(const QString& conversationName, charactersConversations.values(characterName)) {
            TreeItem* conversation = new TreeItem(conversationName, character);
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

    // TODO: Actually write to the SQL database
    emit dataChanged(index, index);
    return true;
}

bool TreeModel::insertRow(int, const QModelIndex &parent)
{
    TreeItem *parentItem = static_cast<TreeItem*>(parent.internalPointer());
    if (!parentItem)
        return false;

    beginInsertRows(parent, 0, parentItem->childCount());
    parentItem->appendChild(new TreeItem("New Item", parentItem));
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

