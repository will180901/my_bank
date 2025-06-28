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
#include <QDialog>
#include <QPushButton>
#include <QGraphicsBlurEffect>
#include <functional>

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
    fenMain(CreationBD& m_BD, QWidget *parent = nullptr, const QString &utilisateur_id = "");
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
    void on_btn_creer_compte_courant_clicked();
    void on_btn_creer_compte_epargne_clicked();

private:
    Ui::fenMain *ui;
    AnimationSolde *m_soldeAnimation;

    CreationBD & m_creationBD;

    QString m_boutonActif;
    bool m_soldeVisibleCompteCourant;
    bool m_soldeVisibleCompteEpargne;
    QString m_utilisateur_id;

    Banque m_banque;

    QWidget* m_rideauCompteCourant;
    QWidget* m_rideauCompteEpargne;

    // Méthodes d'interface utilisateur
    void appliquerEffetFlou(QLabel* label, bool masquer);
    void mettreAJourStyleBoutonsLateraux();
    void mettreAjourIcon(QToolButton* button, bool visible);
    void appliquerStyleBoutonMasquage(QToolButton* button, bool survole);
    void appliquerEffetFlouCompte(QWidget* widgetCarte, bool appliquerFlou);
    void mettreAJourApparenceComptes();
    void creerRideau(QWidget* widgetParent, QWidget*& rideau, const QString& message, std::function<void()> callback);

    // Méthodes de gestion des données
    void chargerDonneesDepuisBD();
    void chargerInformationsUtilisateur();
    void chargerComptesBancaires();
    void sauvegarderDonnees();
    void mettreAJourAffichageComptes();

    // Accesseurs aux comptes
    CompteCourant* getCompteCourant() const;
    CompteEpargne* getCompteEpargne() const;

    // Méthodes de création de comptes
    void creerCompteCourant();
    void creerCompteEpargne();
    QString genererNumeroCompte(const QString& typeCompte);
    void creerCompteCourantEnBD();
    void creerCompteEpargneEnBD();
};

#endif // FENMAIN_H
