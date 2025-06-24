#include "fenetreprincipale.h"
#include "ui_fenetreprincipale.h"
#include "authentification.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolButton>
#include <QFrame>
#include <QMouseEvent>
#include <QPushButton>
#include <QScreen>
#include <QApplication>
#include <QIcon>
#include <QStyle>
#include <QGraphicsBlurEffect>
#include <QScrollArea>
#include <QSettings>
#include "gestionbd.h"
#include <QSequentialAnimationGroup>
#include "utilitairesmotdepasse.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent, const QString &userId)
    : QMainWindow(parent),
    ui(new Ui::FenetrePrincipale),
    m_userId(userId),
    m_boutonActif("dashboard"),
    m_menuCompte(nullptr),
    m_menuCompteVisible(false),
    m_soldeVisibleCompteCourant(true),
    m_soldeVisibleCompteEpargne(true),
    m_soldeVisibleCompteJoint(true),
    m_soldeAnimation(new AnimationSolde(this)),
    m_boutonBasculeNotificationEmail(nullptr),
    m_compteCourantExiste(false),
    m_compteEpargneExiste(false),
    m_rideauCompteCourant(nullptr),
    m_rideauCompteEpargne(nullptr)
{
    ui->setupUi(this);

    ui->setupUi(this);

    InitialisationThemeCouleur();
    configurerFenetrePrincipale();
    creerMenuCompte();
    configurerBoutonBasculeNotificationEmail();

    ui->mes_pages->setCurrentWidget(ui->page_dashboard);
    mettreAJourStyleBoutonsLateraux();
    mettreAJourStyleBoutonCompte();

    // Style robuste pour les boutons de masquage
    QString buttonStyle = "QToolButton { background: transparent; border: none; padding: 6px 12px; }"
                          "QToolButton:hover { background-color: rgb(41, 98, 255); border-radius: 4px; }";

    ui->masquer_solde_compte_courant_principale->setStyleSheet(buttonStyle);
    ui->masquer_solde_compte_epargne->setStyleSheet(buttonStyle);

    updateButtonIcon(ui->masquer_solde_compte_courant_principale, m_soldeVisibleCompteCourant);
    updateButtonIcon(ui->masquer_solde_compte_epargne, m_soldeVisibleCompteEpargne);

    ui->combo_choix_type_operation_onglet_DR->addItem("Dépot");
    ui->combo_choix_type_operation_onglet_DR->addItem("Retrait");

    // Charger les données de l'utilisateur après l'authentification
    chargerDonneesUtilisateur();

    qApp->installEventFilter(this);
    ui->masquer_solde_compte_courant_principale->installEventFilter(this);
    ui->masquer_solde_compte_epargne->installEventFilter(this);

    // Configurer les champs de mot de passe pour afficher/masquer
    UtilitairesMotDePasse::configurerBoutonVisibilite(ui->sai_mot_de_passe_modif_parametres);
    UtilitairesMotDePasse::configurerBoutonVisibilite(ui->sai_confirm_mot_de_passe_modif_parametres);

    connect(ui->combo_choix_type_operation_onglet_DR, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FenetrePrincipale::gererChangementTypeOperation);

    // Transformation automatique pour le NOM COMPLET - MAJUSCULES
    connect(ui->sai_nom_complet_modif_parametres, &QLineEdit::textChanged, [this](const QString &text) {
        QSignalBlocker blocker(ui->sai_nom_complet_modif_parametres);
        ui->sai_nom_complet_modif_parametres->setText(text.toUpper());
    });

    // Transformation automatique pour l'EMAIL - minuscules
    connect(ui->sai_email_modif_parametres, &QLineEdit::textChanged, [this](const QString &text) {
        QSignalBlocker blocker(ui->sai_email_modif_parametres);
        ui->sai_email_modif_parametres->setText(text.toLower());
    });

    // Connexions pour vider les messages d'erreur
    connect(ui->sai_nom_complet_modif_parametres, &QLineEdit::textChanged, [this]() {
        ui->zone_message_erreur_page_parametre->clear();
    });
    connect(ui->sai_email_modif_parametres, &QLineEdit::textChanged, [this]() {
        ui->zone_message_erreur_page_parametre->clear();
    });
    connect(ui->sai_mot_de_passe_modif_parametres, &QLineEdit::textChanged, [this]() {
        ui->zone_message_erreur_page_parametre->clear();
    });
    connect(ui->sai_confirm_mot_de_passe_modif_parametres, &QLineEdit::textChanged, [this]() {
        ui->zone_message_erreur_page_parametre->clear();
    });
}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
    delete m_menuCompte;
    delete m_soldeAnimation;
    delete m_boutonBasculeNotificationEmail;

    // Nettoyer les rideaux
    if (m_rideauCompteCourant) delete m_rideauCompteCourant;
    if (m_rideauCompteEpargne) delete m_rideauCompteEpargne;
}

