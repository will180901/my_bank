#ifndef COMPTEBANCAIRE_H
#define COMPTEBANCAIRE_H

#include <QString>

class CompteBancaire
{
public:
    CompteBancaire(const QString& numeroCompte, const QString& nomTitulaire, double solde = 0.0);
    virtual ~CompteBancaire() = default;

    // Méthodes
    virtual void deposer(double montant);
    virtual bool retirer(double montant) = 0; // Méthode abstraite

    // Getters
    QString getNumeroCompte() const;
    QString getNomTitulaire() const;
    double getSolde() const;

protected:
    QString m_numeroCompte;
    QString m_nomTitulaire;
    double m_solde;
};

#endif // COMPTEBANCAIRE_H
