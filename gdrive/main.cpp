
#include <QCoreApplication>
#include <QTimer>

#include "gdrive.h"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName("prog-org-ru-developers");
    app.setApplicationName("gdrive-cli");
    
    GDrive cli;
    return app.exec();
}