void FenetrePrincipale::chargerDonneesUtilisateur()
{
    GestionBD gestionBD("banque.db");
    if (!gestionBD.ouvrirConnexion()) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données");
        return;
    }

    // Récupérer les informations de l'utilisateur
    QSqlQuery queryUtilisateur;
    queryUtilisateur.prepare("SELECT nom_complet, email FROM Utilisateurs WHERE id = ?");
    queryUtilisateur.addBindValue(m_userId);
    if (queryUtilisateur.exec() && queryUtilisateur.next()) {
        QString nomComplet = queryUtilisateur.value("nom_complet").toString();
        QString email = queryUtilisateur.value("email").toString();
        m_userFullName = nomComplet;

        // Mettre à jour le label de bienvenue
        ui->labelBienvenue->setText("Bienvenue, " + nomComplet.toUpper());

        // Mettre à jour les labels du profil
        ui->label_nom_profil->setText(nomComplet.toUpper());
        ui->label_email_profil->setText(email);

        // Pré-remplir les champs des paramètres
        ui->sai_nom_complet_modif_parametres->setText(nomComplet);
        ui->sai_email_modif_parametres->setText(email);
    } else {
        qDebug() << "Erreur lors de la récupération des informations de l'utilisateur";
    }

    // Récupérer les comptes de l'utilisateur
    QList<CompteBancaire*> comptes = gestionBD.getComptesUtilisateur(m_userId);

    for (CompteBancaire* compte : comptes) {
        if (compte->getType() == "COURANT") {
            m_compteCourantExiste = true;
            // Compte courant
            ui->label_numero_courant->setText(compte->getNumeroCompte());
            ui->label_solde_compte_courant_principale->setText(QString::number(compte->getSolde(), 'f', 2) + " FCFA");

            CompteCourant* cc = dynamic_cast<CompteCourant*>(compte);
            if (cc) {
                // Afficher le découvert autorisé
                ui->label_decouvert_autorise->setText(QString::number(cc->getDecouvertAutorise(), 'f', 2) + " FCFA");
            }

            // Récupérer la dernière opération pour le compte courant
            QMap<QString, QVariant> derniereTransaction = gestionBD.getDerniereTransaction(compte->getId());
            if (!derniereTransaction.isEmpty()) {
                QString type = derniereTransaction["type"].toString();
                double montant = derniereTransaction["montant"].toDouble();
                QString date = derniereTransaction["date"].toString();

                QString message = type + " - " + date + " - " + QString::number(montant, 'f', 2) + " FCFA";
                ui->label_derniere_operation_compte_courant_principale->setText(message);
            } else {
                ui->label_derniere_operation_compte_courant_principale->setText("Aucune opération récente");
            }
        }
        else if (compte->getType() == "EPARGNE") {
            m_compteEpargneExiste = true;
            // Compte épargne
            ui->label_numero_epargne->setText(compte->getNumeroCompte());
            ui->label_solde_compte_epargne->setText(QString::number(compte->getSolde(), 'f', 2) + " FCFA");

            CompteEpargne* ce = dynamic_cast<CompteEpargne*>(compte);
            if (ce) {
                // Afficher le taux d'intérêt
                ui->label_taux_interet->setText(QString::number(ce->getTauxInteret(), 'f', 2) + "%");
            }

            // Récupérer la dernière opération pour le compte épargne
            QMap<QString, QVariant> derniereTransaction = gestionBD.getDerniereTransaction(compte->getId());
            if (!derniereTransaction.isEmpty()) {
                QString type = derniereTransaction["type"].toString();
                double montant = derniereTransaction["montant"].toDouble();
                QString date = derniereTransaction["date"].toString();

                QString message = type + " - " + date + " - " + QString::number(montant, 'f', 2) + " FCFA";
                ui->label_derniere_operation_livret_epargne->setText(message);
            } else {
                ui->label_derniere_operation_livret_epargne->setText("Aucune opération récente");
            }
        }
    }

    mettreAJourApparenceComptes();
    qDeleteAll(comptes);
}

void FenetrePrincipale::configurerFenetrePrincipale()
{
    this->showMaximized();
}

