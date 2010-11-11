#include "DialogDelegate.h"

#include "ConversationDialog.h"
#include "EventDialog.h"
#include "SqlTreeModel.h"
#include "SqlRelationalTableDialog.h"
#include "Database.h"

#include <QDebug>
#include <QItemEditorFactory>
#include <QSqlRelationalTableModel>

DialogDelegate::DialogDelegate(QObject *parent) :
    QItemDelegate(parent), m_model(0)
{
}

QWidget* DialogDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const {
    const QSqlRelationalTableModel *model = qobject_cast<const QSqlRelationalTableModel*>(index.model());
    Q_ASSERT(model);
    if (!model)
        return 0;

    SqlRelationalTableDialog *dialog = 0;
    if (model->tableName() == ConversationsTable)
        dialog = new ConversationDialog(parent);
    else if (model->tableName() == EventsTable)
        dialog = new EventDialog(parent);

    Q_ASSERT(dialog);
    if (!dialog)
        return 0;

    dialog->setWindowModality(Qt::WindowModal);
    return dialog;
}

void DialogDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    SqlRelationalTableDialog *dialog = qobject_cast<SqlRelationalTableDialog*>(editor);
    Q_ASSERT(dialog);
    if (!dialog)
        return;

    Q_ASSERT(m_model);
    if (!m_model)
        return;

    dialog->setModelRow(m_model, index.row());
}

void DialogDelegate::setModelData(QWidget *editor, QAbstractItemModel*, const QModelIndex &) const {
    SqlRelationalTableDialog *dialog = qobject_cast<SqlRelationalTableDialog*>(editor);
    Q_ASSERT(dialog);
    if (dialog && dialog->result() == QDialog::Accepted)
        dialog->writeToModel();
}

void DialogDelegate::updateEditorGeometry(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const {
    // Don't do anything, we're modal and sharing the same widget.
}

bool DialogDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    QSqlRelationalTableModel *sqlModel = qobject_cast<QSqlRelationalTableModel*>(model);
    Q_ASSERT(sqlModel);
    if (sqlModel)
        m_model = sqlModel;

    return QItemDelegate::editorEvent(event, model, option, index);
}
