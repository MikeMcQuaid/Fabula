#ifndef HIDECOLUMNSPROXYMODEL_H
#define HIDECOLUMNSPROXYMODEL_H

#include <QSortFilterProxyModel>

class HideColumnsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit HideColumnsProxyModel(QObject *parent = 0);
    void setHideColumns(const QList<int> &columns);
protected:
    bool filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const;
private:
    QList<int> m_hideColumns;
};

#endif // HIDECOLUMNSPROXYMODEL_H
