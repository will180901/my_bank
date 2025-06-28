#ifndef FENMAIN_H
#define FENMAIN_H

#include "Banque.h"
#include "CreationBD.h"
#include "AnimationSolde.h"
#include "comptebancaire.h"
#include "comptecourant.h"  // Ajouté
#include "compteepargne.h"  // Ajouté

#include <QMainWindow>
#include <QToolButton>
#include <QStackedWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextEdit>
#include <QLineEdit>

namespace Ui {
class fenMain;
}

class fenMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit fenMain(CreationBD& m_BD, QWidget *parent = nullptr, const QString &utilisateur_id = QString());
    ~fenMain();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    // Slots pour les transactions
    void on_btn_valider_transaction_clicked();
    // Les slots pour les boutons depot/retrait ont été supprimés car non implémentés dans le .cpp

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

    // Méthodes pour les transactions
    void effectuerDepot(CompteBancaire* compte, double montant, const QString& motif);
    void effectuerRetrait(CompteBancaire* compte, double montant, const QString& motif);
    void effectuerVirement(const QString& compteSource, const QString& compteDest, double montant, const QString& motif);
    void sauvegarderCompte(CompteBancaire* compte);
    void mettreAJourAffichageComptes();


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
