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

fenMain::fenMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::fenMain)
    , m_soldeVisibleCompteCourant(true)
    , m_soldeVisibleCompteEpargne(true)
    , m_soldeAnimation(new AnimationSolde(this))
    , m_banque("MyBank")
{
    ui->setupUi(this);
    qApp->installEventFilter(this);

    this->showMaximized();

    initialiserTableTransactions();

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

    // Initialisation de la base de données
    if (!m_gestionBD.creerDossierBD()) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le dossier de la base de données.");
        return;
    }

    if (!m_gestionBD.ouvrirBD()) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données.");
        return;
    }

    if (!m_gestionBD.creerTables()) {
        QMessageBox::warning(this, "Avertissement", "Erreur lors de la création des tables.");
    }

    // Charger les données depuis la base de données
    chargerDonneesDepuisBD();

    // Afficher les données dans l'interface
    afficherCompteCourant();
    afficherCompteEpargne();
}



fenMain::~fenMain()
{
    // Sauvegarder les données dans la base de données avant de fermer
    QSqlDatabase db = m_gestionBD.getDatabase();
    QSqlQuery query(db);

    // Vider les tables existantes
    query.exec("DELETE FROM compte");

    // Sauvegarde des comptes
    for(CompteBancaire* compte : m_banque.getComptes()) {
        query.prepare("INSERT INTO compte (numero_compte, nom_titulaire, solde, type_compte, "
                      "decouvert_autorise, taux_interet, date_creation, derniere_operation) "
                      "VALUES (:num, :titulaire, :solde, :type, :decouvert, :taux, :date_crea, :derniere_op)");

        query.bindValue(":num", compte->getNumeroCompte());
        query.bindValue(":titulaire", compte->getNomTitulaire());
        query.bindValue(":solde", compte->getSolde());

        if(auto cc = dynamic_cast<CompteCourant*>(compte)) {
            query.bindValue(":type", "courant");
            query.bindValue(":decouvert", cc->getDecouvertAutorise());
            query.bindValue(":taux", QVariant()); // NULL
            query.bindValue(":date_crea", cc->getDateCreation());
            query.bindValue(":derniere_op", cc->getDerniereOperation());
        }
        else if(auto ce = dynamic_cast<CompteEpargne*>(compte)) {
            query.bindValue(":type", "epargne");
            query.bindValue(":decouvert", QVariant()); // NULL
            query.bindValue(":taux", ce->getTauxInteret());
            query.bindValue(":date_crea", ce->getDateCreation());
            query.bindValue(":derniere_op", ce->getDerniereOperation());
        }

        query.exec();
    }

    delete ui;
}


void fenMain::chargerDonneesDepuisBD()
{
    QSqlDatabase db = m_gestionBD.getDatabase();
    QSqlQuery query(db);

    // Chargement des comptes courants
    query.exec("SELECT * FROM compte WHERE type_compte = 'courant'");
    while(query.next()) {
        QString numero = query.value("numero_compte").toString();
        QString titulaire = query.value("nom_titulaire").toString();
        double solde = query.value("solde").toDouble();
        double decouvert = query.value("decouvert_autorise").toDouble();
        QString dateCreation = query.value("date_creation").toString();
        QString derniereOp = query.value("derniere_operation").toString();

        m_compteCourant = new CompteCourant(numero, titulaire, solde, decouvert);
        m_compteCourant->setDateCreation(dateCreation);
        m_compteCourant->setDerniereOperation(derniereOp);
        m_banque.ajouterCompte(m_compteCourant);
    }

    // Chargement des comptes épargne
    query.exec("SELECT * FROM compte WHERE type_compte = 'epargne'");
    while(query.next()) {
        QString numero = query.value("numero_compte").toString();
        QString titulaire = query.value("nom_titulaire").toString();
        double solde = query.value("solde").toDouble();
        double taux = query.value("taux_interet").toDouble();
        QString dateCreation = query.value("date_creation").toString();
        QString derniereOp = query.value("derniere_operation").toString();

        m_compteEpargne = new CompteEpargne(numero, titulaire, solde, taux);
        m_compteEpargne->setDateCreation(dateCreation);
        m_compteEpargne->setDerniereOperation(derniereOp);
        m_banque.ajouterCompte(m_compteEpargne);
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
    qDebug() << "Solde compte courant visible:" << m_soldeVisibleCompteCourant;
}

void fenMain::on_btn_masquer_solde_compte_epargne_clicked()
{
    m_soldeVisibleCompteEpargne = !m_soldeVisibleCompteEpargne;
    appliquerEffetFlou(ui->label_solde_compte_epargne, !m_soldeVisibleCompteEpargne);
    mettreAjourIcon(ui->btn_masquer_solde_compte_epargne, m_soldeVisibleCompteEpargne);
    qDebug() << "Solde compte épargne visible:" << m_soldeVisibleCompteEpargne;
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
    // il  va positionner le choix dans le combo de filter comme element de recheche

    // focntion à  completer obligatoirement

    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();


}


void fenMain::on_btn_consulter_compte_courant_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();

    // il  va positionner le choix dans le combo de filter comme element de recheche

    // focntion à  completer obligatoirement
}




