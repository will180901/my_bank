#include "CompteBancaire.h"

CompteBancaire::CompteBancaire(const QString& numeroCompte, const QString& nomTitulaire, double solde)
    : m_numeroCompte(numeroCompte), m_nomTitulaire(nomTitulaire), m_solde(solde)
{
}

void CompteBancaire::deposer(double montant)
{
    if (montant > 0) {
        m_solde += montant;
    }
}

QString CompteBancaire::getNumeroCompte() const
{
    return m_numeroCompte;
}

QString CompteBancaire::getNomTitulaire() const
{
    return m_nomTitulaire;
}

double CompteBancaire::getSolde() const
{
    return m_solde;
}