bool FenetrePrincipale::eventFilter(QObject *obj, QEvent *event)
{
    if (m_menuCompteVisible && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (!m_menuCompte->geometry().contains(mouseEvent->globalPosition().toPoint()) &&
            obj != ui->btn_compte) {
            cacherMenuCompte();
        }
    }

    QToolButton* button = qobject_cast<QToolButton*>(obj);
    if (button && (button == ui->masquer_solde_compte_courant_principale ||
                   button == ui->masquer_solde_compte_epargne))
    {
        if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
            updateButtonIcon(button, getCurrentVisibilityState(button));
            return true;
        }
    }



    // Gestion du redimensionnement pour les rideaux
    if (event->type() == QEvent::Resize || event->type() == QEvent::Move) {
        if (m_rideauCompteCourant) {
            m_rideauCompteCourant->setGeometry(ui->carte_courant_principal->geometry());
        }
        if (m_rideauCompteEpargne) {
            m_rideauCompteEpargne->setGeometry(ui->Carte_Livret_A->geometry());
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void FenetrePrincipale::mettreAJourStyleBoutonsLateraux()
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

    ui->btn_lateral_dashboard->setStyleSheet(styleBase);
    ui->btn_lateral_depot_et_retrait->setStyleSheet(styleBase);
    ui->btn_lateral_historique->setStyleSheet(styleBase);
    ui->btn_lateral_parametres->setStyleSheet(styleBase);
    ui->btn_lateral_deconnexion->setStyleSheet(styleBase);

    if (ui->mes_pages->currentWidget() == ui->page_dashboard) {
        ui->btn_lateral_dashboard->setStyleSheet(styleActif);
        m_boutonActif = "dashboard";
    }
    else if (ui->mes_pages->currentWidget() == ui->page_transaction) {
        ui->btn_lateral_depot_et_retrait->setStyleSheet(styleActif);
        m_boutonActif = "virements";
    }
    else if (ui->mes_pages->currentWidget() == ui->page_historique) {
        ui->btn_lateral_historique->setStyleSheet(styleActif);
        m_boutonActif = "historique";
    }
    else if (ui->mes_pages->currentWidget() == ui->page_parametre) {
        ui->btn_lateral_parametres->setStyleSheet(styleActif);
        m_boutonActif = "parametres";
    }
}

void FenetrePrincipale::creerMenuCompte()
{
    m_menuCompte = new QWidget(this);
    m_menuCompte->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    m_menuCompte->setStyleSheet(
        "QWidget { "
        "background-color: #FFFFFF; "
        "border: 1px solid #E1E6EF; "
        "padding: 8px 0; "
        "}");
    m_menuCompte->setFixedSize(120, 150);

    QGraphicsDropShadowEffect *ombre = new QGraphicsDropShadowEffect(m_menuCompte);
    ombre->setBlurRadius(12);
    ombre->setColor(QColor(0, 0, 0, 25));
    ombre->setOffset(0, 4);
    m_menuCompte->setGraphicsEffect(ombre);

    QVBoxLayout *layout = new QVBoxLayout(m_menuCompte);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *titre = new QLabel("Mon Compte");
    titre->setStyleSheet(
        "QLabel { "
        "font-weight: 600; "
        "color: #4A4A4A; "
        "padding: 2px 12px; "
        "font-size: 13px; "
        "border-bottom: 1px solid #F0F2F5; "
        "margin-bottom: 0; "
        "}");
    titre->setFixedHeight(39);
    layout->addWidget(titre);

    creerBoutonMenu("Profil", "menu_profil", SLOT(on_menu_profil_clicked()));
    creerBoutonMenu("Paramètres", "menu_parametres", SLOT(on_menu_parametres_clicked()));
    ajouterSeparateurMenu(layout);
    creerBoutonMenu("Déconnexion", "menu_deconnexion", SLOT(on_menu_deconnexion_clicked()));

    m_menuCompte->setLayout(layout);
    m_menuCompte->hide();
}

void FenetrePrincipale::creerBoutonMenu(const QString &texte, const QString &objetNom, const char *slot)
{
    QPushButton *bouton = new QPushButton(texte);
    bouton->setObjectName(objetNom);
    bouton->setStyleSheet(
        "QPushButton { "
        "border: none; "
        "padding: 8px 12px; "
        "text-align: left; "
        "color: #4A4A4A; "
        "font-size: 13px; "
        "border-radius: 3px; "
        "margin: 1px 6px; "
        "}"
        "QPushButton:hover { "
        " background-color: rgb(41, 98, 255); "
        "color: #FFFFFF; "
        "}");
    bouton->setCursor(Qt::PointingHandCursor);
    qobject_cast<QVBoxLayout*>(m_menuCompte->layout())->addWidget(bouton);
    connect(bouton, SIGNAL(clicked()), this, slot);
}

void FenetrePrincipale::ajouterSeparateurMenu(QVBoxLayout *layout)
{
    QFrame *separateur = new QFrame();
    separateur->setFrameShape(QFrame::HLine);
    separateur->setStyleSheet(
        "QFrame { "
        "border: none; "
        "border-top: 1px solid rgb(223, 223, 223); "
        "width:1px;"
        "margin: 0 "
        "}");
    layout->addWidget(separateur);
}

void FenetrePrincipale::cacherMenuCompte()
{
    if (m_menuCompteVisible) {
        m_menuCompte->hide();
        m_menuCompteVisible = false;
        mettreAJourStyleBoutonCompte();
    }
}

void FenetrePrincipale::afficherMenuCompte()
{
    if (!m_menuCompteVisible) {
        positionnerMenuCompte();
        m_menuCompte->show();
        m_menuCompteVisible = true;
        mettreAJourStyleBoutonCompte();
    }
}

void FenetrePrincipale::positionnerMenuCompte()
{
    QPoint pos = ui->btn_compte->mapToGlobal(QPoint(-80, ui->btn_compte->height()));
    QScreen *ecran = QGuiApplication::screenAt(pos);
    QRect geometrieEcran = ecran ? ecran->availableGeometry() : QGuiApplication::primaryScreen()->availableGeometry();

    if (pos.y() + m_menuCompte->height() > geometrieEcran.bottom()) {
        pos.setY(geometrieEcran.bottom() - m_menuCompte->height());
    }
    if (pos.x() < geometrieEcran.left()) {
        pos.setX(geometrieEcran.left());
    }
    else if (pos.x() + m_menuCompte->width() > geometrieEcran.right()) {
        pos.setX(geometrieEcran.right() - m_menuCompte->width());
    }

    m_menuCompte->move(pos);
}

void FenetrePrincipale::appliquerStyleBoutonCompte(bool actif)
{
    QString style;
    if (actif) {
        style = QString(
            "QToolButton { "
            "border-radius: 12px; "
            "border: 2px solid rgb(102, 71, 255); "
            "color: rgba(102, 71, 255, 200); "
            "background-color: rgba(102, 71, 255, 0.2); "
            "font: 700 12pt \"Segoe UI\"; "
            "}");
    } else {
        style = QString(
            "QToolButton { "
            "border-radius: 12px; "
            "border: 2px solid rgb(102, 71, 255); "
            "color: rgba(102, 71, 255, 200); "
            "background-color: rgba(102, 71, 255, 0.1); "
            "font: 700 12pt \"Segoe UI\"; "
            "}"
            "QToolButton:hover { "
            "background-color: rgba(102, 71, 255, 0.2); "
            "}");
    }
    ui->btn_compte->setStyleSheet(style);
}

void FenetrePrincipale::mettreAJourStyleBoutonCompte()
{
    appliquerStyleBoutonCompte(m_menuCompteVisible);
}

void FenetrePrincipale::changerPage(const QString &nomPage)
{
    if (nomPage == "parametres") {
        ui->mes_pages->setCurrentWidget(ui->page_parametre);
    }
    else if (nomPage == "profil") {
        ui->mes_pages->setCurrentWidget(ui->page_profil);
    }
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::appliquerEffetFlou(QLabel* label, bool masquer)
{
    m_soldeAnimation->appliquerAvecLabel(label, masquer);
}

void FenetrePrincipale::appliquerEffetFlouCompte(QWidget* widgetCarte, bool appliquerFlou)
{
    if (appliquerFlou) {
        QGraphicsBlurEffect *effetFlou = new QGraphicsBlurEffect(this);
        effetFlou->setBlurRadius(20);
        widgetCarte->setGraphicsEffect(effetFlou);
        widgetCarte->setEnabled(false);

        // Créer le rideau noir s'il n'existe pas
        if (widgetCarte == ui->carte_courant_principal && !m_rideauCompteCourant) {
            m_rideauCompteCourant = new QWidget(widgetCarte->parentWidget());
            m_rideauCompteCourant->setStyleSheet("background-color: rgba(0, 0, 0, 0.7); border-radius: 8px;");
            m_rideauCompteCourant->setGeometry(widgetCarte->geometry());
            m_rideauCompteCourant->raise();

            QLabel* message = new QLabel("Aucun compte courant n'est créé pour cet utilisateur  ! ", m_rideauCompteCourant);
            message->setStyleSheet("QLabel { color: white; font-weight: bold; font-size: 8px; background: transparent; }");
            message->setAlignment(Qt::AlignCenter);
            message->setGeometry(0,
                                 (m_rideauCompteCourant->height() - 60) / 2,
                                 m_rideauCompteCourant->width(),
                                 20);

            QPushButton* bouton = new QPushButton("Créer un compte", m_rideauCompteCourant);
            bouton->setStyleSheet(
                "QPushButton {"
                "   background-color: rgb(102, 71, 255);"
                "   color: white;"
                "   border-radius: 4px;"
                "   padding: 8px 16px;"
                "   font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "   background-color: rgb(82, 51, 235);"
                "}"
                );
            bouton->setCursor(Qt::PointingHandCursor);
            bouton->setGeometry((m_rideauCompteCourant->width() - 150) / 2,
                                (m_rideauCompteCourant->height() - 30) / 2 + 20,
                                150,
                                30);
            connect(bouton, &QPushButton::clicked, this, &FenetrePrincipale::creerCompteCourant);

            m_rideauCompteCourant->show();
        }
        else if (widgetCarte == ui->Carte_Livret_A && !m_rideauCompteEpargne) {
            m_rideauCompteEpargne = new QWidget(widgetCarte->parentWidget());
            m_rideauCompteEpargne->setStyleSheet("background-color: rgba(0, 0, 0, 0.7); border-radius: 8px;");
            m_rideauCompteEpargne->setGeometry(widgetCarte->geometry());
            m_rideauCompteEpargne->raise();

            QLabel* message = new QLabel("Aucun compte epargne n'est créé pour cet utilisateur  ! ", m_rideauCompteEpargne);
            message->setStyleSheet("QLabel { color: white; font-weight: bold; font-size: 8px; background: transparent; }");
            message->setAlignment(Qt::AlignCenter);
            message->setGeometry(0,
                                 (m_rideauCompteEpargne->height() - 60) / 2,
                                 m_rideauCompteEpargne->width(),
                                 20);

            QPushButton* bouton = new QPushButton("Créer un compte", m_rideauCompteEpargne);
            bouton->setStyleSheet(
                "QPushButton {"
                "   background-color: rgb(102, 71, 255);"
                "   color: white;"
                "   border-radius: 4px;"
                "   padding: 8px 16px;"
                "   font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "   background-color: rgb(82, 51, 235);"
                "}"
                );
            bouton->setCursor(Qt::PointingHandCursor);
            bouton->setGeometry((m_rideauCompteEpargne->width() - 150) / 2,
                                (m_rideauCompteEpargne->height() - 30) / 2 + 20,
                                150,
                                30);
            connect(bouton, &QPushButton::clicked, this, &FenetrePrincipale::creerCompteEpargne);

            m_rideauCompteEpargne->show();
        }
    } else {
        widgetCarte->setGraphicsEffect(nullptr);
        widgetCarte->setEnabled(true);

        // Supprimer le rideau noir s'il existe
        if (widgetCarte == ui->carte_courant_principal && m_rideauCompteCourant) {
            delete m_rideauCompteCourant;
            m_rideauCompteCourant = nullptr;
        }
        else if (widgetCarte == ui->Carte_Livret_A && m_rideauCompteEpargne) {
            delete m_rideauCompteEpargne;
            m_rideauCompteEpargne = nullptr;
        }
    }
}

void FenetrePrincipale::mettreAJourApparenceComptes()
{
    // Compte courant
    appliquerEffetFlouCompte(ui->carte_courant_principal, !m_compteCourantExiste);

    // Compte épargne
    appliquerEffetFlouCompte(ui->Carte_Livret_A, !m_compteEpargneExiste);

    // Message d'info si aucun compte
    if (!m_compteCourantExiste && !m_compteEpargneExiste) {
        QMessageBox::information(this, "Information",
                                 "Vous n'avez aucun compte bancaire actif.\n"
                                 "Veuillez contacter votre agence pour en créer un.");
    }
}

void FenetrePrincipale::creerCompteCourant()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Création de compte",
                                  "Voulez-vous vraiment créer un compte courant?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        GestionBD gestionBD("banque.db");
        if (!gestionBD.ouvrirConnexion()) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données");
            return;
        }

        if (gestionBD.creerCompte("COURANT", m_userId, "BANK001", 0.0, 500.0)) {
            m_compteCourantExiste = true;
            mettreAJourApparenceComptes();
            chargerDonneesUtilisateur();
            QMessageBox::information(this, "Succès", "Compte courant créé avec succès!");
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la création du compte courant");
        }
    }
}