void fenMain::on_btn_effectuer_transaction_compte_courant_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);

    ui->sai_numero_compte_onglet_depot_retrait->setText(m_compteCourant->getNumeroCompte());
     mettreAJourStyleBoutonsLateraux();
}

void fenMain::on_btn_effectuer_transaction_compte_epargne_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);

    ui->sai_numero_compte_onglet_depot_retrait->setText(m_compteEpargne->getNumeroCompte());
     mettreAJourStyleBoutonsLateraux();
}



void fenMain::on_btn_voir_liste_complete_transaction_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
}


void fenMain::on_btn_valider_transaction_clicked()
{
    if (ui->mes_onglets_page_transaction->currentIndex() == 0) {
        // Dépôt/retrait
        QString numero = ui->sai_numero_compte_onglet_depot_retrait->text();
        double montant = ui->doubleSpinBox_montant_onglet_depot_retrait->value();

        CompteBancaire* compte = m_banque.trouverCompte(numero);
        if (compte) {
            // Dépôt
            compte->deposer(montant);

            // Mettre à jour la dernière opération
            compte->setDerniereOperation(QDateTime::currentDateTime().toString(Qt::ISODate));

            // Rafraîchir l'affichage
            afficherCompteCourant();
            afficherCompteEpargne();
        }
    }
    else {
        // Virement
        QString source = ui->sai_numero_compte_source_onglet_virement->text();
        QString dest = ui->sai_numero_compte_beneficiaire_onglet_virement->text();
        double montant = ui->doubleSpinBox_montant_onglet_virement->value();

        if (m_banque.effectuerVirement(source, dest, montant)) {
            // Mettre à jour les dernières opérations
            CompteBancaire* compteSource = m_banque.trouverCompte(source);
            CompteBancaire* compteDest = m_banque.trouverCompte(dest);

            if (compteSource) {
                compteSource->setDerniereOperation(QDateTime::currentDateTime().toString(Qt::ISODate));
            }
            if (compteDest) {
                compteDest->setDerniereOperation(QDateTime::currentDateTime().toString(Qt::ISODate));
            }

            // Rafraîchir l'affichage
            afficherCompteCourant();
            afficherCompteEpargne();
        }
        else {
            ui->label_erreur_dans_chmps->setText("Échec du virement !");
        }
    }
}


void fenMain::on_btn_supprimer_transaction_clicked()
{
    // Vérifier si une transaction est sélectionnée dans la table
    QModelIndexList selectedIndexes = ui->tableView_transactions->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "Aucune sélection", "Veuillez sélectionner une transaction à supprimer.");
        return;
    }

    // Récupérer l'ID de la transaction à partir de la première colonne (supposée être l'ID)
    int row = selectedIndexes.first().row();
    int idTransaction = ui->tableView_transactions->model()->data(
                                                               ui->tableView_transactions->model()->index(row, 0)).toInt();

    // Demander confirmation
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmer suppression",
                                  "Êtes-vous sûr de vouloir supprimer cette transaction?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Supprimer la transaction de la base de données
        QSqlQuery query(m_gestionBD.getDatabase());
       query.prepare("DELETE FROM transactions WHERE id = :id");
        query.bindValue(":id", idTransaction);

        if (query.exec()) {
            // Recharger le modèle des transactions
            m_modelTransactions->select();

            // Mettre à jour les soldes des comptes concernés
            mettreAJourSoldesApresSuppression(idTransaction);

            QMessageBox::information(this, "Succès", "Transaction supprimée avec succès.");
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression: " + query.lastError().text());
        }
    }
}

