#include "banque.h"
#include <QDebug>

Banque::Banque(const QString& nom) : m_nom(nom) {}

Banque::~Banque() {
    qDeleteAll(m_comptes);
    m_comptes.clear();
}

bool Banque::ajouterCompte(CompteBancaire* compte) {
    if (!compte || m_comptes.contains(compte->getNumeroCompte())) {
        return false;
    }
    m_comptes.insert(compte->getNumeroCompte(), compte);
    return true;
}

CompteBancaire* Banque::trouverCompte(const QString& numeroCompte) const {
    return m_comptes.value(numeroCompte, nullptr);
}

QList<CompteBancaire*> Banque::getTousLesComptes() const {
    return m_comptes.values();
}

bool Banque::effectuerVirement(const QString& compteSource,
                               const QString& compteDest,
                               double montant) {
    CompteBancaire* source = trouverCompte(compteSource);
    CompteBancaire* dest = trouverCompte(compteDest);

    if (!source || !dest || montant <= 0) {
        return false;
    }

    if (source->retirer(montant)) {
        dest->deposer(montant);
        qDebug() << "Virement effectué :" << montant
                 << "de" << source->getNumeroCompte()
                 << "vers" << dest->getNumeroCompte();
        return true;
    }
    return false;
}

void Banque::afficherTousLesComptes() const {
    qDebug() << "=== Liste des comptes de" << m_nom << "===";
    for (CompteBancaire* compte : m_comptes) {
        compte->afficherDetails();
    }
    qDebug() << "=================================";
}

QString Banque::getNom() const {
    return m_nom;
}

int Banque::getNombreComptes() const {
    return m_comptes.size();
}
