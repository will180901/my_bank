#include "authentification.h"
#include "fenmain.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    Authentification auth;
    fenMain* mainWindow = nullptr;

    // Connexion pour gérer l'authentification réussie
    QObject::connect(&auth, &Authentification::authentificationReussie,
                     [&](const QString& userId, CreationBD& m_BD) {
                         qDebug() << "Authentification réussie pour l'utilisateur:" << userId;

                         // Nettoyer l'ancienne fenêtre si elle existe
                         if (mainWindow) {
                             mainWindow->deleteLater();
                             mainWindow = nullptr;
                         }

                         // Créer et afficher la nouvelle fenêtre principale
                         // Ordre des paramètres corrigé : CreationBD&, QWidget*, QString
                         try {
                             mainWindow = new fenMain(m_BD, nullptr, userId);
                             mainWindow->show();
                             auth.hide();
                             qDebug() << "Fenêtre principale créée et affichée";
                         } catch (const std::exception& e) {
                             qDebug() << "Erreur lors de la création de la fenêtre principale:" << e.what();
                         }
                     });

    // Optionnel: Gérer la fermeture de l'authentification
    QObject::connect(&auth, &QWidget::destroyed, [&]() {
        if (mainWindow) {
            mainWindow->deleteLater();
            mainWindow = nullptr;
        }
    });

    // Optionnel: Gérer la fermeture de la fenêtre principale
    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
        if (mainWindow) {
            mainWindow->deleteLater();
            mainWindow = nullptr;
        }
    });

    // Afficher la fenêtre d'authentification
    auth.show();

    return app.exec();
}
