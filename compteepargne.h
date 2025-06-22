#ifndef COMPTEEPARGNE_H
#define COMPTEEPARGNE_H

#include "comptebancaire.h"

class CompteEpargne : public CompteBancaire
{
public:
    CompteEpargne(const QString& id, const QString& numeroCompte,
                  const QString& nomTitulaire, double soldeInitial,
                  double tauxInteret, const QString& banque = "");

    void calculerInterets();
    bool retirer(double montant) override;
    void afficherDetails() const override;
    QString getType() const override;
    double getTauxInteret() const;

private:
    double m_tauxInteret;
};

#endif // COMPTEEPARGNE_H
