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
#include <QDebug>

TwoRowDelegate::TwoRowDelegate(QObject *parent) :
    QSqlRelationalDelegate(parent)
{
}

void TwoRowDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    qDebug() << "paint called";
    QSqlRelationalDelegate::paint(painter, option, index);
}

QSize TwoRowDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index)
{
    qDebug() << "sizeHint called";
    return QSqlRelationalDelegate::sizeHint(option, index);
}
