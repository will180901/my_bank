#ifndef FENMAIN_H
#define FENMAIN_H

#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QEvent>

#include "animationsolde.h"

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

private:
    Ui::fenMain *ui;
    AnimationSolde *m_soldeAnimation;

    QString m_boutonActif;
    bool m_soldeVisibleCompteCourant;
    bool m_soldeVisibleCompteEpargne;

    void appliquerEffetFlou(QLabel* label, bool masquer);
    void mettreAJourStyleBoutonsLateraux();
    void mettreAjourIcon(QToolButton* button, bool visible);
    void appliquerStyleBoutonMasquage(QToolButton* button, bool survole);
};

#endif // FENMAIN_H
