#include "fenmain.h"
#include "ui_fenmain.h"
#include <QApplication>
#include <QEvent>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDateTime>
#include "comptecourant.h"
#include "compteepargne.h"
#include <QFormLayout>
#include <QStandardPaths>
#include <QDir>

fenMain::fenMain(CreationBD& m_BD, QWidget *parent, const QString &utilisateur_id)
    : QMainWindow(parent)
    , ui(new Ui::fenMain)
    , m_utilisateur_id(utilisateur_id)
    , m_soldeVisibleCompteCourant(true)
    , m_soldeVisibleCompteEpargne(true)
    , m_soldeAnimation(new AnimationSolde(this))
    , m_banque("MyBank")
    , m_creationBD(m_BD)
{
    ui->setupUi(this);
    qApp->installEventFilter(this);

    this->showMaximized();

    ui->mes_pages->setCurrentWidget(ui->page_dashboard);
    ui->btn_masquer_solde_compte_courant->installEventFilter(this);
    ui->btn_masquer_solde_compte_epargne->installEventFilter(this);

    mettreAJourStyleBoutonsLateraux();

    // Initialiser le style par défaut des boutons de masquage
    appliquerStyleBoutonMasquage(ui->btn_masquer_solde_compte_courant, false);
    appliquerStyleBoutonMasquage(ui->btn_masquer_solde_compte_epargne, false);

    // Initialiser les icônes
    mettreAjourIcon(ui->btn_masquer_solde_compte_courant, m_soldeVisibleCompteCourant);
    mettreAjourIcon(ui->btn_masquer_solde_compte_epargne, m_soldeVisibleCompteEpargne);

    chargerDonneesDepuisBD();

    // Appliquer l'effet flou initial
    appliquerEffetFlou(ui->label_solde_compte_courant, !m_soldeVisibleCompteCourant);
    appliquerEffetFlou(ui->label_solde_compte_epargne, !m_soldeVisibleCompteEpargne);
}

fenMain::~fenMain()
{
    sauvegarderDonnees();
    delete ui;
}

void fenMain::chargerDonneesDepuisBD()
{
    chargerInformationsUtilisateur();
    chargerComptesBancaires();
    mettreAJourAffichageComptes();
}

void fenMain::chargerInformationsUtilisateur()
{
    if (!m_creationBD.estOuverte()) {
        qWarning() << "Base de données non ouverte!";
        return;
    }

    QSqlDatabase db = m_creationBD.getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT nom_complet FROM utilisateurs WHERE id = ?");
    query.addBindValue(m_utilisateur_id);

    if (query.exec() && query.next()) {
        QString nomComplet = query.value("nom_complet").toString();
        ui->labelBienvenue->setText("Bienvenue, " + nomComplet + " !");
    } else {
        qWarning() << "Erreur récupération nom utilisateur:" << query.lastError().text();
        ui->labelBienvenue->setText("Bienvenue !");
    }
}

void fenMain::chargerComptesBancaires()
{
    if (!m_creationBD.estOuverte()) {
        qWarning() << "Base de données non ouverte!";
        return;
    }

    // Vider les comptes existants
    for (CompteBancaire* compte : m_banque.getComptes()) {
        delete compte;
    }
    m_banque.getComptes().clear();

    QSqlDatabase db = m_creationBD.getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM comptes WHERE id_utilisateur = ?");
    query.addBindValue(m_utilisateur_id);

    if (!query.exec()) {
        qWarning() << "Erreur récupération comptes:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString numeroCompte = query.value("numero_compte").toString();
        QString nomTitulaire = query.value("nom_titulaire").toString();
        double solde = query.value("solde").toDouble();
        QString typeCompte = query.value("type_compte").toString();
        QString dateCreation = query.value("date_creation").toString();
        QString derniereOperation = query.value("derniere_operation").toString();

        CompteBancaire* compte = nullptr;

        if (typeCompte == "courant") {
            double decouvert = query.value("decouvert_autorise").toDouble();
            compte = new CompteCourant(numeroCompte, nomTitulaire, solde, decouvert);
        } else if (typeCompte == "epargne") {
            double taux = query.value("taux_interet").toDouble();
            compte = new CompteEpargne(numeroCompte, nomTitulaire, solde, taux);
        }

        if (compte) {
            compte->setDateCreation(dateCreation);
            compte->setDerniereOperation(derniereOperation);
            m_banque.ajouterCompte(compte);
        }
    }
}

