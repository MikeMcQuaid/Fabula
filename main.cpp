/*  This file is part of Fabula.
    Copyright (C) 2010 Mike Arthur <mike@mikearthur.co.uk>

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

#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName(QString::fromAscii("Fabula"));
    application.setApplicationVersion(QString::fromAscii("0.1"));
    application.setOrganizationDomain(QString::fromAscii("mikearthur.co.uk"));
    application.setOrganizationName(QString::fromAscii("Mike Arthur"));

    MainWindow mainWindow;
    mainWindow.show();
    return application.exec();
}
