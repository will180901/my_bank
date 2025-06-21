#ifndef GESTIONBD_H
#define GESTIONBD_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>
#include <QList>
#include <QDateTime>
#include <QMap>
#include <QVariant>
#include "comptebancaire.h"
#include "comptecourant.h"
#include "compteepargne.h"

class GestionBD {
private:
    QSqlDatabase m_db;
    QString m_nomFichier;

    bool creerTables();
    QString hasherMotDePasse(const QString& motDePasse);
    bool executerRequete(const QString& requete);
    QString genererNumeroCompte(const QString& prefixe);
    bool enregistrerTransaction(const QString& compteId, const QString& type, double montant);

public:
    explicit GestionBD(const QString& nomFichier = "banque.db");
    ~GestionBD();

    // Gestion de la base de données
    bool ouvrirConnexion();
    void fermerConnexion();

    // Gestion des banques
    bool creerBanque(const QString& nom, const QString& codeBanque);
    int getBanqueId(const QString& codeBanque);
    QString getNomBanque(const QString& codeBanque);

    // Gestion des utilisateurs
    bool creerUtilisateur(const QString& nomComplet,
                          const QString& email,
                          const QString& motDePasse);
    bool authentifierUtilisateur(const QString& email,
                                 const QString& motDePasse,
                                 QString& idUtilisateur);
    bool existeUtilisateur(const QString& email);

    // Gestion des comptes bancaires
    bool creerCompte(const QString& typeCompte,
                     const QString& idUtilisateur,
                     const QString& codeBanque,
                     double soldeInitial,
                     double parametreSupplementaire);
    QList<CompteBancaire*> getComptesUtilisateur(const QString& idUtilisateur);
    CompteBancaire* getCompte(const QString& numeroCompte);

    // Gestion des transactions
    bool effectuerDepot(const QString& numeroCompte, double montant);
    bool effectuerRetrait(const QString& numeroCompte, double montant);
    bool effectuerVirement(const QString& compteSource,
                           const QString& compteDest,
                           double montant);
    QList<QString> getHistoriqueTransactions(const QString& numeroCompte);
    QMap<QString, QVariant> getDerniereTransaction(const QString& compteId);

    // Journalisation
    bool enregistrerConnexion(const QString& idUtilisateur, bool succes);

    bool modifierUtilisateur(const QString& idUtilisateur,
                             const QString& nouveauNomComplet,
                             const QString& nouvelEmail,
                             const QString& nouveauMotDePasse = QString());
};

#endif // GESTIONBD_H
