#include "CompteCourant.h"

CompteCourant::CompteCourant(const QString& numeroCompte, const QString& nomTitulaire, double solde, double decouvertAutorise)
    : CompteBancaire(numeroCompte, nomTitulaire, solde), m_decouvertAutorise(decouvertAutorise)
{
}

bool CompteCourant::retirer(double montant)
{
    if (montant > 0 && (m_solde - montant) >= -m_decouvertAutorise) {
        m_solde -= montant;
        return true;
    }
    return false;
}

double CompteCourant::getDecouvertAutorise() const
{
    return m_decouvertAutorise;
}

void CompteCourant::setDecouvertAutorise(double decouvert)
{
    m_decouvertAutorise = decouvert;
}