void FenetrePrincipale::creerCompteEpargne()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Création de compte",
                                  "Voulez-vous vraiment créer un compte épargne?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        GestionBD gestionBD("banque.db");
        if (!gestionBD.ouvrirConnexion()) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données");
            return;
        }

        if (gestionBD.creerCompte("EPARGNE", m_userId, "BANK001", 0.0, 2.5)) {
            m_compteEpargneExiste = true;
            mettreAJourApparenceComptes();
            chargerDonneesUtilisateur();
            QMessageBox::information(this, "Succès", "Compte épargne créé avec succès!");
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la création du compte épargne");
        }
    }
}

void FenetrePrincipale::supprimerCompteCourant()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Suppression de compte",
                                  "Voulez-vous vraiment supprimer votre compte courant?\n"
                                  "Cette action est irréversible.",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        GestionBD gestionBD("banque.db");
        if (!gestionBD.ouvrirConnexion()) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données");
            return;
        }

        QList<CompteBancaire*> comptes = gestionBD.getComptesUtilisateur(m_userId);
        QString idCompteCourant;

        for (CompteBancaire* compte : comptes) {
            if (compte->getType() == "COURANT") {
                idCompteCourant = compte->getId();
                break;
            }
        }

        if (idCompteCourant.isEmpty()) {
            QMessageBox::critical(this, "Erreur", "Aucun compte courant trouvé");
            qDeleteAll(comptes);
            return;
        }

        // Supprimer d'abord les transactions associées
        QSqlQuery query;
        query.prepare("DELETE FROM Transactions WHERE compte_id = ?");
        query.addBindValue(idCompteCourant);
        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression des transactions");
            qDeleteAll(comptes);
            return;
        }

        // Puis supprimer le compte
        if (gestionBD.supprimerCompte(idCompteCourant)) {
            m_compteCourantExiste = false;
            appliquerEffetFlouCompte(ui->carte_courant_principal, true);

            // Réinitialiser les labels
            ui->label_numero_courant->setText("");
            ui->label_solde_compte_courant_principale->setText("");
            ui->label_decouvert_autorise->setText("");
            ui->label_derniere_operation_compte_courant_principale->setText("Aucune opération récente");

            QMessageBox::information(this, "Succès", "Compte courant supprimé avec succès!");
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression du compte courant");
        }

        qDeleteAll(comptes);
    }
}

