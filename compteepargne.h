#ifndef COMPTEEPARGNE_H
#define COMPTEEPARGNE_H
#include <QDateTime>
#include "comptebancaire.h"

class CompteEpargne : public CompteBancaire {
private:
    double m_tauxInteret;
    QString m_banque;

public:
    CompteEpargne(const QString& numeroCompte,
                  const QString& nomTitulaire,
                  double soldeInitial,
                  double tauxInteret,
                  const QString& banque = "");

    void calculerInterets();
    bool retirer(double montant) override;
    void afficherDetails() const override;
    double getTauxInteret() const;
};

#endif // COMPTEEPARGNE_H