void fenMain::on_btn_modifier_la_transaction_clicked()
{
    // Vérifier si une transaction est sélectionnée
    QModelIndexList selectedIndexes = ui->tableView_transactions->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "Aucune sélection", "Veuillez sélectionner une transaction à modifier.");
        return;
    }

    // Récupérer les données de la transaction sélectionnée
    int row = selectedIndexes.first().row();
    int idTransaction = ui->tableView_transactions->model()->data(
                                                               ui->tableView_transactions->model()->index(row, 0)).toInt();

    QString typeOperation = ui->tableView_transactions->model()->data(
                                                                   ui->tableView_transactions->model()->index(row, 2)).toString();

    double montant = ui->tableView_transactions->model()->data(
                                                            ui->tableView_transactions->model()->index(row, 3)).toDouble();

    QString compteSource = ui->tableView_transactions->model()->data(
                                                                  ui->tableView_transactions->model()->index(row, 4)).toString();

    QString compteBenef = ui->tableView_transactions->model()->data(
                                                                 ui->tableView_transactions->model()->index(row, 5)).toString();

    QString motif = ui->tableView_transactions->model()->data(
                                                           ui->tableView_transactions->model()->index(row, 6)).toString();

    // Ouvrir la boîte de dialogue de modification
    QDialog dialog(this);
    dialog.setWindowTitle("Modifier Transaction");

    QFormLayout form(&dialog);

    // Type d'opération
    QComboBox *comboType = new QComboBox(&dialog);
    comboType->addItems({"dépôt", "retrait", "virement"});
    comboType->setCurrentText(typeOperation);
    form.addRow("Type d'opération:", comboType);

    // Montant
    QDoubleSpinBox *spinMontant = new QDoubleSpinBox(&dialog);
    spinMontant->setRange(0, 10000000);
    spinMontant->setValue(montant);
    spinMontant->setPrefix("FCFA ");
    form.addRow("Montant:", spinMontant);

    // Compte source (pour virement/retrait)
    QLineEdit *editSource = new QLineEdit(compteSource, &dialog);
    form.addRow("Compte source:", editSource);

    // Compte bénéficiaire (pour virement)
    QLineEdit *editBenef = new QLineEdit(compteBenef, &dialog);
    form.addRow("Compte bénéficiaire:", editBenef);

    // Motif
    QTextEdit *textMotif = new QTextEdit(&dialog);
    textMotif->setPlainText(motif);
    form.addRow("Motif:", textMotif);

    // Boutons
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        // Mettre à jour la transaction dans la base de données
        QSqlQuery query(m_gestionBD.getDatabase());
        query.prepare("UPDATE transactions SET "
                      "type_operation = :type, "
                      "montant = :montant, "
                      "compte_source = :source, "
                      "compte_beneficiaire = :benef, "
                      "motif = :motif "
                      "WHERE id = :id");

        query.bindValue(":type", comboType->currentText());
        query.bindValue(":montant", spinMontant->value());
        query.bindValue(":source", editSource->text());
        query.bindValue(":benef", editBenef->text());
        query.bindValue(":motif", textMotif->toPlainText());
        query.bindValue(":id", idTransaction);

        if (query.exec()) {
            // Recharger le modèle
            m_modelTransactions->select();

            // Mettre à jour les soldes
            mettreAJourSoldesApresModification(idTransaction, montant, spinMontant->value());

            QMessageBox::information(this, "Succès", "Transaction modifiée avec succès.");
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la modification: " + query.lastError().text());
        }
    }
}



void fenMain::afficherCompteCourant()
{
    if (!m_compteCourant) return;

    // Mettre à jour les labels avec les données du compte
    ui->label_solde_compte_courant->setText(
        QString::number(m_compteCourant->getSolde(), 'f', 2) + " FCFA"
        );

    ui->label_decouvert_autorise_compte_courant->setText(
        QString::number(m_compteCourant->getDecouvertAutorise(), 'f', 2) + " FCFA"
        );

    ui->label_numero_de_compte_courant->setText(
        m_compteCourant->getNumeroCompte()
        );

    // Affichage des dates
    ui->label_date_creation_compte_courant->setText(
        QDateTime::fromString(m_compteCourant->getDateCreation(), Qt::ISODate)
            .toString("dd/MM/yyyy")
        );

    ui->label_derniere_transaction_compte_courant->setText(
        QDateTime::fromString(m_compteCourant->getDerniereOperation(), Qt::ISODate)
            .toString("dd/MM/yyyy - hh:mm")
        );
}



