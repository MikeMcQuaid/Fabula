/*  This file is part of Fabula.
    Copyright (C) 2010 Mike McQuaid <mike@mikemcquaid.com>

    Fabula is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Fabula is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Fabula. If not, see <http://www.gnu.org/licenses/>.
*/

#include "TwoRowDelegate.h"

#include <Qt>
#include <QObject>
#include <QPainter>
#include <QModelIndex>
#include <QSize>
#include <QStyleOptionViewItemV4>
#include <QTableView>
#include <QMouseEvent>
#include <QRect>
#include <QDebug>

TwoRowDelegate::TwoRowDelegate(int secondRowColumn, QAbstractItemView *view, QObject *parent) :
    QSqlRelationalDelegate(parent), m_column(secondRowColumn), m_view(view)
{
    QTableView *tableView = qobject_cast<QTableView*>(view);
    if (tableView)
        tableView->hideColumn(m_column);
}

void TwoRowDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == m_column)
        return;

    QSize firstRowSize(firstRowSizeHint(option, index));

    if (index.column() == 1) {
        QStyleOptionViewItemV4 secondRowOption(option);
        secondRowOption.rect.translate(0, firstRowSize.height());
        secondRowOption.rect.setHeight(secondRowHeight(option, index));
        secondRowOption.rect.setWidth(m_view->width());
        QSqlRelationalDelegate::paint(painter, secondRowOption, secondRowIndex(index));
    }

    QStyleOptionViewItemV4 firstRowOption(option);
    firstRowOption.rect.setSize(firstRowSize);
    QSqlRelationalDelegate::paint(painter, firstRowOption, index);
}

QSize TwoRowDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize bothRowsSize = firstRowSizeHint(option, index);
    bothRowsSize.rheight() += secondRowHeight(option, index);
    return bothRowsSize;
}

QSize TwoRowDelegate::firstRowSizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSqlRelationalDelegate::sizeHint(option, index);
}

int TwoRowDelegate::secondRowHeight(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 secondRowOption(option);
    secondRowOption.rect.setHeight(m_view->height());
    return secondRowOption.fontMetrics.boundingRect(secondRowOption.rect, Qt::TextWordWrap, index.data().toString()).height();
}

QModelIndex TwoRowDelegate::secondRowIndex(const QModelIndex &index) const
{
    return index.sibling(index.row(), m_column);
}

void TwoRowDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSqlRelationalDelegate::updateEditorGeometry(editor, option, index);
    editor->resize(firstRowSizeHint(option, index));
}

bool TwoRowDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    if (mouseEvent) {
        QRect firstRowRect(option.rect);
        firstRowRect.setSize(firstRowSizeHint(option, index));
        if (!firstRowRect.contains(mouseEvent->pos()))
            return true;
    }
    return QSqlRelationalDelegate::editorEvent(event, model, option, index);
}
