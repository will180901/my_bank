#include "fenmain.h"
#include "ui_fenmain.h"
#include <QApplication>
#include <QEvent>
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDateTime>
#include <QGridLayout>
#include <QUuid>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QMouseEvent>
#include <QToolButton>

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
    this->installEventFilter(this);



    qApp->installEventFilter(this);
    qApp->processEvents();

    this->showMaximized();


    ui->comboBox_type_operation_onglet_depot_retrait->clear();
    ui->comboBox_type_operation_onglet_depot_retrait->addItem("Dépôt");
    ui->comboBox_type_operation_onglet_depot_retrait->addItem("Retrait");
    ui->comboBox_type_operation_onglet_depot_retrait->setCurrentIndex(0);

    ui->mes_pages->setCurrentWidget(ui->page_dashboard);
    ui->btn_masquer_solde_compte_courant->installEventFilter(this);
    ui->btn_masquer_solde_compte_epargne->installEventFilter(this);

    mettreAJourStyleBoutonsLateraux();

    appliquerStyleBoutonMasquage(ui->btn_masquer_solde_compte_courant, false);
    appliquerStyleBoutonMasquage(ui->btn_masquer_solde_compte_epargne, false);

    mettreAjourIcon(ui->btn_masquer_solde_compte_courant, m_soldeVisibleCompteCourant);
    mettreAjourIcon(ui->btn_masquer_solde_compte_epargne, m_soldeVisibleCompteEpargne);

    chargerDonneesDepuisBD();
    mettreAJourAffichageComptes();
}



fenMain::~fenMain()
{
    // Sauvegarder avant de détruire
    sauvegarderDonnees();

    if (m_soldeAnimation) {
        delete m_soldeAnimation;
        m_soldeAnimation = nullptr;
    }

    delete ui;
}




