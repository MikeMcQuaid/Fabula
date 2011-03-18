#ifndef SQLQUERYTREEPROXYMODEL_H
#define SQLQUERYTREEPROXYMODEL_H

#include <QList>
#include <QAbstractProxyModel>

class TreeNode;

class TableToTreeProxyModel : public QAbstractProxyModel
{
public:
    explicit TableToTreeProxyModel(QObject *parent = 0);
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

#endif // SQLQUERYTREEPROXYMODEL_H
