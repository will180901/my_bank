#ifndef COMPTEBANCAIRE_H
#define COMPTEBANCAIRE_H

#include <QString>

class CompteBancaire {
protected:
    QString m_numeroCompte;
    QString m_nomTitulaire;
    double m_solde;

public:
    CompteBancaire(const QString& numeroCompte, const QString& nomTitulaire, double soldeInitial);
    virtual ~CompteBancaire() = default;

    // Méthodes de base
    virtual void deposer(double montant);
    virtual bool retirer(double montant);  // Retourne true si l'opération réussit
    double getSolde() const;
    QString getNumeroCompte() const;
    QString getNomTitulaire() const;

    // Méthode polymorphe
    virtual void afficherDetails() const = 0;  // Pure virtuelle -> rend la classe abstraite
};

#endif // COMPTEBANCAIRE_H
