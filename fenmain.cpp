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
#include <QGridLayout>
#include <QUuid>

fenMain::fenMain(CreationBD& m_BD, QWidget *parent, const QString &utilisateur_id)
    : QMainWindow(parent)
    , ui(new Ui::fenMain)
    , m_utilisateur_id(utilisateur_id)
    , m_soldeVisibleCompteCourant(true)
    , m_soldeVisibleCompteEpargne(true)
    , m_soldeAnimation(new AnimationSolde(this))
    , m_banque("MyBank")
    , m_creationBD(m_BD)
    , m_rideauCompteCourant(nullptr)
    , m_rideauCompteEpargne(nullptr)
{
    ui->setupUi(this);
    qApp->installEventFilter(this);

    this->showMaximized();

    ui->mes_pages->setCurrentWidget(ui->page_dashboard);
    ui->btn_masquer_solde_compte_courant->installEventFilter(this);
    ui->btn_masquer_solde_compte_epargne->installEventFilter(this);

    mettreAJourStyleBoutonsLateraux();

    appliquerStyleBoutonMasquage(ui->btn_masquer_solde_compte_courant, false);
    appliquerStyleBoutonMasquage(ui->btn_masquer_solde_compte_epargne, false);

    mettreAjourIcon(ui->btn_masquer_solde_compte_courant, m_soldeVisibleCompteCourant);
    mettreAjourIcon(ui->btn_masquer_solde_compte_epargne, m_soldeVisibleCompteEpargne);

    chargerDonneesDepuisBD();
    mettreAJourApparenceComptes();
}

fenMain::~fenMain()
{
    sauvegarderDonnees();
    delete ui;
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

void fenMain::appliquerEffetFlouCompte(QWidget* widgetCarte, bool appliquerFlou)
{
    if (appliquerFlou) {
        if (widgetCarte == ui->carte_courant_principal && !m_rideauCompteCourant) {
            // Utiliser le conteneur existant zone_rideau_compte_courant
            QWidget* conteneurRideau = ui->zone_rideau_compte_courant;
            creerRideau(conteneurRideau, m_rideauCompteCourant,
                        "Aucun compte courant n'est créé pour cet utilisateur !",
                        [this]() { this->creerCompteCourant(); });
        }
        else if (widgetCarte == ui->carte_epargne && !m_rideauCompteEpargne) {
            // Utiliser le conteneur existant zone_rideau_compte_epargne
            QWidget* conteneurRideau = ui->zone_rideau_compte_epargne;
            creerRideau(conteneurRideau, m_rideauCompteEpargne,
                        "Aucun compte épargne n'est créé pour cet utilisateur !",
                        [this]() { this->creerCompteEpargne(); });
        }
    } else {
        if (widgetCarte == ui->carte_courant_principal && m_rideauCompteCourant) {
            delete m_rideauCompteCourant;
            m_rideauCompteCourant = nullptr;
        }
        else if (widgetCarte == ui->carte_epargne && m_rideauCompteEpargne) {
            delete m_rideauCompteEpargne;
            m_rideauCompteEpargne = nullptr;
        }
    }
}

void fenMain::creerRideau(QWidget* conteneurParent, QWidget*& rideau, const QString& message, std::function<void()> callback)
{
    // Créer le rideau comme overlay par-dessus le contenu existant
    rideau = new QWidget(conteneurParent);
    rideau->setStyleSheet(
        "QWidget {"
        "   background-color: rgba(0, 0, 0, 0.8);"
        "   border-radius: 8px;"
        "}"
        );

    // Permettre aux événements de souris de passer aux widgets enfants
    rideau->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    rideau->setFocusPolicy(Qt::NoFocus);

    // Positionner le rideau pour qu'il couvre exactement tout le conteneur parent
    rideau->move(0, 0);
    rideau->resize(conteneurParent->size());

    // S'assurer que le rideau se redimensionne avec le conteneur parent
    rideau->setGeometry(0, 0, conteneurParent->width(), conteneurParent->height());

    // Layout interne du rideau pour centrer le contenu
    QVBoxLayout* layoutRideau = new QVBoxLayout(rideau);
    layoutRideau->setContentsMargins(20, 20, 20, 20);
    layoutRideau->setSpacing(15);

    // Spacer du haut pour centrer verticalement
    layoutRideau->addStretch(1);

    // Label du message avec une meilleure visibilité
    QLabel* labelMessage = new QLabel(message, rideau);
    labelMessage->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "   background: transparent;"
        "   padding: 15px;"
        "   margin: 0px;"
        "   border: none;"
        "   text-shadow: 2px 2px 4px rgba(0, 0, 0, 1.0);"
        "}"
        );
    labelMessage->setAlignment(Qt::AlignCenter);
    labelMessage->setWordWrap(true);
    layoutRideau->addWidget(labelMessage);

    // Bouton de création avec gestion optimisée des événements
    QPushButton* boutonCreer = new QPushButton("Créer un compte", rideau);
    boutonCreer->setStyleSheet(
        "QPushButton {"
        "   background-color: rgb(102, 71, 255);"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 15px 30px;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   min-height: 20px;"
        "   margin: 0px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgb(82, 51, 235);"
        "   cursor: pointer;"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgb(62, 31, 215);"
        "}"
        );

    // Configuration du curseur et des événements
    boutonCreer->setCursor(Qt::PointingHandCursor);
    boutonCreer->setAttribute(Qt::WA_Hover, true);
    boutonCreer->setFocusPolicy(Qt::StrongFocus);
    boutonCreer->setEnabled(true);
    boutonCreer->setVisible(true);

    // S'assurer que le bouton est interactif
    boutonCreer->setMouseTracking(true);
    boutonCreer->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    boutonCreer->raise();

    // Test de connexion avec debug
    connect(boutonCreer, &QPushButton::clicked, this, [callback, this, boutonCreer]() {
        qDebug() << "Bouton cliqué!"; // Pour debug
        callback();
        mettreAJourApparenceComptes();
    });

    // Centrer le bouton horizontalement
    QHBoxLayout* layoutBouton = new QHBoxLayout();
    layoutBouton->addStretch(1);
    layoutBouton->addWidget(boutonCreer);
    layoutBouton->addStretch(1);
    layoutRideau->addLayout(layoutBouton);

    // Spacer du bas pour centrer verticalement
    layoutRideau->addStretch(1);

    // Mettre le rideau au premier plan comme overlay
    rideau->raise();
    rideau->show();

    // S'assurer que tous les widgets enfants sont au-dessus
    labelMessage->raise();
    boutonCreer->raise();

    // Forcer la mise à jour de l'affichage
    rideau->repaint();
    boutonCreer->repaint();

    // Installer un filtre d'événements pour détecter le redimensionnement
    conteneurParent->installEventFilter(this);
}


