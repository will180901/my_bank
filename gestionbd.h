#ifndef GESTIONBD_H
#define GESTIONBD_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>
#include <QList>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include "comptebancaire.h"
#include "comptecourant.h"
#include "compteepargne.h"

class GestionBD {
private:
    QSqlDatabase m_db;
    QString m_nomFichier;
    QString m_dbPath;

    bool creerTables();
    bool initialiserDonneesParDefaut();
    QString hasherMotDePasse(const QString& motDePasse);
    bool executerRequete(const QString& requete);
    QString genererNumeroCompte(const QString& prefixe);
    bool initialiserRepertoireBD();

public:
    explicit GestionBD(const QString& nomFichier = "banque.db");
    ~GestionBD();

    // Gestion de la base de données
    bool ouvrirConnexion();
    void fermerConnexion();

    // Gestion des paramètres
    bool enregistrerParametre(const QString& cle, const QVariant& valeur);
    QVariant obtenirParametre(const QString& cle, const QVariant& defaut = QVariant());

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
    bool effectuerTransaction(const QString& compteSource,
                              const QString& compteDest,
                              double montant,
                              const QString& description);
    QList<QString> getHistoriqueTransactions(const QString& numeroCompte);

    // Journalisation
    bool enregistrerConnexion(const QString& idUtilisateur, bool succes);
};

#endif // GESTIONBD_H
