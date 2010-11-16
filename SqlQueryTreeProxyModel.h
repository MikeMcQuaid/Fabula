#ifndef SQLQUERYTREEPROXYMODEL_H
#define SQLQUERYTREEPROXYMODEL_H

#include <QAbstractProxyModel>

class SqlQueryTreeProxyModel : public QAbstractProxyModel
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
    void reset();
};

#endif // SQLQUERYTREEPROXYMODEL_H
