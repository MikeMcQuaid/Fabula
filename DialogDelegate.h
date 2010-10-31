#ifndef EVENTDELEGATE_H
#define EVENTDELEGATE_H

#include <QItemDelegate>

class QSqlRelationalTableModel;

class DialogDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit DialogDelegate(QObject *parent = 0);
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    QAbstractItemModel *m_model;
};

#endif // EVENTDELEGATE_H
