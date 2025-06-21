#ifndef COMPTEBANCAIRE_H
#define COMPTEBANCAIRE_H

#include <QString>

class CompteBancaire {
protected:
    QString m_id;
    QString m_numeroCompte;
    QString m_nomTitulaire;
    double m_solde;
    QString m_banque;

public:
    CompteBancaire(const QString& id, const QString& numeroCompte, const QString& nomTitulaire, double soldeInitial, const QString& banque = "");
    virtual ~CompteBancaire() = default;

    virtual void deposer(double montant);
    virtual bool retirer(double montant);
    double getSolde() const;
    QString getNumeroCompte() const;
    QString getNomTitulaire() const;
    QString getBanque() const;
    QString getId() const;
    void setId(const QString& id);

    virtual void afficherDetails() const = 0;
    virtual QString getType() const = 0;
};

#endif // COMPTEBANCAIRE_H