void fenMain::mettreAJourApparenceComptes()
{
    bool compteCourantExiste = (getCompteCourant() != nullptr);
    bool compteEpargneExiste = (getCompteEpargne() != nullptr);

    appliquerEffetFlouCompte(ui->carte_courant_principal, !compteCourantExiste);
    appliquerEffetFlouCompte(ui->carte_epargne, !compteEpargneExiste);

    ui->carte_courant_principal->setEnabled(compteCourantExiste);
    ui->carte_epargne->setEnabled(compteEpargneExiste);
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
    mettreAJourApparenceComptes();

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
    mettreAJourApparenceComptes();

    QMessageBox::information(this, "Compte créé", "Votre compte épargne a été créé avec succès !");
}

QString fenMain::genererNumeroCompte(const QString& typeCompte)
{
    QString prefixe = (typeCompte == "courant") ? "CC" : "CE";
    QString uuid = QUuid::createUuid().toString().remove('{').remove('}').remove('-').left(10);
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
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, 1)"); // id_banque = 1 pour MyBank

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
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, 1)"); // id_banque = 1 pour MyBank

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

    mettreAJourApparenceComptes();
}

bool fenMain::eventFilter(QObject* obj, QEvent* event)
{
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


    if (event->type() == QEvent::Resize) {
        if (obj == ui->zone_rideau_compte_courant && m_rideauCompteCourant) {
            QWidget* conteneur = static_cast<QWidget*>(obj);
            m_rideauCompteCourant->resize(conteneur->size());
            m_rideauCompteCourant->setGeometry(0, 0, conteneur->width(), conteneur->height());
        }
        else if (obj == ui->zone_rideau_compte_epargne && m_rideauCompteEpargne) {
            QWidget* conteneur = static_cast<QWidget*>(obj);
            m_rideauCompteEpargne->resize(conteneur->size());
            m_rideauCompteEpargne->setGeometry(0, 0, conteneur->width(), conteneur->height());
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

// SLOTS - Gestion des événements UI
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

void fenMain::on_btn_creer_compte_courant_clicked()
{
    creerCompteCourant();
}

void fenMain::on_btn_creer_compte_epargne_clicked()
{
    creerCompteEpargne();
}
