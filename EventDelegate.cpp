#include "EventDelegate.h"

#include "EventDialog.cpp"

#include <QItemEditorFactory>
#include <QDebug>

EventDelegate::EventDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget* EventDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
    EventDialog *dialog = new EventDialog(parent);
    dialog->setWindowModality(Qt::WindowModal);
    return dialog;
}

void EventDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    EventDialog *dialog = qobject_cast<EventDialog*>(editor);
    Q_ASSERT(dialog);
    if (!dialog)
        return;

    QSqlRelationalTableModel *model = qobject_cast<QSqlRelationalTableModel*>(m_model);
    Q_ASSERT(model);
    if (!model)
        return;

    dialog->setModelRow(m_model, index.row());
}

void EventDelegate::setModelData(QWidget *editor, QAbstractItemModel*, const QModelIndex &) const {
    EventDialog *dialog = qobject_cast<EventDialog*>(editor);
    Q_ASSERT(dialog);
    if (dialog)
        dialog->writeToModel();
}

void EventDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem&, const QModelIndex&) const {
    Q_ASSERT(qobject_cast<EventDialog*>(editor));
    // Don't do anything, we're modal and sharing the same widget.
}

bool EventDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    QSqlRelationalTableModel *sqlModel = qobject_cast<QSqlRelationalTableModel*>(model);
    Q_ASSERT(sqlModel);
    if (sqlModel)
        m_model = sqlModel;

    return QItemDelegate::editorEvent(event, model, option, index);
}