void FenetrePrincipale::supprimerCompteEpargne()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Suppression de compte",
                                  "Voulez-vous vraiment supprimer votre compte épargne?\n"
                                  "Cette action est irréversible.",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        GestionBD gestionBD("banque.db");
        if (!gestionBD.ouvrirConnexion()) {
            QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données");
            return;
        }

        QList<CompteBancaire*> comptes = gestionBD.getComptesUtilisateur(m_userId);
        QString idCompteEpargne;

        for (CompteBancaire* compte : comptes) {
            if (compte->getType() == "EPARGNE") {
                idCompteEpargne = compte->getId();
                break;
            }
        }

        if (idCompteEpargne.isEmpty()) {
            QMessageBox::critical(this, "Erreur", "Aucun compte épargne trouvé");
            qDeleteAll(comptes);
            return;
        }

        // Supprimer d'abord les transactions associées
        QSqlQuery query;
        query.prepare("DELETE FROM Transactions WHERE compte_id = ?");
        query.addBindValue(idCompteEpargne);
        if (!query.exec()) {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression des transactions");
            qDeleteAll(comptes);
            return;
        }

        // Puis supprimer le compte
        if (gestionBD.supprimerCompte(idCompteEpargne)) {
            m_compteEpargneExiste = false;
            appliquerEffetFlouCompte(ui->Carte_Livret_A, true);

            // Réinitialiser les labels
            ui->label_numero_epargne->setText("");
            ui->label_solde_compte_epargne->setText("");
            ui->label_taux_interet->setText("");
            ui->label_derniere_operation_livret_epargne->setText("Aucune opération récente");

            QMessageBox::information(this, "Succès", "Compte épargne supprimé avec succès!");
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de la suppression du compte épargne");
        }

        qDeleteAll(comptes);
    }
}