void fenMain::afficherCompteEpargne()
{
    if (!m_compteEpargne) return;

    // Mettre à jour les labels avec les données du compte
    ui->label_solde_compte_epargne->setText(
        QString::number(m_compteEpargne->getSolde(), 'f', 2) + " FCFA"
        );

    ui->label_taux_interet_compte_epargne->setText(
        QString::number(m_compteEpargne->getTauxInteret(), 'f', 2) + "%"
        );

    ui->label_numero_de_compte_epargne->setText(
        m_compteEpargne->getNumeroCompte()
        );

    // Affichage des dates
    ui->label_date_creation_compte_epargne->setText(
        QDateTime::fromString(m_compteEpargne->getDateCreation(), Qt::ISODate)
            .toString("dd/MM/yyyy")
        );

    ui->label_derniere_transaction_compte_epargne->setText(
        QDateTime::fromString(m_compteEpargne->getDerniereOperation(), Qt::ISODate)
            .toString("dd/MM/yyyy - hh:mm")
        );
}





void fenMain::initialiserTableTransactions()
{
    // Initialiser le modèle de table pour les transactions
    m_modelTransactions = new QSqlTableModel(this, m_gestionBD.getDatabase());
    m_modelTransactions->setTable("transactions");
    m_modelTransactions->select();

    // Configurer les en-têtes de colonnes
    m_modelTransactions->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_modelTransactions->setHeaderData(1, Qt::Horizontal, tr("Date"));
    m_modelTransactions->setHeaderData(2, Qt::Horizontal, tr("Type"));
    m_modelTransactions->setHeaderData(3, Qt::Horizontal, tr("Montant"));
    m_modelTransactions->setHeaderData(4, Qt::Horizontal, tr("Source"));
    m_modelTransactions->setHeaderData(5, Qt::Horizontal, tr("Bénéficiaire"));
    m_modelTransactions->setHeaderData(6, Qt::Horizontal, tr("Motif"));

    // Appliquer le modèle à la table
    ui->tableView_transactions->setModel(m_modelTransactions);
    ui->tableView_transactions->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_transactions->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_transactions->resizeColumnsToContents();
    ui->tableView_transactions->hideColumn(0); // Cacher l'ID
}

void fenMain::mettreAJourSoldesApresSuppression(int idTransaction)
{
    QSqlQuery query(m_gestionBD.getDatabase());
    query.prepare("SELECT type_operation, montant, compte_source, compte_beneficiaire "
                  "FROM transactions WHERE id = :id");
    query.bindValue(":id", idTransaction);

    if (query.exec() && query.next()) {
        QString type = query.value(0).toString();
        double montant = query.value(1).toDouble();
        QString source = query.value(2).toString();
        QString benef = query.value(3).toString();

        // Annuler l'effet de la transaction
        if (type == "dépôt") {
            CompteBancaire* compte = m_banque.trouverCompte(source);
            if (compte) compte->retirer(montant);
        }
        else if (type == "retrait") {
            CompteBancaire* compte = m_banque.trouverCompte(source);
            if (compte) compte->deposer(montant);
        }
        else if (type == "virement") {
            CompteBancaire* compteSource = m_banque.trouverCompte(source);
            CompteBancaire* compteBenef = m_banque.trouverCompte(benef);

            if (compteSource && compteBenef) {
                compteSource->deposer(montant); // Annuler le retrait
                compteBenef->retirer(montant);  // Annuler le dépôt
            }
        }

        // Mettre à jour l'affichage
        afficherCompteCourant();
        afficherCompteEpargne();
    }
}

void fenMain::mettreAJourSoldesApresModification(int idTransaction, double ancienMontant, double nouveauMontant)
{
    // 1. Annuler l'ancienne transaction
    mettreAJourSoldesApresSuppression(idTransaction);

    // 2. Appliquer la nouvelle transaction
    QSqlQuery query(m_gestionBD.getDatabase());
    query.prepare("SELECT type_operation, compte_source, compte_beneficiaire "
                  "FROM transactions WHERE id = :id");
    query.bindValue(":id", idTransaction);

    if (query.exec() && query.next()) {
        QString type = query.value(0).toString();
        QString source = query.value(1).toString();
        QString benef = query.value(2).toString();
        double difference = nouveauMontant - ancienMontant;

        if (type == "dépôt") {
            CompteBancaire* compte = m_banque.trouverCompte(source);
            if (compte) compte->deposer(nouveauMontant);
        }
        else if (type == "retrait") {
            CompteBancaire* compte = m_banque.trouverCompte(source);
            if (compte) compte->retirer(nouveauMontant);
        }
        else if (type == "virement") {
            CompteBancaire* compteSource = m_banque.trouverCompte(source);
            CompteBancaire* compteBenef = m_banque.trouverCompte(benef);

            if (compteSource && compteBenef) {
                compteSource->retirer(nouveauMontant);
                compteBenef->deposer(nouveauMontant);
            }
        }

        // Mettre à jour l'affichage
        afficherCompteCourant();
        afficherCompteEpargne();
    }
}