void fenMain::sauvegarderDonnees()
{
    if (!m_creationBD.estOuverte()) {
        qWarning() << "Base de données non ouverte! Impossible de sauvegarder.";
        return;
    }

    QSqlDatabase db = m_creationBD.getDatabase();
    QSqlQuery query(db);

    // Sauvegarde des comptes
    for (CompteBancaire* compte : m_banque.getComptes()) {
        query.prepare("UPDATE comptes SET "
                      "solde = ?, "
                      "derniere_operation = ? "
                      "WHERE numero_compte = ?");

        query.addBindValue(compte->getSolde());
        query.addBindValue(compte->getDerniereOperation());
        query.addBindValue(compte->getNumeroCompte());

        if (!query.exec()) {
            qWarning() << "Erreur sauvegarde compte"
                       << compte->getNumeroCompte()
                       << ":" << query.lastError().text();
        }
    }
}

CompteCourant* fenMain::getCompteCourant() const
{
    for (CompteBancaire* compte : m_banque.getComptes()) {
        if (CompteCourant* cc = dynamic_cast<CompteCourant*>(compte)) {
            return cc;
        }
    }
    return nullptr;
}

CompteEpargne* fenMain::getCompteEpargne() const
{
    for (CompteBancaire* compte : m_banque.getComptes()) {
        if (CompteEpargne* ce = dynamic_cast<CompteEpargne*>(compte)) {
            return ce;
        }
    }
    return nullptr;
}

void fenMain::mettreAJourAffichageComptes()
{
    // Compte Courant
    if (CompteCourant* compteCourant = getCompteCourant()) {
        ui->label_solde_compte_courant->setText(
            QString::number(compteCourant->getSolde(), 'f', 2) + " FCFA");

        ui->label_decouvert_autorise_compte_courant->setText(
            QString::number(compteCourant->getDecouvertAutorise(), 'f', 2) + " FCFA");

        ui->label_numero_de_compte_courant->setText(
            compteCourant->getNumeroCompte());

        ui->label_date_creation_compte_courant->setText(
            compteCourant->getDateCreation());

        ui->label_derniere_transaction_compte_courant->setText(
            compteCourant->getDerniereOperation());
    } else {
        ui->label_solde_compte_courant->setText("0.00 FCFA");
        ui->label_decouvert_autorise_compte_courant->setText("0.00 FCFA");
        ui->label_numero_de_compte_courant->setText("N/A");
        ui->label_date_creation_compte_courant->setText("N/A");
        ui->label_derniere_transaction_compte_courant->setText("N/A");
    }

    // Compte Épargne
    if (CompteEpargne* compteEpargne = getCompteEpargne()) {
        ui->label_solde_compte_epargne->setText(
            QString::number(compteEpargne->getSolde(), 'f', 2) + " FCFA");

        ui->label_taux_interet_compte_epargne->setText(
            QString::number(compteEpargne->getTauxInteret(), 'f', 2) + "%");

        ui->label_numero_de_compte_epargne->setText(
            compteEpargne->getNumeroCompte());

        ui->label_date_creation_compte_epargne->setText(
            compteEpargne->getDateCreation());

        ui->label_derniere_transaction_compte_epargne->setText(
            compteEpargne->getDerniereOperation());
    } else {
        ui->label_solde_compte_epargne->setText("0.00 FCFA");
        ui->label_taux_interet_compte_epargne->setText("0.00%");
        ui->label_numero_de_compte_epargne->setText("N/A");
        ui->label_date_creation_compte_epargne->setText("N/A");
        ui->label_derniere_transaction_compte_epargne->setText("N/A");
    }
}

