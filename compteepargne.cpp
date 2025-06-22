#include "compteepargne.h"
#include <QDebug>

CompteEpargne::CompteEpargne(const QString& id, const QString& numeroCompte,
                             const QString& nomTitulaire, double soldeInitial,
                             double tauxInteret, const QString& banque)
    : CompteBancaire(id, numeroCompte, nomTitulaire, soldeInitial, banque),
    m_tauxInteret(tauxInteret) {}

void CompteEpargne::calculerInterets()
{
    double interets = getSolde() * m_tauxInteret / 100;
    m_solde += interets;
    qDebug() << "Intérêts appliqués : +" << interets
             << "| Nouveau solde :" << getSolde();
}

bool CompteEpargne::retirer(double montant)
{
    if (montant > 0 && montant <= getSolde()) {
        m_solde -= montant;
        qDebug() << "Retrait effectué. Nouveau solde :" << getSolde();
        return true;
    }
    qDebug() << "Erreur : Retrait impossible (solde insuffisant)";
    return false;
}

void CompteEpargne::afficherDetails() const
{
    qDebug() << "=== Compte Épargne ===";
    qDebug() << "ID:" << getId();
    qDebug() << "Titulaire:" << getNomTitulaire();
    qDebug() << "Numéro:" << getNumeroCompte();
    qDebug() << "Solde:" << getSolde();
    qDebug() << "Taux d'intérêt:" << m_tauxInteret << "%";
    if (!getBanque().isEmpty()) {
        qDebug() << "Banque:" << getBanque();
    }
    qDebug() << "======================";
}

QString CompteEpargne::getType() const
{
    return "Compte Épargne";
}

double CompteEpargne::getTauxInteret() const
{
    return m_tauxInteret;
}
