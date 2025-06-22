#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include "gestionbd.h"
#include "comptebancaire.h"
#include "comptecourant.h"
#include "compteepargne.h"
#include "animationsolde.h"
#include "monboutonbascule.h"

namespace Ui {
class FenetrePrincipale;
}

class AnimationSolde;
class MonBoutonBascule;

class FenetrePrincipale : public QMainWindow
{
    Q_OBJECT

public:
    explicit FenetrePrincipale(QWidget *parent = nullptr, const QString &userId = "");
    ~FenetrePrincipale();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    // Slots pour les boutons latéraux
    void on_btn_lateral_dashboard_clicked();
    void on_btn_lateral_virements_clicked();
    void on_btn_lateral_historique_clicked();
    void on_btn_lateral_parametres_clicked();
    void on_btn_lateral_deconnexion_clicked();

    // Slots pour les boutons de compte
    void on_btn_compte_clicked();
    void on_menu_profil_clicked();
    void on_menu_parametres_clicked();
    void on_menu_deconnexion_clicked();

    // Slots pour la gestion des comptes
    void on_btn_consulter_compte_courant_principal_clicked();
    void on_btn_consulter_livret_epargne_clicked();
    void on_masquer_solde_compte_courant_principale_clicked();
    void on_masquer_solde_compte_epargne_clicked();
    void on_btn_supprimer_compte_courant_clicked();
    void on_btn_supprimer_compte_epargne_clicked();
    void on_btn_effectuer_transaction_compte_courant_clicked();
    void on_btn_effectuer_transaction_Compte_epargne_clicked();

    // Slots pour les paramètres
    void on_btn_sauvegarde_modification_parametre_clicked();
    void on_btn_raccourci_profil_parametre_clicked();

    // Slots pour les raccourcis
    void on_btn_raccourci_effectuer_le_virement_clicked();
    void on_btn_voir_liste_complet_virement_clicked();

    // Slot pour le type d'opération
    void gererChangementTypeOperation(int index);

    void on_btn_lateral_depot_et_retrait_clicked();

signals:
    void authentificationReussie(const QString& userId);
    void deconnexionDemandee();

private:
    Ui::FenetrePrincipale *ui;
    QString m_userId;
    QString m_userFullName;
    QString m_boutonActif;
    QWidget *m_menuCompte;
    bool m_menuCompteVisible;
    bool m_soldeVisibleCompteCourant;
    bool m_soldeVisibleCompteEpargne;
    bool m_soldeVisibleCompteJoint;
    AnimationSolde *m_soldeAnimation;
    MonBoutonBascule *m_boutonBasculeNotificationEmail;
    bool m_notificationsEmailActivees;
    bool m_compteCourantExiste;
    bool m_compteEpargneExiste;
    QWidget *m_rideauCompteCourant;
    QWidget *m_rideauCompteEpargne;

    void configurerFenetrePrincipale();
    void creerMenuCompte();
    void cacherMenuCompte();
    void afficherMenuCompte();
    void positionnerMenuCompte();
    void mettreAJourStyleBoutonCompte();
    void mettreAJourStyleBoutonsLateraux();
    void appliquerStyleBoutonCompte(bool actif);
    void changerPage(const QString &nomPage);
    void chargerDonneesUtilisateur();
    void mettreAJourApparenceComptes();
    void appliquerEffetFlouCompte(QWidget* widgetCarte, bool appliquerFlou);
    void appliquerEffetFlou(QLabel* label, bool masquer);
    void configurerBoutonBasculeNotificationEmail();
    void gererBasculeNotificationEmail(bool estActive);
    void setupPasswordVisibilityToggle(QLineEdit* passwordLineEdit);
    void repositionnerBoutonVisibilite(QLineEdit* passwordLineEdit, QPushButton* toggleButton);
    bool getCurrentVisibilityState(QToolButton* button);
    void updateButtonIcon(QToolButton* button, bool visible);
    void creerBoutonMenu(const QString &texte, const QString &objetNom, const char *slot);
    void ajouterSeparateurMenu(QVBoxLayout *layout);
    void creerCompteCourant();
    void creerCompteEpargne();
    void supprimerCompteCourant();
    void supprimerCompteEpargne();
};

#endif // FENETREPRINCIPALE_H
