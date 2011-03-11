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
    return index(sourceIndex.row(), 0);
}

QModelIndex	TableToDuplicatedTreeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    QModelIndex parentIndex = proxyIndex.parent();
    if (parentIndex.isValid())
        return sourceModel()->index(parentIndex.row(), 1);
    return sourceModel()->index(proxyIndex.row(), 0);
}

QVariant TableToDuplicatedTreeProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();

    QModelIndex parent = proxyIndex.parent();
    if (!parent.isValid())
        return QAbstractProxyModel::data(proxyIndex, role);

    QModelIndex firstIndex = sourceModel()->index(0, 0);
    QVariant parentData = sourceModel()->data(mapToSource(parent), role);
    if (!parentData.isValid())
        return sourceModel()->data(mapToSource(proxyIndex), role);

    QModelIndexList indexes = sourceModel()->match(firstIndex, role, parentData, -1);
    QModelIndex parentIndex = indexes.at(proxyIndex.row());
    QModelIndex index = sourceModel()->index(parentIndex.row(), 1);

    return sourceModel()->data(index, role);
}

QModelIndex TableToDuplicatedTreeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    quint32 internalId = parent.row() + 1;
    return createIndex(row, column, internalId);
}

QModelIndex TableToDuplicatedTreeProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || !child.internalId())
        return QModelIndex();

    int parentRowId = child.internalId() - 1;
    return createIndex(parentRowId, 0);
}

int TableToDuplicatedTreeProxyModel::rowCount(const QModelIndex &parent) const
{
    // TODO: Checking parent of parent is a nasty hack but it works for now.
    if (parent.parent().isValid())
        return 0;

    if (!parent.isValid()) {
        return sourceModel()->rowCount();
    }

    QVariant parentData = sourceModel()->data(mapToSource(parent));
    QModelIndex firstIndex = sourceModel()->index(0, 0);
    int matches = sourceModel()->match(firstIndex, Qt::DisplayRole, parentData, -1).size();
    return matches;
}

int TableToDuplicatedTreeProxyModel::columnCount(const QModelIndex &) const
{
    return 1;
}

bool TableToDuplicatedTreeProxyModel::hasChildren(const QModelIndex &parent) const
{
    // TODO: Horrible, horrible hack but works in the short term
    return !parent.parent().isValid();
}

struct TreeData {
    TreeData *parent;
    QList<TreeData*> children;
    QString text;
};

void TableToDuplicatedTreeProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    TreeData *root = new TreeData;
    for (int column=0; column < sourceModel->columnCount(); ++column) {
        for (int row=0; row < sourceModel->rowCount(); ++row) {
            TreeData *data = new TreeData;
            if (!row) {
                data->parent = root;
                data->parent->children.append(data);
            }
            data->text = sourceModel->data(sourceModel->index(row, column)).toString();
        }
    }

    QAbstractProxyModel::setSourceModel(sourceModel);
}

TableToTreeProxyModel::TableToTreeProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent),
      m_tableToTreeModel(new TableToDuplicatedTreeProxyModel(m_hideColumnsModel)),
      m_hideColumnsModel(new HideColumnsProxyModel(m_tableToTreeModel))
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
