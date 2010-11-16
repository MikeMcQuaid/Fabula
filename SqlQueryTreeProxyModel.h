#ifndef SQLQUERYTREEPROXYMODEL_H
#define SQLQUERYTREEPROXYMODEL_H

#include <QSortFilterProxyModel>

class SqlQueryTreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SqlQueryTreeProxyModel(QObject *parent = 0);
    QModelIndex	mapFromSource(const QModelIndex &sourceIndex) const;
    QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const;
    QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const;
    QModelIndex	mapToSource(const QModelIndex &proxyIndex) const;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    void reset();
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif // SQLQUERYTREEPROXYMODEL_H
