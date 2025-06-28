#ifndef BANQUE_H
#define BANQUE_H

#include <QString>
#include <QList>
#include "CompteBancaire.h"

class Banque
{
private:
    QString m_nom;
    QList<CompteBancaire*> m_comptes;  // AJOUT: Déclaration du membre manquant

public:
    explicit Banque(const QString& nom);
    ~Banque();

    // Méthodes de gestion des comptes
    void ajouterCompte(CompteBancaire* compte);
    CompteBancaire* trouverCompte(const QString& numeroCompte) const;
    void viderComptes();
    void supprimerCompte(const QString& numeroCompte);  // AJOUT: Déclaration manquante

    // Opérations
    bool effectuerVirement(const QString& compteSource, const QString& compteDest, double montant);

    // Accesseurs
    QString getNom() const;
    QList<CompteBancaire*> getComptes() const;  // CORRECTION: Maintenant public
};

#endif // BANQUE_H
