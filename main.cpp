#include "authentification.h"
#include "fenetreprincipale.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Authentification auth;
    FenetrePrincipale* mainWindow = nullptr;

    QObject::connect(&auth, &Authentification::authentificationReussie, [&](const QString& userId) {
        if (mainWindow) {
            mainWindow->deleteLater();
        }
        mainWindow = new FenetrePrincipale(nullptr, userId);
        mainWindow->show();
        auth.hide();
    });

    auth.show();

    return app.exec();
}
