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
