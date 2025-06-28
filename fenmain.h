// ========== CORRECTIONS POUR LE HEADER fenmain.h ==========

#ifndef FENMAIN_H
#define FENMAIN_H

#include "Banque.h"
#include "CreationBD.h"
#include "AnimationSolde.h"
#include "comptebancaire.h"
#include "comptecourant.h"
#include "compteepargne.h"
#include "monboutonbascule.h"
#include "utilitairesmotdepasse.h"

#include <QMainWindow>
#include <QToolButton>
#include <QStackedWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextEdit>
#include <QLineEdit>
#include <QSettings>

namespace Ui {
class fenMain;
}

class fenMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit fenMain(CreationBD& m_BD, QWidget *parent = nullptr, const QString &utilisateur_id = QString());
    ~fenMain();

    // CORRECTION : Ajout de méthodes publiques pour la gestion des thèmes
    bool estThemeSombreActif() const;
    void forcerTheme(bool themeSombre);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    // Slots pour les transactions
    void on_btn_valider_transaction_clicked();

    // Slots pour la suppression des comptes
    void on_btn_supprimer_compte_courant_clicked();
    void on_btn_supprimer_compte_epargne_clicked();

    // Autres slots existants
    void on_btn_masquer_solde_compte_courant_clicked();
    void on_btn_masquer_solde_compte_epargne_clicked();
    void on_btn_dashboard_barre_latterale_clicked();
    void on_btn_historique_barre_latterale_clicked();
    void on_btn_parametres_barre_latterale_clicked();
    void on_btn_consulter_compte_epargne_clicked();
    void on_btn_consulter_compte_courant_clicked();
    void on_btn_effectuer_transaction_compte_courant_clicked();
    void on_btn_effectuer_transaction_compte_epargne_clicked();
    void on_btn_voir_liste_complete_transaction_clicked();
    void on_btn_ajouter_compte_courant_clicked();
    void on_btn_ajouter_compte_epargne_clicked();
    void on_btn_modifier_info_tutilaire_parametre_clicked();

private:
    Ui::fenMain *ui;

    // CORRECTION : Regroupement des variables de thème
    MonBoutonBascule* m_boutonBasculeChangertheme;
    bool m_mode_sombre_active;

    // Méthodes pour les transactions
    void effectuerDepot(CompteBancaire* compte, double montant, const QString& motif);
    void effectuerRetrait(CompteBancaire* compte, double montant, const QString& motif);
    void effectuerVirement(const QString& compteSource, const QString& compteDest, double montant, const QString& motif);
    void sauvegarderCompte(CompteBancaire* compte);
    void mettreAJourAffichageComptes();

    // CORRECTION : Regroupement des méthodes de thème
    void configurerBoutonBasculeThemeCouleur();
    void gererBasculeThemeCouleur(bool estActive);
    void InitialisationThemeCouleur();

    // Méthodes pour la suppression
    void supprimerCompteCourant();
    void supprimerCompteEpargne();

    // Autres méthodes
    void chargerDonneesDepuisBD();
    void chargerInformationsUtilisateur();
    void chargerComptesBancaires();
    CompteCourant* getCompteCourant() const;
    CompteEpargne* getCompteEpargne() const;
    void appliquerStyleBoutonMasquage(QToolButton* button, bool survole);
    void mettreAjourIcon(QToolButton* button, bool visible);
    void mettreAJourStyleBoutonsLateraux();
    void appliquerEffetFlou(QLabel* label, bool masquer);
    void creerCompteCourant();
    void creerCompteEpargne();
    QString genererNumeroCompte(const QString& typeCompte);
    void creerCompteCourantEnBD();
    void creerCompteEpargneEnBD();
    void sauvegarderDonnees();
    bool enregistrerTransaction(const QString& typeOperation, double montant,
                                const QString& compteSource, const QString& compteDest,
                                const QString& motif);

    // Variables membres
    QString m_utilisateur_id;
    bool m_soldeVisibleCompteCourant;
    bool m_soldeVisibleCompteEpargne;
    AnimationSolde* m_soldeAnimation;
    Banque m_banque;
    CreationBD& m_creationBD;
    QString m_boutonActif;
};

#endif // FENMAIN_H
