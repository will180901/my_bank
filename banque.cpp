#include "Banque.h"

Banque::Banque(const QString& nom) : m_nom(nom) {}

Banque::~Banque() {
    viderComptes();
}

void Banque::viderComptes()
{
    for (CompteBancaire* compte : m_comptes) {
        if (compte) {
            delete compte;
        }
    }
    m_comptes.clear();
}

void Banque::ajouterCompte(CompteBancaire* compte)
{
    if (compte && !trouverCompte(compte->getNumeroCompte())) {
        m_comptes.append(compte);
    }
}

CompteBancaire* Banque::trouverCompte(const QString& numeroCompte) const
{
    for (CompteBancaire* compte : m_comptes) {
        if (compte->getNumeroCompte() == numeroCompte) {
            return compte;
        }
    }
    return nullptr;
}

bool Banque::effectuerVirement(const QString& compteSource, const QString& compteDest, double montant)
{
    CompteBancaire* source = trouverCompte(compteSource);
    CompteBancaire* dest = trouverCompte(compteDest);

    if (!source || !dest || montant <= 0) return false;

    if (source->retirer(montant)) {
        dest->deposer(montant);
        return true;
    }
    return false;
}

void Banque::supprimerCompte(const QString& numeroCompte)
{
    for (auto it = m_comptes.begin(); it != m_comptes.end(); ++it) {
        if ((*it)->getNumeroCompte() == numeroCompte) {
            delete *it;
            m_comptes.erase(it);
            break;
        }
    }
}

QString Banque::getNom() const {
    return m_nom;
}

QList<CompteBancaire*> Banque::getComptes() const {
    return m_comptes;
}
