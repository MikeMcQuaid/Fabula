#ifndef TABLETREEMODEL_H
#define TABLETREEMODEL_H

#include <QAbstractItemModel>

class TreeItem
{
public:
    static const qint64 INVALID_ID = -1;

    TreeItem(const QString &data, TreeItem *parent = 0, qint64 id = INVALID_ID);
    ~TreeItem();

    TreeItem *child(int row);
    void appendChild(TreeItem* child);
    int childCount() const;
    const QString &data() const;
    int row() const;
    TreeItem *parent();
    void setData(const QString &value);
    qint64 id() const;
    void setId(qint64 id);
    bool dirty() const;
    void setDirty(bool dirty);
    const QList<TreeItem*>& children() const;

private:
    QList<TreeItem*> m_children;
    QString m_data;
    TreeItem *m_parent;
    int m_id;
    bool m_dirty;
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
    bool submit();

private:
    TreeItem *root;
};

#endif // TABLETREEMODEL_H
