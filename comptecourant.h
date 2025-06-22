#ifndef COMPTECOURANT_H
#define COMPTECOURANT_H

#include "comptebancaire.h"

class CompteCourant : public CompteBancaire
{
public:
    CompteCourant(const QString& id, const QString& numeroCompte,
                  const QString& nomTitulaire, double soldeInitial,
                  double decouvertAutorise, const QString& banque = "");

    bool retirer(double montant) override;
    void afficherDetails() const override;
    QString getType() const override;
    double getDecouvertAutorise() const;

private:
    double m_decouvertAutorise;
};

#endif // COMPTECOURANT_H