void FenetrePrincipale::on_btn_lateral_dashboard_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_dashboard);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_btn_lateral_virements_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_btn_lateral_historique_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_btn_lateral_parametres_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_parametre);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_btn_compte_clicked()
{
    if (m_menuCompteVisible) {
        cacherMenuCompte();
    } else {
        afficherMenuCompte();
    }
}

void FenetrePrincipale::on_btn_consulter_compte_courant_principal_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_btn_consulter_livret_epargne_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_btn_voir_liste_complet_virement_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_historique);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_btn_lateral_deconnexion_clicked()
{
    emit deconnexionDemandee();
    this->hide();

    Authentification *auth = new Authentification();
    auth->setAttribute(Qt::WA_DeleteOnClose);

    connect(auth, &Authentification::authentificationReussie, this, [this](const QString& userId) {
        this->m_userId = userId;
        this->show();
    });

    auth->show();
}

void FenetrePrincipale::on_btn_raccourci_profil_parametre_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_parametre);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_btn_raccourci_effectuer_le_virement_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_menu_profil_clicked()
{
    changerPage("profil");
}

void FenetrePrincipale::on_menu_parametres_clicked()
{
    changerPage("parametres");
}

void FenetrePrincipale::on_menu_deconnexion_clicked()
{
    emit deconnexionDemandee();
    this->hide();

    Authentification *auth = new Authentification();
    auth->setAttribute(Qt::WA_DeleteOnClose);

    connect(auth, &Authentification::authentificationReussie, this, [this](const QString& userId) {
        this->m_userId = userId;
        this->show();
    });

    auth->show();
}

void FenetrePrincipale::on_masquer_solde_compte_courant_principale_clicked()
{
    m_soldeVisibleCompteCourant = !m_soldeVisibleCompteCourant;
    appliquerEffetFlou(ui->label_solde_compte_courant_principale, !m_soldeVisibleCompteCourant);
    updateButtonIcon(ui->masquer_solde_compte_courant_principale, m_soldeVisibleCompteCourant);
}

void FenetrePrincipale::on_masquer_solde_compte_epargne_clicked()
{
    m_soldeVisibleCompteEpargne = !m_soldeVisibleCompteEpargne;
    appliquerEffetFlou(ui->label_solde_compte_epargne, !m_soldeVisibleCompteEpargne);
    updateButtonIcon(ui->masquer_solde_compte_epargne, m_soldeVisibleCompteEpargne);
}

void FenetrePrincipale::configurerBoutonBasculeNotificationEmail()
{
    m_boutonBasculeNotificationEmail = new MonBoutonBascule(this);

    m_notificationsEmailActivees = false;
    m_boutonBasculeNotificationEmail->definirEtatBascule(false);

    connect(m_boutonBasculeNotificationEmail, &MonBoutonBascule::aBascule,
            this, &FenetrePrincipale::gererBasculeNotificationEmail);

    if (ui->zone_bouton_bascule && !ui->zone_bouton_bascule->layout()) {
        QVBoxLayout *layoutBouton = new QVBoxLayout(ui->zone_bouton_bascule);
        layoutBouton->setContentsMargins(0, 0, 0, 0);
        layoutBouton->addWidget(m_boutonBasculeNotificationEmail);
        layoutBouton->setAlignment(m_boutonBasculeNotificationEmail, Qt::AlignCenter);
        ui->zone_bouton_bascule->setLayout(layoutBouton);
    } else if (ui->zone_bouton_bascule && ui->zone_bouton_bascule->layout()) {
        qobject_cast<QVBoxLayout*>(ui->zone_bouton_bascule->layout())->addWidget(m_boutonBasculeNotificationEmail);
        qobject_cast<QVBoxLayout*>(ui->zone_bouton_bascule->layout())->setAlignment(m_boutonBasculeNotificationEmail, Qt::AlignCenter);
    } else {
        qDebug() << "Erreur: Le widget 'zone_bouton_bascule' n'a pas été trouvé dans l'UI ou n'est pas un QWidget.";
    }
}

