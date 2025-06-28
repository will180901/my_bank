#include "CompteBancaire.h"

CompteBancaire::CompteBancaire(const QString& numeroCompte, const QString& nomTitulaire, double solde)
    : m_numeroCompte(numeroCompte), m_nomTitulaire(nomTitulaire), m_solde(solde) {}

void CompteBancaire::deposer(double montant)
{
    if (montant > 0) {
        m_solde += montant;
    }
}

bool CompteBancaire::retirer(double montant)
{
    // Implémentation par défaut - à redéfinir dans les classes dérivées
    Q_UNUSED(montant)
    return false;
}

// Getters
QString CompteBancaire::getNumeroCompte() const { return m_numeroCompte; }
QString CompteBancaire::getNomTitulaire() const { return m_nomTitulaire; }
double CompteBancaire::getSolde() const { return m_solde; }
QString CompteBancaire::getDateCreation() const { return m_dateCreation; }
QString CompteBancaire::getDerniereOperation() const { return m_derniereOperation; }

// Setters
void CompteBancaire::setDateCreation(const QString& date) { m_dateCreation = date; }
void CompteBancaire::setDerniereOperation(const QString& operation) { m_derniereOperation = operation; }
