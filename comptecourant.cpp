#include "comptecourant.h"
#include <QDebug>

CompteCourant::CompteCourant(const QString& id,
                             const QString& numeroCompte,
                             const QString& nomTitulaire,
                             double soldeInitial,
                             double decouvertAutorise,
                             const QString& banque)
    : CompteBancaire(id, numeroCompte, nomTitulaire, soldeInitial, banque),
    m_decouvertAutorise(decouvertAutorise) {}

bool CompteCourant::retirer(double montant) {
    if (montant > 0 && (m_solde - montant) >= -m_decouvertAutorise) {
        m_solde -= montant;
        qDebug() << "Retrait effectué. Nouveau solde :" << m_solde
                 << "(Découvert utilisé :" << (m_solde < 0 ? -m_solde : 0) << ")";
        return true;
    }
    qDebug() << "Erreur : Retrait impossible (dépassement du découvert autorisé)";
    return false;
}

void CompteCourant::afficherDetails() const {
    qDebug() << "=== Compte Courant ===";
    qDebug() << "ID:" << getId();
    qDebug() << "Titulaire:" << getNomTitulaire();
    qDebug() << "Numéro:" << getNumeroCompte();
    qDebug() << "Solde:" << getSolde();
    qDebug() << "Découvert autorisé:" << m_decouvertAutorise;
    if (!getBanque().isEmpty()) {
        qDebug() << "Banque:" << getBanque();
    }
    qDebug() << "======================";
}

QString CompteCourant::getType() const {
    return "Compte Courant";
}

double CompteCourant::getDecouvertAutorise() const {
    return m_decouvertAutorise;
}
