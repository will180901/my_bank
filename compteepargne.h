#ifndef COMPTEEPARGNE_H
#define COMPTEEPARGNE_H

#include "CompteBancaire.h"

class CompteEpargne : public CompteBancaire
{
public:
    CompteEpargne(const QString& numeroCompte, const QString& nomTitulaire, double solde, double tauxInteret);

    bool retirer(double montant) override;
    void calculerInterets();

    // Getters/Setters
    double getTauxInteret() const;
    void setTauxInteret(double taux);

private:
    double m_tauxInteret;
};

#endif // COMPTEEPARGNE_H