bool fenMain::eventFilter(QObject* obj, QEvent* event)
{
    // Gestion des boutons de masquage (code existant inchangé)
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




void fenMain::sauvegarderDonnees()
{
    if (!m_creationBD.estOuverte()) {
        qWarning() << "Base de données non ouverte! Impossible de sauvegarder.";
        return;
    }

    QSqlDatabase db = m_creationBD.getDatabase();
    QSqlQuery query(db);

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



void fenMain::creerCompteCourant()
{
    if (getCompteCourant() != nullptr) {
        QMessageBox::information(this, "Information", "Un compte courant existe déjà!");
        return;
    }

    creerCompteCourantEnBD();
    chargerComptesBancaires();
    mettreAJourAffichageComptes();

    QMessageBox::information(this, "Compte créé", "Votre compte courant a été créé avec succès !");
}

void fenMain::creerCompteEpargne()
{
    if (getCompteEpargne() != nullptr) {
        QMessageBox::information(this, "Information", "Un compte épargne existe déjà!");
        return;
    }

    creerCompteEpargneEnBD();
    chargerComptesBancaires();
    mettreAJourAffichageComptes();

    QMessageBox::information(this, "Compte créé", "Votre compte épargne a été créé avec succès !");
}

QString fenMain::genererNumeroCompte(const QString& typeCompte)
{
    QString prefixe = (typeCompte == "courant") ? "CC" : "CE";
    QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).remove('-').left(10);
    return prefixe + uuid.toUpper();
}

void fenMain::creerCompteCourantEnBD()
{
    if (!m_creationBD.estOuverte()) {
        qWarning() << "Base de données non ouverte!";
        return;
    }

    QSqlDatabase db = m_creationBD.getDatabase();
    QSqlQuery query(db);

    QString numeroCompte = genererNumeroCompte("courant");
    QString dateCreation = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm");
    QString derniereOperation = "Création de compte - " + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm");
    double decouvertAutorise = 500.0;

    // Récupérer le nom de l'utilisateur
    query.prepare("SELECT nom_complet FROM utilisateurs WHERE id = ?");
    query.addBindValue(m_utilisateur_id);

    QString nomTitulaire = "Utilisateur";
    if (query.exec() && query.next()) {
        nomTitulaire = query.value("nom_complet").toString();
    }

    // Insérer le nouveau compte
    query.prepare("INSERT INTO comptes (numero_compte, nom_titulaire, solde, type_compte, "
                  "date_creation, derniere_operation, id_utilisateur, decouvert_autorise, id_banque) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, 1)");

    query.addBindValue(numeroCompte);
    query.addBindValue(nomTitulaire);
    query.addBindValue(0.0);
    query.addBindValue("courant");
    query.addBindValue(dateCreation);
    query.addBindValue(derniereOperation);
    query.addBindValue(m_utilisateur_id);
    query.addBindValue(decouvertAutorise);

    if (!query.exec()) {
        qWarning() << "Erreur création compte courant:" << query.lastError().text();
        QMessageBox::critical(this, "Erreur", "Erreur création compte courant: " + query.lastError().text());
    }
}


void fenMain::creerCompteEpargneEnBD()
{
    if (!m_creationBD.estOuverte()) {
        qWarning() << "Base de données non ouverte!";
        return;
    }

    QSqlDatabase db = m_creationBD.getDatabase();
    QSqlQuery query(db);

    QString numeroCompte = genererNumeroCompte("epargne");
    QString dateCreation = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm");
    QString derniereOperation = "Création de compte - " + QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm");
    double tauxInteret = 2.5;

    // Récupérer le nom de l'utilisateur
    query.prepare("SELECT nom_complet FROM utilisateurs WHERE id = ?");
    query.addBindValue(m_utilisateur_id);

    QString nomTitulaire = "Utilisateur";
    if (query.exec() && query.next()) {
        nomTitulaire = query.value("nom_complet").toString();
    }

    // Insérer le nouveau compte
    query.prepare("INSERT INTO comptes (numero_compte, nom_titulaire, solde, type_compte, "
                  "date_creation, derniere_operation, id_utilisateur, taux_interet, id_banque) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, 1)");

    query.addBindValue(numeroCompte);
    query.addBindValue(nomTitulaire);
    query.addBindValue(0.0);
    query.addBindValue("epargne");
    query.addBindValue(dateCreation);
    query.addBindValue(derniereOperation);
    query.addBindValue(m_utilisateur_id);
    query.addBindValue(tauxInteret);

    if (!query.exec()) {
        qWarning() << "Erreur création compte épargne:" << query.lastError().text();
        QMessageBox::critical(this, "Erreur", "Erreur création compte épargne: " + query.lastError().text());
    }
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

    // Vider les comptes existants avant de recharger
    m_banque.viderComptes();


    QSqlDatabase db = m_creationBD.getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM comptes WHERE id_utilisateur = ? AND est_actif = 1");
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

        // CORRECTION: Validation des valeurs avant création
        if (typeCompte == "courant") {
            double decouvert = query.value("decouvert_autorise").toDouble();
            if (decouvert < 0) decouvert = 0; // Protection contre valeurs négatives
            compte = new CompteCourant(numeroCompte, nomTitulaire, solde, decouvert);
        } else if (typeCompte == "epargne") {
            double taux = query.value("taux_interet").toDouble();
            if (taux < 0) taux = 0; // Protection contre valeurs négatives
            compte = new CompteEpargne(numeroCompte, nomTitulaire, solde, taux);
        }

        if (compte) {
            compte->setDateCreation(dateCreation);
            compte->setDerniereOperation(derniereOperation);
            m_banque.ajouterCompte(compte);
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



void fenMain::sauvegarderCompte(CompteBancaire* compte)
{
    if (!compte || !m_creationBD.estOuverte()) return;

    QSqlDatabase db = m_creationBD.getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE comptes SET solde = ?, derniere_operation = ? WHERE numero_compte = ?");
    query.addBindValue(compte->getSolde());
    query.addBindValue(compte->getDerniereOperation());
    query.addBindValue(compte->getNumeroCompte());

    if (!query.exec()) {
        qWarning() << "Erreur sauvegarde compte" << compte->getNumeroCompte() << ":" << query.lastError().text();
    }
}



bool fenMain::enregistrerTransaction(const QString& typeOperation, double montant,
                                     const QString& compteSource, const QString& compteDest,
                                     const QString& motif)
{
    if (!m_creationBD.estOuverte()) {
        return false;
    }

    QSqlDatabase db = m_creationBD.getDatabase();
    QSqlQuery query(db);

    // Récupérer les IDs des comptes
    query.prepare("SELECT id FROM comptes WHERE numero_compte = ?");
    query.addBindValue(compteSource);

    if (!query.exec() || !query.next()) {
        qWarning() << "Compte source non trouvé pour transaction";
        return false;
    }
    int idCompteSource = query.value(0).toInt();

    int idCompteBeneficiaire = -1;
    if (!compteDest.isEmpty()) {
        query.prepare("SELECT id FROM comptes WHERE numero_compte = ?");
        query.addBindValue(compteDest);

        if (query.exec() && query.next()) {
            idCompteBeneficiaire = query.value(0).toInt();
        }
    }

    // Insérer la transaction
    if (idCompteBeneficiaire > 0) {
        query.prepare("INSERT INTO transactions (type_operation, montant, id_compte_source, "
                      "id_compte_beneficiaire, date_operation, libelle, statut) "
                      "VALUES (?, ?, ?, ?, ?, ?, 'valide')");
        query.addBindValue(typeOperation);
        query.addBindValue(montant);
        query.addBindValue(idCompteSource);
        query.addBindValue(idCompteBeneficiaire);
        query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
        query.addBindValue(motif);
    } else {
        query.prepare("INSERT INTO transactions (type_operation, montant, id_compte_source, "
                      "date_operation, libelle, statut) "
                      "VALUES (?, ?, ?, ?, ?, 'valide')");
        query.addBindValue(typeOperation);
        query.addBindValue(montant);
        query.addBindValue(idCompteSource);
        query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
        query.addBindValue(motif);
    }

    if (!query.exec()) {
        qWarning() << "Erreur enregistrement transaction:" << query.lastError().text();
        return false;
    }

    return true;
}




void fenMain::on_btn_valider_transaction_clicked()
{
    int indexOnglet = ui->mes_onglets_page_transaction->currentIndex();

    if (indexOnglet == 0) { // Onglet dépôt/retrait
        QString numeroCompte = ui->sai_numero_compte_onglet_depot_retrait->text().trimmed();
        double montant = ui->doubleSpinBox_montant_onglet_depot_retrait->value();
        QString motif = ui->textEdit_motif_onglet_depot_retrait->toPlainText().trimmed();
        QString typeOperation = ui->comboBox_type_operation_onglet_depot_retrait->currentText();

        // CORRECTION: Validation des entrées
        if (numeroCompte.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Veuillez saisir un numéro de compte!");
            ui->sai_numero_compte_onglet_depot_retrait->setFocus();
            return;
        }

        if (montant <= 0) {
            QMessageBox::warning(this, "Erreur", "Veuillez saisir un montant valide!");
            ui->doubleSpinBox_montant_onglet_depot_retrait->setFocus();
            return;
        }

        CompteBancaire* compte = m_banque.trouverCompte(numeroCompte);
        if (!compte) {
            QMessageBox::warning(this, "Erreur", "Compte introuvable!");
            ui->sai_numero_compte_onglet_depot_retrait->setFocus();
            return;
        }

        if (typeOperation == "Dépôt") {
            effectuerDepot(compte, montant, motif);
        }
        else if (typeOperation == "Retrait") {
            effectuerRetrait(compte, montant, motif);
        }
        else {
            QMessageBox::warning(this, "Erreur", "Type d'opération invalide!");
            return;
        }

    } else if (indexOnglet == 1) { // Onglet virement
        QString compteSource = ui->sai_numero_compte_source_onglet_virement->text().trimmed();
        QString compteDest = ui->sai_numero_compte_beneficiaire_onglet_virement->text().trimmed();
        double montant = ui->doubleSpinBox_montant_onglet_virement->value();
        QString motif = ui->textEdit_motif_onglet_virement->toPlainText().trimmed();

        // CORRECTION: Validation des entrées
        if (compteSource.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Veuillez saisir le compte source!");
            ui->sai_numero_compte_source_onglet_virement->setFocus();
            return;
        }

        if (compteDest.isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Veuillez saisir le compte bénéficiaire!");
            ui->sai_numero_compte_beneficiaire_onglet_virement->setFocus();
            return;
        }

        if (montant <= 0) {
            QMessageBox::warning(this, "Erreur", "Veuillez saisir un montant valide!");
            ui->doubleSpinBox_montant_onglet_virement->setFocus();
            return;
        }

        effectuerVirement(compteSource, compteDest, montant, motif);
    }

    // Réinitialiser les champs après opération réussie
    ui->sai_numero_compte_onglet_depot_retrait->clear();
    ui->doubleSpinBox_montant_onglet_depot_retrait->setValue(0);
    ui->textEdit_motif_onglet_depot_retrait->clear();
    ui->sai_numero_compte_source_onglet_virement->clear();
    ui->sai_numero_compte_beneficiaire_onglet_virement->clear();
    ui->doubleSpinBox_montant_onglet_virement->setValue(0);
    ui->textEdit_motif_onglet_virement->clear();
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
    ui->mes_onglets_page_transaction->setCurrentWidget(ui->onglet_depot_retrait);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_effectuer_transaction_compte_epargne_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);
    ui->mes_onglets_page_transaction->setCurrentWidget(ui->onglet_depot_retrait);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_voir_liste_complete_transaction_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_ajouter_compte_courant_clicked()
{
    creerCompteCourant();
}

void fenMain::on_btn_ajouter_compte_epargne_clicked()
{
    creerCompteEpargne();
}






void fenMain::effectuerDepot(CompteBancaire* compte, double montant, const QString& motif)
{
    // CORRECTION: Validation complète des paramètres
    if (!compte) {
        QMessageBox::warning(this, "Erreur", "Compte invalide!");
        return;
    }

    if (montant <= 0 || montant > 999999999.99) { // Limite raisonnable
        QMessageBox::warning(this, "Erreur", "Montant invalide! (0 < montant ≤ 999,999,999.99)");
        return;
    }

    // Vérification du débordement
    double nouveauSolde = compte->getSolde() + montant;
    if (nouveauSolde > 999999999.99) {
        QMessageBox::warning(this, "Erreur", "Le solde dépasserait la limite autorisée!");
        return;
    }

    compte->deposer(montant);

    // Mettre à jour dernière opération avec timestamp
    QString timestamp = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    QString operation = QString("Dépôt: +%1 FCFA - %2 [%3]")
                            .arg(QString::number(montant, 'f', 2))
                            .arg(motif.isEmpty() ? "Sans motif" : motif)
                            .arg(timestamp);
    compte->setDerniereOperation(operation);

    // Enregistrer la transaction en base
    if (!enregistrerTransaction("depot", montant, compte->getNumeroCompte(), QString(), motif)) {
        qWarning() << "Erreur lors de l'enregistrement de la transaction";
    }

    sauvegarderCompte(compte);
    mettreAJourAffichageComptes();
    QMessageBox::information(this, "Succès", "Dépôt effectué avec succès!");
}







void fenMain::effectuerRetrait(CompteBancaire* compte, double montant, const QString& motif)
{
    // CORRECTION: Validation complète des paramètres
    if (!compte) {
        QMessageBox::warning(this, "Erreur", "Compte invalide!");
        return;
    }

    if (montant <= 0 || montant > 999999999.99) {
        QMessageBox::warning(this, "Erreur", "Montant invalide! (0 < montant ≤ 999,999,999.99)");
        return;
    }

    // Tentative de retrait avec vérification du type de compte
    bool retraitReussi = false;
    QString messageErreur;

    if (CompteCourant* cc = dynamic_cast<CompteCourant*>(compte)) {
        if ((compte->getSolde() - montant) >= -cc->getDecouvertAutorise()) {
            retraitReussi = cc->retirer(montant);
        } else {
            messageErreur = QString("Découvert autorisé dépassé! Limite: %1 FCFA")
                                .arg(QString::number(cc->getDecouvertAutorise(), 'f', 2));
        }
    } else if (CompteEpargne* ce = dynamic_cast<CompteEpargne*>(compte)) {
        if (montant <= compte->getSolde()) {
            retraitReussi = ce->retirer(montant);
        } else {
            messageErreur = "Solde insuffisant pour un compte épargne!";
        }
    } else {
        messageErreur = "Type de compte non reconnu!";
    }

    if (retraitReussi) {
        // Mettre à jour dernière opération avec timestamp
        QString timestamp = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
        QString operation = QString("Retrait: -%1 FCFA - %2 [%3]")
                                .arg(QString::number(montant, 'f', 2))
                                .arg(motif.isEmpty() ? "Sans motif" : motif)
                                .arg(timestamp);
        compte->setDerniereOperation(operation);

        // Enregistrer la transaction en base
        if (!enregistrerTransaction("retrait", montant, compte->getNumeroCompte(), QString(), motif)) {
            qWarning() << "Erreur lors de l'enregistrement de la transaction";
        }

        sauvegarderCompte(compte);
        mettreAJourAffichageComptes();
        QMessageBox::information(this, "Succès", "Retrait effectué avec succès!");
    } else {
        QMessageBox::warning(this, "Erreur",
                             messageErreur.isEmpty() ? "Opération impossible!" : messageErreur);
    }
}










void fenMain::effectuerVirement(const QString& compteSource, const QString& compteDest, double montant, const QString& motif)
{
    // CORRECTION: Validation des paramètres
    if (compteSource.isEmpty() || compteDest.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Numéros de compte manquants!");
        return;
    }

    if (compteSource == compteDest) {
        QMessageBox::warning(this, "Erreur", "Impossible de virer vers le même compte!");
        return;
    }

    if (montant <= 0 || montant > 999999999.99) {
        QMessageBox::warning(this, "Erreur", "Montant invalide!");
        return;
    }

    CompteBancaire* source = m_banque.trouverCompte(compteSource);
    CompteBancaire* dest = m_banque.trouverCompte(compteDest);

    if (!source) {
        QMessageBox::warning(this, "Erreur", "Compte source introuvable!");
        return;
    }

    if (!dest) {
        QMessageBox::warning(this, "Erreur", "Compte bénéficiaire introuvable!");
        return;
    }

    // Vérifier les limites selon le type de compte source
    bool virementPossible = false;
    QString messageErreur;

    if (CompteCourant* cc = dynamic_cast<CompteCourant*>(source)) {
        if ((source->getSolde() - montant) >= -cc->getDecouvertAutorise()) {
            virementPossible = true;
        } else {
            messageErreur = "Découvert autorisé dépassé!";
        }
    } else if (dynamic_cast<CompteEpargne*>(source)) {
        if (montant <= source->getSolde()) {
            virementPossible = true;
        } else {
            messageErreur = "Solde insuffisant!";
        }
    }

    // Vérifier débordement côté destination
    if (virementPossible && (dest->getSolde() + montant > 999999999.99)) {
        virementPossible = false;
        messageErreur = "Le virement dépasserait la limite du compte bénéficiaire!";
    }

    if (virementPossible && m_banque.effectuerVirement(compteSource, compteDest, montant)) {
        // Mettre à jour dernières opérations avec timestamp
        QString timestamp = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
        QString opSource = QString("Virement sortant: -%1 FCFA vers %2 - %3 [%4]")
                               .arg(QString::number(montant, 'f', 2))
                               .arg(compteDest)
                               .arg(motif.isEmpty() ? "Sans motif" : motif)
                               .arg(timestamp);
        QString opDest = QString("Virement entrant: +%1 FCFA de %2 - %3 [%4]")
                             .arg(QString::number(montant, 'f', 2))
                             .arg(compteSource)
                             .arg(motif.isEmpty() ? "Sans motif" : motif)
                             .arg(timestamp);

        source->setDerniereOperation(opSource);
        dest->setDerniereOperation(opDest);

        // Enregistrer la transaction en base
        if (!enregistrerTransaction("virement", montant, compteSource, compteDest, motif)) {
            qWarning() << "Erreur lors de l'enregistrement de la transaction";
        }

        sauvegarderCompte(source);
        sauvegarderCompte(dest);
        mettreAJourAffichageComptes();
        QMessageBox::information(this, "Succès", "Virement effectué avec succès!");
    } else {
        QMessageBox::warning(this, "Erreur",
                             messageErreur.isEmpty() ? "Virement impossible!" : messageErreur);
    }
}








// Gestion de la suppression des comptes
void fenMain::on_btn_supprimer_compte_courant_clicked()
{
    if (QMessageBox::question(this, "Confirmation",
                              "Voulez-vous vraiment supprimer votre compte courant?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        supprimerCompteCourant();
    }
}




void fenMain::on_btn_supprimer_compte_epargne_clicked()
{
    if (QMessageBox::question(this, "Confirmation",
                              "Voulez-vous vraiment supprimer votre compte épargne?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        supprimerCompteEpargne();
    }
}

void fenMain::supprimerCompteCourant()
{
    CompteCourant* compte = getCompteCourant();
    if (!compte) {
        QMessageBox::information(this, "Information", "Aucun compte courant à supprimer!");
        return;
    }

    // CORRECTION: Vérifier les transactions en cours
    if (!m_creationBD.estOuverte()) {
        QMessageBox::critical(this, "Erreur", "Base de données non accessible!");
        return;
    }

    QSqlDatabase db = m_creationBD.getDatabase();

    // Commencer une transaction SQL pour assurer la cohérence
    if (!db.transaction()) {
        QMessageBox::critical(this, "Erreur", "Impossible de démarrer la transaction!");
        return;
    }

    try {
        QSqlQuery query(db);

        // Marquer le compte comme inactif au lieu de le supprimer
        query.prepare("UPDATE comptes SET est_actif = 0 WHERE numero_compte = ?");
        query.addBindValue(compte->getNumeroCompte());

        if (!query.exec()) {
            db.rollback();
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression: " + query.lastError().text());
            return;
        }

        // CORRECTION: Utiliser une méthode spécifique pour supprimer le compte
        m_banque.supprimerCompte(compte->getNumeroCompte());
        // OU si cette méthode n'existe pas, utilisez :
        // auto& comptes = m_banque.getComptes();
        // comptes.removeOne(compte);
        // delete compte;

        if (!db.commit()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la validation!");
            return;
        }

        mettreAJourAffichageComptes();
        QMessageBox::information(this, "Succès", "Compte courant supprimé!");

    } catch (const std::exception& e) {
        db.rollback();
        QMessageBox::critical(this, "Erreur", QString("Erreur inattendue: %1").arg(e.what()));
    }
}







void fenMain::supprimerCompteEpargne()
{
    CompteEpargne* compte = getCompteEpargne();
    if (!compte) {
        QMessageBox::information(this, "Information", "Aucun compte épargne à supprimer!");
        return;
    }

    // CORRECTION: Même logique sécurisée que pour le compte courant
    if (!m_creationBD.estOuverte()) {
        QMessageBox::critical(this, "Erreur", "Base de données non accessible!");
        return;
    }

    QSqlDatabase db = m_creationBD.getDatabase();

    if (!db.transaction()) {
        QMessageBox::critical(this, "Erreur", "Impossible de démarrer la transaction!");
        return;
    }

    try {
        QSqlQuery query(db);

        query.prepare("UPDATE comptes SET est_actif = 0 WHERE numero_compte = ?");
        query.addBindValue(compte->getNumeroCompte());

        if (!query.exec()) {
            db.rollback();
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression: " + query.lastError().text());
            return;
        }

        // CORRECTION: Utiliser une méthode spécifique pour supprimer le compte
        m_banque.supprimerCompte(compte->getNumeroCompte());
        // OU si cette méthode n'existe pas, utilisez :
        // auto& comptes = m_banque.getComptes();
        // comptes.removeOne(compte);
        // delete compte;

        if (!db.commit()) {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la validation!");
            return;
        }

        mettreAJourAffichageComptes();
        QMessageBox::information(this, "Succès", "Compte épargne supprimé!");

    } catch (const std::exception& e) {
        db.rollback();
        QMessageBox::critical(this, "Erreur", QString("Erreur inattendue: %1").arg(e.what()));
    }
}






void fenMain::on_btn_modifier_info_tutilaire_parametre_clicked()
{
        /// à completer obligatoirement
}

