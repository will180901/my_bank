#ifndef BANQUE_H
#define BANQUE_H

#include <QString>
#include <QMap>
#include "comptebancaire.h"
#include "comptecourant.h"
#include "compteepargne.h"

class Banque {
private:
    QString m_nom;
    QMap<QString, CompteBancaire*> m_comptes; // Clé = numéro de compte

public:
    explicit Banque(const QString& nom);
    ~Banque();

    // Gestion des comptes
    bool ajouterCompte(CompteBancaire* compte);
    CompteBancaire* trouverCompte(const QString& numeroCompte) const;
    QList<CompteBancaire*> getTousLesComptes() const;

    // Opérations bancaires
    bool effectuerVirement(const QString& compteSource,
                           const QString& compteDest,
                           double montant);

    // Affichage
    void afficherTousLesComptes() const;

    // Getters
    QString getNom() const;
    int getNombreComptes() const;
};

#endif // BANQUE_H
