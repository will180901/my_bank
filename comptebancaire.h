#ifndef COMPTEBANCAIRE_H
#define COMPTEBANCAIRE_H

#include <QString>
#include <QList>
#include <memory>

// Classe abstraite CompteBancaire
class CompteBancaire
{
protected:
    QString m_numeroCompte;
    QString m_nomTitulaire;
    double m_solde;

public:
    CompteBancaire(const QString& numeroCompte, const QString& nomTitulaire, double soldeInitial = 0.0);
    virtual ~CompteBancaire() = default;

    // Méthodes publiques
    virtual bool deposer(double montant);
    virtual bool retirer(double montant) = 0; // Méthode virtuelle pure
    double afficherSolde() const;

    // Getters
    QString getNumeroCompte() const;
    QString getNomTitulaire() const;
    double getSolde() const;

    // Setters
    void setSolde(double nouveauSolde);

protected:
    bool validerMontant(double montant) const;
    void ajouterAuSolde(double montant);
    void retirerDuSolde(double montant);
};

// Classe CompteCourant
class CompteCourant : public CompteBancaire
{
private:
    double m_decouvertAutorise;

public:
    CompteCourant(const QString& numeroCompte, const QString& nomTitulaire,
                  double soldeInitial = 0.0, double decouvertAutorise = 0.0);

    // Redéfinition des méthodes virtuelles
    bool retirer(double montant) override;

    // Méthodes spécifiques
    double getDecouvertAutorise() const;
    void setDecouvertAutorise(double nouveauDecouverte);
    double getSoldeDisponible() const;

private:
    bool verifierLimiteDecouverte(double montant) const;
};

// Classe CompteEpargne
class CompteEpargne : public CompteBancaire
{
private:
    double m_tauxInteret;

public:
    CompteEpargne(const QString& numeroCompte, const QString& nomTitulaire,
                  double soldeInitial = 0.0, double tauxInteret = 0.0);

    // Redéfinition des méthodes virtuelles
    bool retirer(double montant) override;

    // Méthodes spécifiques
    double getTauxInteret() const;
    void setTauxInteret(double nouveauTaux);
    double calculerInterets() const;
    void appliquerInterets();

private:
    bool verifierSoldePositif(double montant) const;
};

// Classe Banque
class Banque
{
private:
    QString m_nom;

    QList<std::shared_ptr<CompteBancaire>> m_listeComptes;

public:
    Banque(const QString& nom);
    ~Banque() = default;

    // Gestion des comptes
    bool ajouterCompte(std::shared_ptr<CompteBancaire> compte);
    std::shared_ptr<CompteBancaire> trouverCompte(const QString& numeroCompte);
    bool supprimerCompte(const QString& numeroCompte);

    // Opérations bancaires
    bool effectuerVirement(const QString& compteSrc, const QString& compteDest, double montant);
    bool effectuerDepot(const QString& numeroCompte, double montant);
    bool effectuerRetrait(const QString& numeroCompte, double montant);

    // Getters
    QString getNom() const;
    double getSolde() const;
    QList<std::shared_ptr<CompteBancaire>> getListeComptes() const;
    int getNombreComptes() const;

    // Setters
    void setNom(const QString& nouveauNom);


private:
    bool validerVirement(std::shared_ptr<CompteBancaire> compteSrc,
                         std::shared_ptr<CompteBancaire> compteDest, double montant);

};

#endif // COMPTEBANCAIRE_H
