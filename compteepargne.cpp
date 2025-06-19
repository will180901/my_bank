#include "compteepargne.h"
#include <QDebug>

CompteEpargne::CompteEpargne(const QString& numeroCompte,
                             const QString& nomTitulaire,
                             double soldeInitial,
                             double tauxInteret,
                             const QString& banque)
    : CompteBancaire(numeroCompte, nomTitulaire, soldeInitial),
    m_tauxInteret(tauxInteret),
    m_banque(banque) {}

void CompteEpargne::calculerInterets() {
    double interets = m_solde * m_tauxInteret / 100;
    m_solde += interets;
    qDebug() << "Intérêts appliqués : +" << interets
             << "| Nouveau solde :" << m_solde;
}

bool CompteEpargne::retirer(double montant) {
    if (montant > 0 && montant <= m_solde) {
        m_solde -= montant;
        qDebug() << "Retrait effectué. Nouveau solde :" << m_solde;
        return true;
    }
    qDebug() << "Erreur : Retrait impossible (solde insuffisant)";
    return false;
}

void CompteEpargne::afficherDetails() const {
    qDebug() << "=== Compte Épargne ===";
    qDebug() << "Titulaire:" << m_nomTitulaire;
    qDebug() << "Numéro:" << m_numeroCompte;
    qDebug() << "Solde:" << m_solde;
    qDebug() << "Taux d'intérêt:" << m_tauxInteret << "%";
    if (!m_banque.isEmpty()) {
        qDebug() << "Banque:" << m_banque;
    }
    qDebug() << "======================";
}

double CompteEpargne::getTauxInteret() const {
    return m_tauxInteret;
}