bool fenMain::eventFilter(QObject* obj, QEvent* event)
{
    // Gestion des événements de survol pour les boutons de masquage
    if (obj == ui->btn_masquer_solde_compte_courant ||
        obj == ui->btn_masquer_solde_compte_epargne) {

        QToolButton* button = qobject_cast<QToolButton*>(obj);
        if (!button) return QMainWindow::eventFilter(obj, event);

        if (event->type() == QEvent::Enter) {
            appliquerStyleBoutonMasquage(button, true);
            bool visible = (button == ui->btn_masquer_solde_compte_courant) ?
                               m_soldeVisibleCompteCourant : m_soldeVisibleCompteEpargne;
            mettreAjourIcon(button, visible);
            return true;
        }
        else if (event->type() == QEvent::Leave) {
            appliquerStyleBoutonMasquage(button, false);
            bool visible = (button == ui->btn_masquer_solde_compte_courant) ?
                               m_soldeVisibleCompteCourant : m_soldeVisibleCompteEpargne;
            mettreAjourIcon(button, visible);
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void fenMain::mettreAJourStyleBoutonsLateraux()
{
    QString styleBase =
        "QToolButton { "
        "padding: 6px 12px; "
        "border-radius: 4px; "
        "border: none; "
        "color: #4A4A4A; "
        "font-weight: 500; "
        "min-height: 18px; "
        "text-align: left; "
        "background-color: transparent; "
        "}"
        "QToolButton:hover{ "
        "background-color: rgba(41, 98, 255,0.2); "
        "}";

    QString styleActif =
        "QToolButton { "
        "background-color: rgb(41, 98, 255); "
        "padding: 6px 12px; "
        "border-radius: 4px; "
        "border: none; "
        "color: rgb(255, 255, 255); "
        "font-weight: 500; "
        "min-height: 18px; "
        "}";

    ui->btn_dashboard_barre_latterale->setStyleSheet(styleBase);
    ui->btn_historique_barre_latterale->setStyleSheet(styleBase);
    ui->btn_parametres_barre_latterale->setStyleSheet(styleBase);

    if (ui->mes_pages->currentWidget() == ui->page_dashboard) {
        ui->btn_dashboard_barre_latterale->setStyleSheet(styleActif);
        m_boutonActif = "dashboard";
    }
    else if (ui->mes_pages->currentWidget() == ui->page_historique) {
        ui->btn_historique_barre_latterale->setStyleSheet(styleActif);
        m_boutonActif = "historique";
    }
    else if (ui->mes_pages->currentWidget() == ui->page_parametres) {
        ui->btn_parametres_barre_latterale->setStyleSheet(styleActif);
        m_boutonActif = "parametres";
    }
}

void fenMain::appliquerStyleBoutonMasquage(QToolButton* button, bool survole)
{
    if (!button) return;

    QString style;
    if (survole) {
        style = "QToolButton { "
                "background-color: rgb(41, 98, 255); "
                "border-radius: 4px; "
                "border: none; "
                "padding: 6px 12px; "
                "}";
    } else {
        style = "QToolButton { "
                "background-color: transparent; "
                "border: none; "
                "border-radius: 4px; "
                "padding: 6px 12px; "
                "}";
    }

    button->setStyleSheet(style);
}

void fenMain::appliquerEffetFlou(QLabel* label, bool masquer)
{
    if (m_soldeAnimation) {
        m_soldeAnimation->appliquerAvecLabel(label, masquer);
    }
}

void fenMain::mettreAjourIcon(QToolButton* button, bool visible)
{
    if (!button) return;

    bool isHovered = button->underMouse();
    QString basePath = isHovered ? ":/icon_blanc/" : ":/icon_gris/";
    QString iconName = visible ? "eye-off.svg" : "eye.svg";

    QIcon icon(basePath + iconName);
    button->setIcon(icon);
    button->setIconSize(QSize(15, 15));
    button->setToolTip(visible ? "Masquer le solde" : "Afficher le solde");

    button->style()->unpolish(button);
    button->style()->polish(button);
    button->update();
}

void fenMain::on_btn_masquer_solde_compte_courant_clicked()
{
    m_soldeVisibleCompteCourant = !m_soldeVisibleCompteCourant;
    appliquerEffetFlou(ui->label_solde_compte_courant, !m_soldeVisibleCompteCourant);
    mettreAjourIcon(ui->btn_masquer_solde_compte_courant, m_soldeVisibleCompteCourant);
}

void fenMain::on_btn_masquer_solde_compte_epargne_clicked()
{
    m_soldeVisibleCompteEpargne = !m_soldeVisibleCompteEpargne;
    appliquerEffetFlou(ui->label_solde_compte_epargne, !m_soldeVisibleCompteEpargne);
    mettreAjourIcon(ui->btn_masquer_solde_compte_epargne, m_soldeVisibleCompteEpargne);
}

void fenMain::on_btn_dashboard_barre_latterale_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_dashboard);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_historique_barre_latterale_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_parametres_barre_latterale_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_parametres);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_consulter_compte_epargne_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_consulter_compte_courant_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_effectuer_transaction_compte_courant_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_effectuer_transaction_compte_epargne_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_voir_liste_complete_transaction_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_valider_transaction_clicked()
{
    // Exemple simplifié de transaction
    QMessageBox::information(this, "Transaction", "Transaction effectuée avec succès!");

    // Mettre à jour les données en mémoire
    if (CompteCourant* compte = getCompteCourant()) {
        compte->deposer(100.0); // Exemple
        compte->setDerniereOperation(QDateTime::currentDateTime().toString());
    }

    // Rafraîchir l'affichage
    mettreAJourAffichageComptes();
}

void fenMain::on_btn_supprimer_transaction_clicked()
{
    // Logique de suppression de transaction
    QMessageBox::information(this, "Suppression", "Transaction supprimée!");

    // Rafraîchir l'affichage
    mettreAJourAffichageComptes();
}

void fenMain::on_btn_modifier_la_transaction_clicked()
{
    // Logique de modification de transaction
    QMessageBox::information(this, "Modification", "Transaction modifiée!");

    // Rafraîchir l'affichage
    mettreAJourAffichageComptes();
}


