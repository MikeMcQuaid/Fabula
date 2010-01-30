#ifndef TREETABLEPROXYMODEL_H
#define TREETABLEPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QAbstractItemModel>

class TreeTableProxyModel : public QAbstractProxyModel
{
Q_OBJECT
public:
    explicit TreeTableProxyModel(QAbstractItemModel *sourceModel, QObject *parent = 0);
    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    virtual void setSourceModel(QAbstractItemModel *sourceModel);
    QAbstractItemModel *sourceModel () const;
    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

signals:

public slots:

};

#endif // TREETABLEPROXYMODEL_H
