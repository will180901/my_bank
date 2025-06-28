#include "CompteEpargne.h"

CompteEpargne::CompteEpargne(const QString& numeroCompte, const QString& nomTitulaire, double solde, double tauxInteret)
    : CompteBancaire(numeroCompte, nomTitulaire, solde), m_tauxInteret(tauxInteret) {}

bool CompteEpargne::retirer(double montant)
{
    if (montant > 0 && montant <= m_solde) {
        m_solde -= montant;
        return true;
    }
    return false;
}

void CompteEpargne::calculerInterets()
{
    m_solde += m_solde * (m_tauxInteret / 100);
}

double CompteEpargne::getTauxInteret() const { return m_tauxInteret; }
void CompteEpargne::setTauxInteret(double taux) { m_tauxInteret = taux; }
