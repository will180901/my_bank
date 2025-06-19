#include "comptecourant.h"
#include <QDebug>

CompteCourant::CompteCourant(const QString& numeroCompte,
                             const QString& nomTitulaire,
                             double soldeInitial,
                             double decouvertAutorise,
                             const QString& banque)
    : CompteBancaire(numeroCompte, nomTitulaire, soldeInitial),
    m_decouvertAutorise(decouvertAutorise),
    m_banque(banque) {}

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
    qDebug() << "Titulaire:" << m_nomTitulaire;
    qDebug() << "Numéro:" << m_numeroCompte;
    qDebug() << "Solde:" << m_solde;
    qDebug() << "Découvert autorisé:" << m_decouvertAutorise;
    if (!m_banque.isEmpty()) {
        qDebug() << "Banque:" << m_banque;
    }
    qDebug() << "======================";
}

double CompteCourant::getDecouvertAutorise() const {
    return m_decouvertAutorise;
}
