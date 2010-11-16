#include "sqlquerytreeproxymodel.h"

#include <QItemSelection>
#include <QSqlQueryModel>

SqlQueryTreeProxyModel::SqlQueryTreeProxyModel(QObject *parent) :
    QAbstractProxyModel(parent)
{
    QSqlQueryModel *sourceModel = new QSqlQueryModel(this);
    sourceModel->setQuery("select characters.name, conversations.name from events "
                          "inner join characters on events.character_id = characters.id "
                          "inner join conversations on events.conversation_id = conversations.id");
    setSourceModel(sourceModel);
}

QModelIndex	SqlQueryTreeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return index(sourceIndex.row(), sourceIndex.column());
}

QItemSelection SqlQueryTreeProxyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
{
    return QAbstractProxyModel::mapSelectionFromSource(sourceSelection);
}

QItemSelection SqlQueryTreeProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
    return QAbstractProxyModel::mapSelectionToSource(proxySelection);
}

QModelIndex	SqlQueryTreeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return sourceModel()->index(proxyIndex.row(), proxyIndex.column(), proxyIndex.parent());
}

QVariant SqlQueryTreeProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();

    return QAbstractProxyModel::data(proxyIndex, role);
}

QModelIndex SqlQueryTreeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!sourceModel()->hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex SqlQueryTreeProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();


    return QModelIndex();
}

int SqlQueryTreeProxyModel::rowCount(const QModelIndex &parent) const
{
    return sourceModel()->rowCount(mapToSource(parent));
}

int SqlQueryTreeProxyModel::columnCount(const QModelIndex &parent) const
{
    return sourceModel()->columnCount(mapToSource(parent));
}

void SqlQueryTreeProxyModel::reset()
{
    return QAbstractProxyModel::reset();
}
