#ifndef SQLQUERYTREEPROXYMODEL_H
#define SQLQUERYTREEPROXYMODEL_H

#include <QList>
#include <QSortFilterProxyModel>

class TableToDuplicatedTreeProxyModel;
class RemoveFirstColumnDuplicatesProxyModel;
class HideColumnsProxyModel;

class TableToTreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit TableToTreeProxyModel(QObject *parent = 0);
    void setSourceModel(QAbstractItemModel *sourceModel);
    void setHideColumns(const QList<int> &columns);
private:
    QList<int> m_hideColumns;
    RemoveFirstColumnDuplicatesProxyModel *m_removeDuplicatesModel;
    TableToDuplicatedTreeProxyModel *m_tableToTreeModel;
    HideColumnsProxyModel *m_hideColumnsModel;
};

#endif // SQLQUERYTREEPROXYMODEL_H
