#ifndef UTILITAIRESMOTDEPASSE_H
#define UTILITAIRESMOTDEPASSE_H

#include <QLineEdit>
#include <QPushButton>
#include <QObject> // Ajout nécessaire

class UtilitairesMotDePasse
{
public:
    static void configurerBoutonVisibilite(QLineEdit* champMotDePasse);
    static void repositionnerBouton(QLineEdit* champMotDePasse);
};

#endif // UTILITAIRESMOTDEPASSE_H
