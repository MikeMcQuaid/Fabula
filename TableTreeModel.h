#ifndef TABLETREEMODEL_H
#define TABLETREEMODEL_H

#include <QAbstractItemModel>

class TreeItem
{
public:
    TreeItem(const QString &data, TreeItem *parent = 0);
    ~TreeItem();

    TreeItem *child(int row);
    void appendChild(TreeItem* child);
    int childCount() const;
    const QString &data() const;
    int row() const;
    TreeItem *parent();
    void setData(const QString &value);

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
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());

private:
    TreeItem *root;
};

#endif // TABLETREEMODEL_H
