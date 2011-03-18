#include "TableToTreeProxyModel.h"

#include <QItemSelection>
#include <QSqlQueryModel>
#include <QDebug>
#include <QStringList>

class HideColumnsProxyModel : public QSortFilterProxyModel
{
public:
    explicit HideColumnsProxyModel(QObject *parent = 0);
    void setHideColumns(const QList<int> &columns);
protected:
    bool filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const;
private:
    QList<int> m_hideColumns;
};

class TreeNode;

class TableToDuplicatedTreeProxyModel : public QAbstractProxyModel
{
public:
    explicit TableToDuplicatedTreeProxyModel(QObject *parent = 0);
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    void setSourceModel(QAbstractItemModel *sourceModel);
private:
    TreeNode *rootNode;
    QList<QList<TreeNode*> > tableNodes;
};

struct TreeNode {
    TreeNode(int row=-1, int column=-1, TreeNode *parent=0)
        : row(row), column(column), parent(parent)
    {
        if (parent)
            parent->children.append(this);
    };
    int row;
    int column;
    TreeNode *parent;
    QList<TreeNode*> children;
};

HideColumnsProxyModel::HideColumnsProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void HideColumnsProxyModel::setHideColumns(const QList<int> &columns)
{
    m_hideColumns = columns;
}

bool HideColumnsProxyModel::filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const
{
    if (m_hideColumns.contains(sourceColumn))
        return false;

    return QSortFilterProxyModel::filterAcceptsColumn(sourceColumn, sourceParent);
}

TableToDuplicatedTreeProxyModel::TableToDuplicatedTreeProxyModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
}

QModelIndex	TableToDuplicatedTreeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    TreeNode *node = tableNodes.at(sourceIndex.column()).at(sourceIndex.row());
    Q_ASSERT(node);
    if (!node)
        return QModelIndex();

    Q_ASSERT(false);
    return index(sourceIndex.row(), 0);
}

QModelIndex	TableToDuplicatedTreeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();

    const TreeNode *node = static_cast<TreeNode*>(proxyIndex.internalPointer());
    Q_ASSERT(node);
    if (!node)
        return QModelIndex();

    return sourceModel()->index(node->row, node->column);
}

QVariant TableToDuplicatedTreeProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();

    const TreeNode *node = static_cast<TreeNode*>(proxyIndex.internalPointer());
    Q_ASSERT(node);
    if (!node)
        return QVariant();

    const QModelIndex index = sourceModel()->index(node->row, node->column);
    return sourceModel()->data(index, role);
}

QModelIndex TableToDuplicatedTreeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    const TreeNode *parentNode = static_cast<TreeNode*>(parent.internalPointer());
    if (!parentNode)
        parentNode = rootNode;

    TreeNode *node = parentNode->children.at(row);
    Q_ASSERT(node);
    if (!node)
        return QModelIndex();

    return createIndex(row, column, node);
}

QModelIndex TableToDuplicatedTreeProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    const TreeNode *childNode = static_cast<TreeNode*>(child.internalPointer());
    Q_ASSERT(childNode);
    if (!childNode)
        return QModelIndex();

    TreeNode *parentNode = childNode->parent;
    Q_ASSERT(parentNode);
    if (!parentNode)
        return QModelIndex();

    return createIndex(parentNode->row, parentNode->column, parentNode);
}

int TableToDuplicatedTreeProxyModel::rowCount(const QModelIndex &parent) const
{
    const TreeNode *node = static_cast<TreeNode*>(parent.internalPointer());
    if (!node)
        node = rootNode;

    return node->children.size();
}

int TableToDuplicatedTreeProxyModel::columnCount(const QModelIndex &) const
{
    return 1;
}

bool TableToDuplicatedTreeProxyModel::hasChildren(const QModelIndex &parent) const
{
    const TreeNode *node = static_cast<TreeNode*>(parent.internalPointer());
    if (!node)
        node = rootNode;

    return node->children.size() > 0;
}

void TableToDuplicatedTreeProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    // TODO: cleanup on already existing
    tableNodes.clear();
    rootNode = new TreeNode;

    for (int row=0; row < sourceModel->rowCount(); ++row) {
        QList<TreeNode*> rowNodes;
        TreeNode *previousNode = rootNode;
        for (int column=0; column < sourceModel->columnCount(); ++column) {
            TreeNode *node = new TreeNode(row, column, previousNode);
            //node->text = sourceModel->data(sourceModel->index(row, column)).toString();
            rowNodes.insert(column, node);
            previousNode = node;
        }
        tableNodes.insert(row, rowNodes);
    }

    QAbstractProxyModel::setSourceModel(sourceModel);
}

TableToTreeProxyModel::TableToTreeProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent),
      m_tableToTreeModel(new TableToDuplicatedTreeProxyModel(this)),
      m_hideColumnsModel(new HideColumnsProxyModel(this))
{
}

void TableToTreeProxyModel::setHideColumns(const QList<int> &columns)
{
    m_hideColumns = columns;
}

void TableToTreeProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    m_hideColumnsModel->setSourceModel(sourceModel);
    m_hideColumnsModel->setHideColumns(m_hideColumns);
    m_tableToTreeModel->setSourceModel(m_hideColumnsModel);
    QSortFilterProxyModel::setSourceModel(m_tableToTreeModel);
}
