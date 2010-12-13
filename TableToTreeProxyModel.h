#ifndef SQLQUERYTREEPROXYMODEL_H
#define SQLQUERYTREEPROXYMODEL_H

#include <QSortFilterProxyModel>

class TableToDuplicatedTreeProxyModel;
class RemoveFirstColumnDuplicatesProxyModel;

class TableToTreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit TableToTreeProxyModel(QObject *parent = 0);
    void setSourceModel(QAbstractItemModel *sourceModel);
    void reset();
private:
    RemoveFirstColumnDuplicatesProxyModel *m_removeDuplicatesModel;
    TableToDuplicatedTreeProxyModel *m_tableToTreeModel;
};

#endif // SQLQUERYTREEPROXYMODEL_H
