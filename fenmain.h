#ifndef FENMAIN_H
#define FENMAIN_H

#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QEvent>
#include <QList>
#include <QSqlTableModel>

#include "animationsolde.h"
#include "comptecourant.h"
#include "compteepargne.h"
#include "creationbd.h"
#include "banque.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class fenMain;
}
QT_END_NAMESPACE

class fenMain : public QMainWindow
{
    Q_OBJECT

public:
    fenMain(QWidget *parent = nullptr);
    ~fenMain();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
private slots:
    void on_btn_dashboard_barre_latterale_clicked();
    void on_btn_masquer_solde_compte_courant_clicked();
    void on_btn_masquer_solde_compte_epargne_clicked();
    void on_btn_historique_barre_latterale_clicked();
    void on_btn_parametres_barre_latterale_clicked();

    void on_btn_consulter_compte_epargne_clicked();

    void on_btn_consulter_compte_courant_clicked();

    void on_btn_effectuer_transaction_compte_epargne_clicked();

    void on_btn_effectuer_transaction_compte_courant_clicked();

    void on_btn_voir_liste_complete_transaction_clicked();

    void on_btn_valider_transaction_clicked();

    void on_btn_supprimer_transaction_clicked();

    void on_btn_modifier_la_transaction_clicked();

private:
    Ui::fenMain *ui;
    AnimationSolde *m_soldeAnimation;

    QString m_boutonActif;
    bool m_soldeVisibleCompteCourant;
    bool m_soldeVisibleCompteEpargne;

    // Ajout des membres pour la base de données et la banque
    CreationBD m_gestionBD;
    Banque m_banque;

    // Comptes spécifiques (pour un accès rapide)
    CompteCourant* m_compteCourant = nullptr;
    CompteEpargne* m_compteEpargne = nullptr;

    void appliquerEffetFlou(QLabel* label, bool masquer);
    void mettreAJourStyleBoutonsLateraux();
    void mettreAjourIcon(QToolButton* button, bool visible);
    void appliquerStyleBoutonMasquage(QToolButton* button, bool survole);

    // Méthodes pour charger et afficher les données
    void chargerDonneesDepuisBD();
    void afficherCompteCourant();
    void afficherCompteEpargne();

    QSqlTableModel* m_modelTransactions;

    void initialiserTableTransactions();
    void mettreAJourSoldesApresSuppression(int idTransaction);
    void mettreAJourSoldesApresModification(int idTransaction, double ancienMontant, double nouveauMontant);
};

#endif // FENMAIN_H
