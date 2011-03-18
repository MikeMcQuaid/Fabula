#include "HideColumnsProxyModel.h"

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
