#ifndef AUTHENTIFICATION_H
#define AUTHENTIFICATION_H

#include <QDialog>
#include <QRegularExpression>
#include <QLineEdit>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "utilitairesmotdepasse.h"
#include "creationbd.h"

namespace Ui {
class Authentification;
}

class Authentification : public QDialog
{
    Q_OBJECT

public:
    explicit Authentification(QWidget *parent = nullptr);
    ~Authentification();

    // Méthodes de validation statiques
    static bool validerNom(const QString& nom);
    static bool validerEmail(const QString& email);
    static bool validerMotDePasse(const QString& motDePasse);

    // Méthodes de validation avancées
    static bool validerForceMotDePasse(const QString& motDePasse, QString& messageErreur);
    static QString genererMotDePasseAleatoire(int longueur = 12);

    // Getters
    QString obtenirUtilisateurID() const { return m_utilisateurID; }
    bool baseDonneesOuverte() const { return m_creationbd.estOuverte(); }


signals:
    void authentificationReussie(const QString& userId,CreationBD & m_BD);
    void compteUtilisateurCree(const QString& userId);
    void erreurAuthentification(const QString& message);

private slots:
    // Navigation entre les pages
    void on_btn_creer_compte_zone_connexion_clicked();
    void on_btn_se_connecter_zone_creer_compte_clicked();

    // Actions principales
    void on_btn_creer_compte_zone_creer_compte_clicked();
    void on_btn_se_connecter_zone_connexion_clicked();

    // Utilitaires UI
    void viderMessagesErreur();
    void reinitialiserFormulaires();

private:
    Ui::Authentification *ui;
    QString m_utilisateurID;
    CreationBD m_creationbd;
    static const QString ADMIN_EMAIL;

    // Compteurs de sécurité
    int m_tentativesConnexionEchouees;
    static const int MAX_TENTATIVES_CONNEXION = 5;

    // Méthodes d'initialisation
    void InitialisationConnexion();
    void initialisationBD();
    void configurerInterface();

    // Méthodes de gestion de base de données

    void verifierEtCreerAdminSiNecessaire();
    void remplirEmailPremierUtilisateur();

    bool existeUtilisateur(const QString& email);
    bool creerUtilisateur(const QString& nomComplet, const QString& email, const QString& motDePasse);
    bool authentifierUtilisateur(const QString& email, const QString& motDePasse);
    bool enregistrerConnexion(const QString& email, bool succes);
    bool creerCompteAdministrateur();

    // Méthodes de sécurité
    QString hasherMotDePasse(const QString& motDePasse);
    bool verifierIntegriteUtilisateur(const QString& userId);
    void bloquerConnexions(int dureeMinutes = 15);
    bool connexionsBloquees() const;

    // Méthodes utilitaires
    void afficherMessageErreur(const QString& message, bool pageCreation = false);
    void afficherMessageSucces(const QString& message);
    QString formaterNomComplet(const QString& nom);
    bool validerDonneesCreation(const QString& nom, const QString& email,
                                const QString& mdp, const QString& confirmMdp);
    bool validerDonneesConnexion(const QString& email, const QString& mdp);

    // Gestion des événements
    bool eventFilter(QObject* obj, QEvent* event) override;


};

#endif // AUTHENTIFICATION_H
