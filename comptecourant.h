#ifndef COMPTECOURANT_H
#define COMPTECOURANT_H

#include "comptebancaire.h"

class CompteCourant : public CompteBancaire {
private:
    double m_decouvertAutorise;

public:
    CompteCourant(const QString& id,
                  const QString& numeroCompte,
                  const QString& nomTitulaire,
                  double soldeInitial,
                  double decouvertAutorise,
                  const QString& banque = "");

    void afficherDetails() const override;
    QString getType() const override;
    bool retirer(double montant) override;
    double getDecouvertAutorise() const;
};

#endif // COMPTECOURANT_H
