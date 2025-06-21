#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGraphicsBlurEffect>
#include "animationsolde.h"
#include "monboutonbascule.h"
#include "gestionbd.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FenetrePrincipale; }
QT_END_NAMESPACE

class FenetrePrincipale : public QMainWindow
{
    Q_OBJECT

public:
    explicit FenetrePrincipale(QWidget *parent = nullptr, const QString &userId = QString());
    ~FenetrePrincipale();

    QString getUserId() const { return m_userId; }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_btn_lateral_dashboard_clicked();
    void on_btn_lateral_virements_clicked();
    void on_btn_lateral_historique_clicked();
    void on_btn_lateral_parametres_clicked();
    void on_btn_lateral_deconnexion_clicked();

    void on_btn_compte_clicked();
    void mettreAJourStyleBoutonCompte();
    void on_menu_profil_clicked();
    void on_menu_parametres_clicked();
    void on_menu_deconnexion_clicked();
    void on_btn_effectuer_transaction_compte_courant_clicked();
    void on_btn_effectuer_transaction_Compte_epargne_clicked();

    void on_btn_consulter_compte_courant_principal_clicked();
    void on_btn_consulter_livret_epargne_clicked();
    void on_btn_voir_liste_complet_virement_clicked();

    void on_btn_raccourci_profil_parametre_clicked();
    void on_btn_raccourci_effectuer_le_virement_clicked();

    void on_masquer_solde_compte_courant_principale_clicked();
    void on_masquer_solde_compte_epargne_clicked();
    void gererBasculeNotificationEmail(bool estActive);

    void on_btn_lateral_depot_et_retrait_clicked();

    void on_btn_sauvegarde_modification_parametre_clicked();

    void gererChangementTypeOperation(int index);

signals:
    void deconnexionDemandee();

private:
    Ui::FenetrePrincipale *ui;
    QWidget *m_menuCompte;
    AnimationSolde* m_soldeAnimation;
    MonBoutonBascule *m_boutonBasculeNotificationEmail;

    QString m_userId;
    QString m_userFullName;
    QString m_boutonActif;
    bool m_menuCompteVisible;
    bool m_soldeVisibleCompteCourant;
    bool m_soldeVisibleCompteEpargne;
    bool m_soldeVisibleCompteJoint;
    bool m_notificationsEmailActivees = false;

    bool m_compteCourantExiste;
    bool m_compteEpargneExiste;

    void configurerFenetrePrincipale();
    void changerPage(const QString &nomPage);
    void configurerBoutonBasculeNotificationEmail();
    void chargerDonneesUtilisateur();
    void setupPasswordVisibilityToggle(QLineEdit* passwordLineEdit);
    void repositionnerBoutonVisibilite(QLineEdit* passwordLineEdit, QPushButton* toggleButton);
    void appliquerEffetFlouCompte(QWidget* widgetCarte, bool appliquerFlou);

    void creerMenuCompte();
    void cacherMenuCompte();
    void afficherMenuCompte();
    void positionnerMenuCompte();
    void creerBoutonMenu(const QString &texte, const QString &objetNom, const char *slot);
    void ajouterSeparateurMenu(QVBoxLayout *layout);

    void mettreAJourStyleBoutonsLateraux();
    void appliquerStyleBoutonCompte(bool actif);
    void appliquerEffetFlou(QLabel* label, bool masquer);
    void updateButtonIcon(QToolButton* button, bool visible);
    bool getCurrentVisibilityState(QToolButton* button);
};

#endif // FENETREPRINCIPALE_H
