#include "comptebancaire.h"
#include <QDebug>

// =============================================================================
// Implémentation de la classe CompteBancaire
// =============================================================================

CompteBancaire::CompteBancaire(const QString& numeroCompte, const QString& nomTitulaire, double soldeInitial)
    : m_numeroCompte(numeroCompte), m_nomTitulaire(nomTitulaire), m_solde(soldeInitial)
{
}

bool CompteBancaire::deposer(double montant)
{
    if (!validerMontant(montant)) {
        qDebug() << "Erreur: Montant invalide pour le dépôt:" << montant;
        return false;
    }

    ajouterAuSolde(montant);
    qDebug() << "Dépôt effectué:" << montant << "€ - Nouveau solde:" << m_solde << "€";
    return true;
}

double CompteBancaire::afficherSolde() const
{
    return m_solde;
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

void CompteBancaire::setSolde(double nouveauSolde)
{
    m_solde = nouveauSolde;
}

bool CompteBancaire::validerMontant(double montant) const
{
    return montant > 0.0;
}

void CompteBancaire::ajouterAuSolde(double montant)
{
    m_solde += montant;
}

void CompteBancaire::retirerDuSolde(double montant)
{
    m_solde -= montant;
}

// =============================================================================
// Implémentation de la classe CompteCourant
// =============================================================================

CompteCourant::CompteCourant(const QString& numeroCompte, const QString& nomTitulaire,
                             double soldeInitial, double decouvertAutorise)
    : CompteBancaire(numeroCompte, nomTitulaire, soldeInitial), m_decouvertAutorise(decouvertAutorise)
{
}

bool CompteCourant::retirer(double montant)
{
    if (!validerMontant(montant)) {
        qDebug() << "Erreur: Montant invalide pour le retrait:" << montant;
        return false;
    }

    if (!verifierLimiteDecouverte(montant)) {
        qDebug() << "Erreur: Retrait refusé - Limite de découvert dépassée";
        qDebug() << "Montant demandé:" << montant << "€ - Solde disponible:" << getSoldeDisponible() << "€";
        return false;
    }

    retirerDuSolde(montant);
    qDebug() << "Retrait effectué:" << montant << "€ - Nouveau solde:" << m_solde << "€";
    return true;
}

double CompteCourant::getDecouvertAutorise() const
{
    return m_decouvertAutorise;
}

void CompteCourant::setDecouvertAutorise(double nouveauDecouverte)
{
    m_decouvertAutorise = nouveauDecouverte;
}

double CompteCourant::getSoldeDisponible() const
{
    return m_solde + m_decouvertAutorise;
}

bool CompteCourant::verifierLimiteDecouverte(double montant) const
{
    return (m_solde - montant) >= -m_decouvertAutorise;
}

// =============================================================================
// Implémentation de la classe CompteEpargne
// =============================================================================

CompteEpargne::CompteEpargne(const QString& numeroCompte, const QString& nomTitulaire,
                             double soldeInitial, double tauxInteret)
    : CompteBancaire(numeroCompte, nomTitulaire, soldeInitial), m_tauxInteret(tauxInteret)
{
}

bool CompteEpargne::retirer(double montant)
{
    if (!validerMontant(montant)) {
        qDebug() << "Erreur: Montant invalide pour le retrait:" << montant;
        return false;
    }

    if (!verifierSoldePositif(montant)) {
        qDebug() << "Erreur: Retrait refusé - Solde insuffisant";
        qDebug() << "Montant demandé:" << montant << "€ - Solde disponible:" << m_solde << "€";
        return false;
    }

    retirerDuSolde(montant);
    qDebug() << "Retrait effectué:" << montant << "€ - Nouveau solde:" << m_solde << "€";
    return true;
}

double CompteEpargne::getTauxInteret() const
{
    return m_tauxInteret;
}

void CompteEpargne::setTauxInteret(double nouveauTaux)
{
    m_tauxInteret = nouveauTaux;
}

double CompteEpargne::calculerInterets() const
{
    return m_solde * (m_tauxInteret / 100.0);
}

void CompteEpargne::appliquerInterets()
{
    double interets = calculerInterets();
    ajouterAuSolde(interets);
    qDebug() << "Intérêts appliqués:" << interets << "€ - Nouveau solde:" << m_solde << "€";
}

bool CompteEpargne::verifierSoldePositif(double montant) const
{
    return (m_solde - montant) >= 0.0;
}

// =============================================================================
// Implémentation de la classe Banque
// =============================================================================

Banque::Banque(const QString& nom)
    : m_nom(nom)
{
}

bool Banque::ajouterCompte(std::shared_ptr<CompteBancaire> compte)
{
    if (!compte) {
        qDebug() << "Erreur: Tentative d'ajout d'un compte null";
        return false;
    }

    // Vérifier si le compte existe déjà
    if (trouverCompte(compte->getNumeroCompte())) {
        qDebug() << "Erreur: Compte déjà existant:" << compte->getNumeroCompte();
        return false;
    }

    m_listeComptes.append(compte);
    qDebug() << "Compte ajouté avec succès:" << compte->getNumeroCompte();
    return true;
}

std::shared_ptr<CompteBancaire> Banque::trouverCompte(const QString& numeroCompte)
{
    for (auto& compte : m_listeComptes) {
        if (compte->getNumeroCompte() == numeroCompte) {
            return compte;
        }
    }
    return nullptr;
}

bool Banque::supprimerCompte(const QString& numeroCompte)
{
    for (int i = 0; i < m_listeComptes.size(); ++i) {
        if (m_listeComptes[i]->getNumeroCompte() == numeroCompte) {
            m_listeComptes.removeAt(i);
            qDebug() << "Compte supprimé:" << numeroCompte;
            return true;
        }
    }
    qDebug() << "Erreur: Compte non trouvé pour suppression:" << numeroCompte;
    return false;
}

bool Banque::effectuerVirement(const QString& compteSrc, const QString& compteDest, double montant)
{
    auto compteSource = trouverCompte(compteSrc);
    auto compteDestination = trouverCompte(compteDest);

    if (!validerVirement(compteSource, compteDestination, montant)) {
        return false;
    }

    // Effectuer le virement
    if (compteSource->retirer(montant)) {
        compteDestination->deposer(montant);
        qDebug() << "Virement effectué:" << montant << "€ de" << compteSrc << "vers" << compteDest;
        return true;
    }

    qDebug() << "Erreur: Échec du retrait pour le virement";
    return false;
}

bool Banque::effectuerDepot(const QString& numeroCompte, double montant)
{
    auto compte = trouverCompte(numeroCompte);
    if (!compte) {
        qDebug() << "Erreur: Compte non trouvé pour dépôt:" << numeroCompte;
        return false;
    }

    return compte->deposer(montant);
}

bool Banque::effectuerRetrait(const QString& numeroCompte, double montant)
{
    auto compte = trouverCompte(numeroCompte);
    if (!compte) {
        qDebug() << "Erreur: Compte non trouvé pour retrait:" << numeroCompte;
        return false;
    }

    return compte->retirer(montant);
}

QString Banque::getNom() const
{
    return m_nom;
}

double Banque::getSolde() const
{
    double total = 0.0;
    for (const auto& compte : m_listeComptes) {
        total += compte->getSolde();
    }
    return total;
}

QList<std::shared_ptr<CompteBancaire>> Banque::getListeComptes() const
{
    return m_listeComptes;
}

int Banque::getNombreComptes() const
{
    return m_listeComptes.size();
}

void Banque::setNom(const QString& nouveauNom)
{
    m_nom = nouveauNom;
}



bool Banque::validerVirement(std::shared_ptr<CompteBancaire> compteSrc,
                             std::shared_ptr<CompteBancaire> compteDest, double montant)
{
    if (!compteSrc) {
        qDebug() << "Erreur: Compte source non trouvé";
        return false;
    }

    if (!compteDest) {
        qDebug() << "Erreur: Compte destination non trouvé";
        return false;
    }

    if (montant <= 0.0) {
        qDebug() << "Erreur: Montant invalide pour virement:" << montant;
        return false;
    }

    if (compteSrc->getNumeroCompte() == compteDest->getNumeroCompte()) {
        qDebug() << "Erreur: Virement vers le même compte";
        return false;
    }

    return true;
}

