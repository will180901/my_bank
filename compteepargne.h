#ifndef COMPTEEPARGNE_H
#define COMPTEEPARGNE_H

#include "CompteBancaire.h"

class CompteEpargne : public CompteBancaire
{
public:
    CompteEpargne(const QString& numeroCompte, const QString& nomTitulaire, double solde = 0.0, double tauxInteret = 0.0);

    // Redéfinition méthode abstraite
    bool retirer(double montant) override;

    // Méthode spécifique
    void calculerInterets();

    // Getter/Setter spécifique
    double getTauxInteret() const;
    void setTauxInteret(double taux);

private:
    double m_tauxInteret;
};

#endif // COMPTEEPARGNE_H