void FenetrePrincipale::gererBasculeNotificationEmail(bool estActive)
{
    m_notificationsEmailActivees = estActive;
    if (m_notificationsEmailActivees) {
        qDebug() << "Notifications par email activées !";
    } else {
        qDebug() << "Notifications par email désactivées !";
    }
}

void FenetrePrincipale::on_btn_effectuer_transaction_compte_courant_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::on_btn_effectuer_transaction_Compte_epargne_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}




void FenetrePrincipale::on_btn_lateral_depot_et_retrait_clicked()
{
    ui->mes_pages->setCurrentWidget(ui->page_transaction);

    if (ui->combo_choix_type_operation_onglet_DR->count() == 0) {
        ui->combo_choix_type_operation_onglet_DR->addItem("Dépot");
        ui->combo_choix_type_operation_onglet_DR->addItem("Retrait");
    }
    mettreAJourStyleBoutonsLateraux();
    cacherMenuCompte();
}

void FenetrePrincipale::setupPasswordVisibilityToggle(QLineEdit* passwordLineEdit)
{
    QPushButton* toggleButton = new QPushButton(passwordLineEdit);
    toggleButton->setCursor(Qt::PointingHandCursor);
    toggleButton->setCheckable(true);
    toggleButton->setChecked(false);

    toggleButton->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   background: none;"
        "   padding: 0px;"
        "   margin: 0px;"
        "   width: 24px;"
        "}");

    QIcon visibilityIcon;
    visibilityIcon.addFile(":/icon_gris/eye.svg", QSize(), QIcon::Normal, QIcon::Off);
    visibilityIcon.addFile(":/icon_gris/eye-off.svg", QSize(), QIcon::Normal, QIcon::On);
    toggleButton->setIcon(visibilityIcon);
    toggleButton->setIconSize(QSize(16, 16));

    // Positionnement initial
    repositionnerBoutonVisibilite(passwordLineEdit, toggleButton);

    connect(toggleButton, &QPushButton::toggled, [passwordLineEdit, toggleButton]() {
        passwordLineEdit->setEchoMode(toggleButton->isChecked()
                                      ? QLineEdit::Normal
                                      : QLineEdit::Password);
    });

    // Gestion du redimensionnement
    passwordLineEdit->installEventFilter(this);
}

void FenetrePrincipale::repositionnerBoutonVisibilite(QLineEdit* passwordLineEdit, QPushButton* toggleButton)
{
    int frameWidth = passwordLineEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    int rightMargin = frameWidth + 4; // Marge supplémentaire

    // Calculer la position - extrême droite
    int xPos = passwordLineEdit->width() - toggleButton->width() - rightMargin;
    int yPos = (passwordLineEdit->height() - toggleButton->height()) / 2;

    toggleButton->move(xPos, yPos);

    // Ajuster le padding pour éviter le chevauchement du texte
    passwordLineEdit->setStyleSheet(
        QString("QLineEdit { padding-right: %1px; }")
            .arg(toggleButton->width() + rightMargin + 2)
        );
}

void FenetrePrincipale::on_btn_sauvegarde_modification_parametre_clicked()
{
    QString nouveauNom = ui->sai_nom_complet_modif_parametres->text().trimmed();
    QString nouvelEmail = ui->sai_email_modif_parametres->text().trimmed();
    QString nouveauMotDePasse = ui->sai_mot_de_passe_modif_parametres->text();
    QString confirmationMotDePasse = ui->sai_confirm_mot_de_passe_modif_parametres->text();

    if (!Authentification::validerNom(nouveauNom)) {
        ui->zone_message_erreur_page_parametre->setText("Le nom doit commencer par une lettre.");
        return;
    }

    if (!Authentification::validerEmail(nouvelEmail)) {
        ui->zone_message_erreur_page_parametre->setText("Format d'email invalide.");
        return;
    }

    if (!nouveauMotDePasse.isEmpty()) {
        if (!Authentification::validerMotDePasse(nouveauMotDePasse)) {
            ui->zone_message_erreur_page_parametre->setText("Le mot de passe doit contenir au moins 8 caractères.");
            return;
        }

        if (nouveauMotDePasse != confirmationMotDePasse) {
            ui->zone_message_erreur_page_parametre->setText("Les mots de passe ne correspondent pas.");
            return;
        }
    }

    GestionBD gestionBD("banque.db");
    if (!gestionBD.ouvrirConnexion()) {
        ui->zone_message_erreur_page_parametre->setText("Impossible de se connecter à la base de données");
        return;
    }

    if (gestionBD.modifierUtilisateur(m_userId, nouveauNom, nouvelEmail, nouveauMotDePasse)) {
        QMessageBox::information(this, "Succès", "Informations mises à jour avec succès !");

        // Mettre à jour tous les labels
        ui->label_nom_profil->setText(nouveauNom.toUpper());
        ui->label_email_profil->setText(nouvelEmail);
        ui->labelBienvenue->setText("Bienvenue, " + nouveauNom.toUpper());

        ui->zone_message_erreur_page_parametre->clear();
    } else {
        ui->zone_message_erreur_page_parametre->setText("Erreur : Cet email est déjà utilisé par un autre compte");
    }
}

