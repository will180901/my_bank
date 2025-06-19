#ifndef COMPTECOURANT_H
#define COMPTECOURANT_H
#include <QDateTime>

#include "comptebancaire.h"

class CompteCourant : public CompteBancaire {
private:
    double m_decouvertAutorise;
    QString m_banque;

public:
    CompteCourant(const QString& numeroCompte,
                  const QString& nomTitulaire,
                  double soldeInitial,
                  double decouvertAutorise,
                  const QString& banque = "");

    bool retirer(double montant) override;
    void afficherDetails() const override;
    double getDecouvertAutorise() const;
};

#endif // COMPTECOURANT_H
