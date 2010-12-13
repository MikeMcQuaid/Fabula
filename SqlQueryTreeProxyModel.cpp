#include "sqlquerytreeproxymodel.h"

#include <QItemSelection>
#include <QSqlQueryModel>
#include <QDebug>
#include <QStringList>

SqlQueryTreeProxyModel::SqlQueryTreeProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    QSqlQueryModel *sourceModel = new QSqlQueryModel(this);
    sourceModel->setQuery("select characters.name, conversations.name from events "
                          "inner join characters on events.character_id = characters.id "
                          "inner join conversations on events.conversation_id = conversations.id");
    setSourceModel(sourceModel);
    setDynamicSortFilter(true);
}

QModelIndex	SqlQueryTreeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return index(sourceIndex.row(), 0);
}

QItemSelection SqlQueryTreeProxyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
{
    return QSortFilterProxyModel::mapSelectionFromSource(sourceSelection);
}

QItemSelection SqlQueryTreeProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
    return QSortFilterProxyModel::mapSelectionToSource(proxySelection);
}

QModelIndex	SqlQueryTreeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    QModelIndex parentIndex = proxyIndex.parent();
    if (parentIndex.isValid())
        return sourceModel()->index(parentIndex.row(), 1);
    return sourceModel()->index(proxyIndex.row(), 0);
}

QVariant SqlQueryTreeProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();

    QModelIndex parent = proxyIndex.parent();
    if (!parent.isValid())
        return QSortFilterProxyModel::data(proxyIndex, role);

    QModelIndex firstIndex = sourceModel()->index(0, 0);
    QVariant parentData = sourceModel()->data(parent, role);
    if (!parentData.isValid())
        return sourceModel()->data(proxyIndex, role);

    QModelIndexList indexes = sourceModel()->match(firstIndex, role, parentData, -1);
    QModelIndex parentIndex = indexes.at(proxyIndex.row());
    QModelIndex index = sourceModel()->index(parentIndex.row(), 1);

    return sourceModel()->data(index, role);
}

QModelIndex SqlQueryTreeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    quint32 internalId = parent.row() + 1;
    return createIndex(row, column, internalId);
}

QModelIndex SqlQueryTreeProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || !child.internalId())
        return QModelIndex();

    int parentRowId = child.internalId() - 1;
    return createIndex(parentRowId, 0);
}

int SqlQueryTreeProxyModel::rowCount(const QModelIndex &parent) const
{
    QModelIndex firstIndex = sourceModel()->index(0, 0);
    // TODO: Checking parent of parent is a nasty hack but it works for now.
    if (parent.parent().isValid())
        return 0;

    if (!parent.isValid()) {
        return sourceModel()->rowCount();
    }

    QVariant parentData = sourceModel()->data(parent);
    int matches = sourceModel()->match(firstIndex, Qt::DisplayRole, parentData, -1).size();
    return matches;
}

int SqlQueryTreeProxyModel::columnCount(const QModelIndex &) const
{
    return 1;
}

bool SqlQueryTreeProxyModel::hasChildren(const QModelIndex &parent) const
{
    // TODO: Horrible, horrible hack but works in the short term
    return !parent.parent().isValid();
}

void SqlQueryTreeProxyModel::reset()
{
    return QSortFilterProxyModel::reset();
}

bool SqlQueryTreeProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex firstIndex = sourceModel()->index(0, 0, sourceParent);
    QModelIndex rowIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    QVariant rowData = sourceModel()->data(rowIndex);
    QModelIndexList indexes = sourceModel()->match(firstIndex, Qt::DisplayRole, "", -1);
    foreach(const QModelIndex &index, indexes) {
        QVariant data = sourceModel()->data(index);
        if (index.row() == sourceRow)
            break;

        if (rowData == data) {
            qDebug() << "HERE" << data;
            return false;
        }
    }
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
