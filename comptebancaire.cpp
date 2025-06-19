#include "comptebancaire.h"
#include <QDebug>

CompteBancaire::CompteBancaire(const QString& numeroCompte, const QString& nomTitulaire, double soldeInitial)
    : m_numeroCompte(numeroCompte), m_nomTitulaire(nomTitulaire), m_solde(soldeInitial) {}

void CompteBancaire::deposer(double montant) {
    if (montant > 0) {
        m_solde += montant;
        qDebug() << "Dépôt effectué. Nouveau solde :" << m_solde;
    }
}

bool CompteBancaire::retirer(double montant) {
    if (montant > 0 && montant <= m_solde) {
        m_solde -= montant;
        qDebug() << "Retrait effectué. Nouveau solde :" << m_solde;
        return true;
    }
    return false;
}

double CompteBancaire::getSolde() const {
    return m_solde;
}

QString CompteBancaire::getNumeroCompte() const {
    return m_numeroCompte;
}

QString CompteBancaire::getNomTitulaire() const {
    return m_nomTitulaire;
}
