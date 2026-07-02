#include <QApplication>

#include "database/DatabaseManager.h"
#include "windows/LoginWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Connect to the database
    if (!DatabaseManager::instance().connect())
        return -1;

    // Show login window
    LoginWindow login;
    login.show();

    return app.exec();
}