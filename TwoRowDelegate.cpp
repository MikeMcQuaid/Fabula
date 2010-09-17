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

#include <QPainter>
#include <QStyleOptionViewItemV4>
#include <QAbstractItemView>
#include <QDebug>

TwoRowDelegate::TwoRowDelegate(int secondRowColumn, QAbstractItemView *view, QObject *parent) :
    QSqlRelationalDelegate(parent), m_column(secondRowColumn), m_view(view)
{
}

void TwoRowDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == m_column)
        return;

    if (index.column() == 1) {
        QStyleOptionViewItemV4 secondRowOption(option);
        secondRowOption.rect.moveTop(secondRowHeight(option, index));
        secondRowOption.rect.setHeight(secondRowHeight(option, index));
        secondRowOption.rect.setWidth(m_view->width());
        QSqlRelationalDelegate::paint(painter, secondRowOption, secondRowIndex(index));
    }

    QStyleOptionViewItemV4 firstRowOption(option);
    firstRowOption.rect.setSize(firstRowSizeHint(option, index));
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
    return QSqlRelationalDelegate::sizeHint(option, secondRowIndex(index)).height();
}

QModelIndex TwoRowDelegate::secondRowIndex(const QModelIndex &index) const
{
    return index.sibling(index.row(), m_column);
}
