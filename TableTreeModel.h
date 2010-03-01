#ifndef TABLETREEMODEL_H
#define TABLETREEMODEL_H

#include <QAbstractItemModel>

class TreeItem
{
public:
    TreeItem(const QString &data, TreeItem *parent = 0);
    ~TreeItem();

    TreeItem *child(int row);
    int childCount() const;
    const QString &data() const;
    int row() const;
    TreeItem *parent();
    void setValue();

private:
    QList<TreeItem*> m_children;
    QString m_data;
    TreeItem *m_parent;
};

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
    TreeItem *root;
};

#endif // TABLETREEMODEL_H