void FenetrePrincipale::gererChangementTypeOperation(int index)
{
    QString operation = ui->combo_choix_type_operation_onglet_DR->itemText(index);
    qDebug() << "Opération sélectionnée:" << operation;

    if (operation == "Dépot") {
        // Configurer pour un dépot
    } else if (operation == "Retrait") {
        // Configurer pour un retrait
    }
}

void FenetrePrincipale::on_btn_supprimer_compte_courant_clicked()
{
    supprimerCompteCourant();
}

void FenetrePrincipale::on_btn_supprimer_compte_epargne_clicked()
{
    supprimerCompteEpargne();
}

bool FenetrePrincipale::getCurrentVisibilityState(QToolButton* button)
{
    if (button == ui->masquer_solde_compte_courant_principale) {
        return m_soldeVisibleCompteCourant;
    } else if (button == ui->masquer_solde_compte_epargne) {
        return m_soldeVisibleCompteEpargne;
    }
    return m_soldeVisibleCompteJoint;
}

void FenetrePrincipale::updateButtonIcon(QToolButton* button, bool visible)
{
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

void FenetrePrincipale::on_btn_change_theme_clicked()
{
    // Désactiver le bouton pendant la transition
    ui->btn_change_theme->setEnabled(false);

    // Créer un overlay pour la transition
    QWidget* overlay = new QWidget(this);
    overlay->setStyleSheet("background-color: black;");
    overlay->setGeometry(this->rect());
    overlay->show();
    overlay->raise();

    // Effet d'opacité pour l'animation
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(overlay);
    opacityEffect->setOpacity(0.0);
    overlay->setGraphicsEffect(opacityEffect);

    // Animation de fondu entrant (0 -> 1)
    QPropertyAnimation* fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
    fadeIn->setDuration(500); // 500 ms
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);

    // Animation de fondu sortant (1 -> 0)
    QPropertyAnimation* fadeOut = new QPropertyAnimation(opacityEffect, "opacity");
    fadeOut->setDuration(500); // 500 ms
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);

    // Séquence d'animations
    QSequentialAnimationGroup* sequence = new QSequentialAnimationGroup(this);
    sequence->addAnimation(fadeIn);
    sequence->addAnimation(fadeOut);

    // Changer le thème au milieu de la transition
    connect(fadeIn, &QPropertyAnimation::finished, this, [this]() {
        // Code de changement de thème (extrait de la fonction originale)
        QSettings settings("BanqueModerne", "Theme");
        bool themeSombreActif = settings.value("themeSombre", false).toBool();
        themeSombreActif = !themeSombreActif;

        QString cheminTheme = themeSombreActif
                                  ? "D:/projets/projet_bancaire/BanqueModerne/themes_fenetre_principale/theme_sombre.txt"
                                  : "D:/projets/projet_bancaire/BanqueModerne/themes_fenetre_principale/theme_clair.txt";

        QFile fichierTheme(cheminTheme);
        if (fichierTheme.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&fichierTheme);
            QString styleSheet = in.readAll();
            this->setStyleSheet(styleSheet);
            fichierTheme.close();

            // Mettre à jour l'icône
            QIcon icon;
            if (themeSombreActif) {
                icon.addFile(":/icon_gris/sun.svg");
            } else {
                icon.addFile(":/icon_gris/moon.svg");
            }
            ui->btn_change_theme->setIcon(icon);
            ui->btn_change_theme->setIconSize(QSize(20, 20));

            // Sauvegarder le nouveau thème
            settings.setValue("themeSombre", themeSombreActif);
        }
    });

    // Nettoyage après l'animation
    connect(sequence, &QSequentialAnimationGroup::finished, this, [=]() {
        overlay->deleteLater();
        ui->btn_change_theme->setEnabled(true);
    });

    // Démarrer l'animation
    sequence->start();
}

void FenetrePrincipale::InitialisationThemeCouleur(){

    // Initialisation du thème
    QSettings settings("BanqueModerne", "Theme");
    bool themeSombreActif = settings.value("themeSombre", false).toBool();

    QString cheminTheme = themeSombreActif
                              ? "D:/projets/projet_bancaire/BanqueModerne/themes_fenetre_principale/theme_sombre.txt"
                              : "D:/projets/projet_bancaire/BanqueModerne/themes_fenetre_principale/theme_clair.txt";

    QFile fichierTheme(cheminTheme);
    if (fichierTheme.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&fichierTheme);
        QString styleSheet = in.readAll();
        this->setStyleSheet(styleSheet);
        fichierTheme.close();

        // Mettre à jour l'icône du bouton
        QIcon icon;
        if (themeSombreActif) {
            icon.addFile(":/icon_gris/sun.svg");
        } else {
            icon.addFile(":/icon_gris/moon.svg");
        }
        ui->btn_change_theme->setIcon(icon);
        ui->btn_change_theme->setIconSize(QSize(20, 20));
    }
}

