#ifndef COMPTECOURANT_H
#define COMPTECOURANT_H

#include "CompteBancaire.h"

class CompteCourant : public CompteBancaire
{
public:
    CompteCourant(const QString& numeroCompte, const QString& nomTitulaire, double solde = 0.0, double decouvertAutorise = 0.0);

    // Redéfinition méthode abstraite
    bool retirer(double montant) override;

    // Getter/Setter spécifique
    double getDecouvertAutorise() const;
    void setDecouvertAutorise(double decouvert);

private:
    double m_decouvertAutorise;
};

#endif // COMPTECOURANT_H
