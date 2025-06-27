#ifndef BANQUE_H
#define BANQUE_H

#include <QList>
#include <QString>
#include "CompteBancaire.h"

class Banque
{
public:
    Banque(const QString& nom);

    // Méthodes de gestion des comptes
    void ajouterCompte(CompteBancaire* compte);

    CompteBancaire* trouverCompte(const QString& numeroCompte) const;

    // Opérations bancaires
    bool effectuerVirement(const QString& compteSource, const QString& compteDest, double montant);

    // Getters
    QString getNom() const;

    QList<CompteBancaire*> getComptes() const;

private:
    QString m_nom;
    QList<CompteBancaire*> m_comptes;
};

#endif // BANQUE_H
