#ifndef GESTIONBD_H
#define GESTIONBD_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QCryptographicHash>
#include "comptebancaire.h"
#include "comptecourant.h"
#include "compteepargne.h"

class GestionBD : public QObject
{
    Q_OBJECT

public:
    explicit GestionBD(const QString& nomFichier, QObject *parent = nullptr);
    ~GestionBD();

    bool ouvrirConnexion();
    void fermerConnexion();
    QString hasherMotDePasse(const QString& motDePasse);

    // Gestion des utilisateurs
    bool creerUtilisateur(const QString& nomComplet, const QString& email, const QString& motDePasse);
    bool authentifierUtilisateur(const QString& email, const QString& motDePasse, QString& idUtilisateur);
    bool existeUtilisateur(const QString& email);
    bool modifierUtilisateur(const QString& idUtilisateur, const QString& nouveauNomComplet,
                             const QString& nouvelEmail, const QString& nouveauMotDePasse);

    // Gestion des comptes
    bool creerCompte(const QString& typeCompte, const QString& idUtilisateur,
                     const QString& codeBanque, double soldeInitial, double parametreSupplementaire);
    bool supprimerCompte(const QString& idCompte);
    bool compteExiste(const QString& idUtilisateur, const QString& typeCompte);
    QList<CompteBancaire*> getComptesUtilisateur(const QString& idUtilisateur);
    CompteBancaire* getCompte(const QString& numeroCompte);

    // Gestion des transactions
    bool effectuerDepot(const QString& numeroCompte, double montant);
    bool effectuerRetrait(const QString& numeroCompte, double montant);
    bool effectuerVirement(const QString& compteSource, const QString& compteDest, double montant);
    QList<QString> getHistoriqueTransactions(const QString& numeroCompte);
    QMap<QString, QVariant> getDerniereTransaction(const QString& compteId);

    // Journalisation
    bool enregistrerConnexion(const QString& idUtilisateur, bool succes);

private:
    bool creerDossierBD();
    bool creerTables();
    bool executerRequete(const QString& requete);
    QString genererNumeroCompte(const QString& prefixe);
    bool enregistrerTransaction(const QString& compteId, const QString& type, double montant);

    // Gestion des banques
    bool creerBanque(const QString& nom, const QString& codeBanque);
    int getBanqueId(const QString& codeBanque);
    QString getNomBanque(const QString& codeBanque);

    QSqlDatabase m_db;
    QString m_nomFichier;
    QString m_cheminBD;
};

#endif // GESTIONBD_H
