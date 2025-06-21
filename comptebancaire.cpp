#include "comptebancaire.h"
#include <QDebug>

CompteBancaire::CompteBancaire(const QString& id, const QString& numeroCompte, const QString& nomTitulaire, double soldeInitial, const QString& banque)
    : m_id(id), m_numeroCompte(numeroCompte), m_nomTitulaire(nomTitulaire), m_solde(soldeInitial), m_banque(banque) {}

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

QString CompteBancaire::getBanque() const {
    return m_banque;
}

QString CompteBancaire::getId() const {
    return m_id;
}

void CompteBancaire::setId(const QString& id) {
    m_id = id;
}
