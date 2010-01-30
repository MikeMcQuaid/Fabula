#include "TreeTableProxyModel.h"

// http://qt.gitorious.org/+qt-developers/qt/lighthouse/blobs/lighthouse/demos/browser/history.h
// http://qt.gitorious.org/+qt-developers/qt/lighthouse/blobs/lighthouse/demos/browser/history.cpp
// HistoryFilterModel

#include <QItemSelection>

TreeTableProxyModel::TreeTableProxyModel(QAbstractItemModel *sourceModel, QObject *parent) :
    QAbstractProxyModel(parent)
{
    setSourceModel(sourceModel);
}

QModelIndex TreeTableProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return sourceModel()->index(sourceIndex.row(), sourceIndex.column());
}

QModelIndex TreeTableProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return sourceModel()->index(proxyIndex.row(), proxyIndex.column());
}

void TreeTableProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);
}

QAbstractItemModel *TreeTableProxyModel::sourceModel () const
{
    return QAbstractProxyModel::sourceModel();
}

bool TreeTableProxyModel::hasChildren(const QModelIndex &parent) const
{
    return sourceModel()->hasChildren(parent);
}

QModelIndex TreeTableProxyModel::parent(const QModelIndex &index) const
{
    return sourceModel()->parent(index);
}

QModelIndex TreeTableProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    return sourceModel()->index(row, column, parent);
}

int TreeTableProxyModel::rowCount(const QModelIndex &parent) const
{
    return sourceModel()->rowCount(parent);
}

int TreeTableProxyModel::columnCount(const QModelIndex &parent) const
{
    return sourceModel()->columnCount(parent);
}
