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
    m_boutonBasculeNotificationEmail(nullptr)
{
    ui->setupUi(this);

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

    qApp->installEventFilter(this);
    ui->masquer_solde_compte_courant_principale->installEventFilter(this);
    ui->masquer_solde_compte_epargne->installEventFilter(this);
}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
    delete m_menuCompte;
    delete m_soldeAnimation;
    delete m_boutonBasculeNotificationEmail;
}

void FenetrePrincipale::configurerFenetrePrincipale()
{
    this->showMaximized();
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

    // Force la mise à jour du style
    button->style()->unpolish(button);
    button->style()->polish(button);
    button->update();
}

bool FenetrePrincipale::eventFilter(QObject *obj, QEvent *event)
{
    // Gestion du menu compte
    if (m_menuCompteVisible && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (!m_menuCompte->geometry().contains(mouseEvent->globalPosition().toPoint()) &&
            obj != ui->btn_compte) {
            cacherMenuCompte();
        }
    }

    // Gestion des boutons de masquage
    QToolButton* button = qobject_cast<QToolButton*>(obj);
    if (button && (button == ui->masquer_solde_compte_courant_principale ||
                   button == ui->masquer_solde_compte_epargne
                   ))
    {
        if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
            updateButtonIcon(button, getCurrentVisibilityState(button));
            return true;
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
    // Émettre le signal de déconnexion
    emit deconnexionDemandee();

    // Masquer cette fenêtre plutôt que de la fermer
    this->hide();

    // Afficher à nouveau la fenêtre d'authentification
    Authentification *auth = new Authentification();
    auth->setAttribute(Qt::WA_DeleteOnClose);

    // Connecter le signal de réussite d'authentification
    connect(auth, &Authentification::authentificationReussie, this, [this](const QString& userId) {
        this->m_userId = userId; // Mettre à jour l'ID utilisateur si nécessaire
        this->show(); // Réafficher la fenêtre principale
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
    // Émettre le signal de déconnexion
    emit deconnexionDemandee();

    // Masquer cette fenêtre plutôt que de la fermer
    this->hide();

    // Afficher à nouveau la fenêtre d'authentification
    Authentification *auth = new Authentification();
    auth->setAttribute(Qt::WA_DeleteOnClose);

    // Connecter le signal de réussite d'authentification
    connect(auth, &Authentification::authentificationReussie, this, [this](const QString& userId) {
        this->m_userId = userId; // Mettre à jour l'ID utilisateur si nécessaire
        this->show(); // Réafficher la fenêtre principale
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

    // Désactivé par défaut
    m_notificationsEmailActivees = false;
    m_boutonBasculeNotificationEmail->definirEtatBascule(false); // Force l'état désactivé

    connect(m_boutonBasculeNotificationEmail, &MonBoutonBascule::aBascule,
            this, &FenetrePrincipale::gererBasculeNotificationEmail);

    if (ui->zone_bouton_bascule && !ui->zone_bouton_bascule->layout()) {
        QVBoxLayout *layoutBouton = new QVBoxLayout(ui->zone_bouton_bascule);
        layoutBouton->setContentsMargins(0, 0, 0, 0);
        layoutBouton->addWidget(m_boutonBasculeNotificationEmail);
        layoutBouton->setAlignment(m_boutonBasculeNotificationEmail, Qt::AlignCenter);
        ui->zone_bouton_bascule->setLayout(layoutBouton);
    } else if (ui->zone_bouton_bascule && ui->zone_bouton_bascule->layout()) {
        // Si un layout existe déjà, on ajoute le bouton à ce layout
        qobject_cast<QVBoxLayout*>(ui->zone_bouton_bascule->layout())->addWidget(m_boutonBasculeNotificationEmail);
        qobject_cast<QVBoxLayout*>(ui->zone_bouton_bascule->layout())->setAlignment(m_boutonBasculeNotificationEmail, Qt::AlignCenter);
    } else {
        qDebug() << "Erreur: Le widget 'zone_bouton_bascule' n'a pas été trouvé dans l'UI ou n'est pas un QWidget.";
    }
}

void FenetrePrincipale::gererBasculeNotificationEmail(bool estActive)
{
    m_notificationsEmailActivees = estActive; // Met à jour l'état de ta variable
    if (m_notificationsEmailActivees) {
        qDebug() << "Notifications par email activées !";
        // Ici, tu peux ajouter la logique pour activer la réception d'emails
        // par exemple, appeler une fonction de ton gestionnaire de notifications
    } else {
        qDebug() << "Notifications par email désactivées !";
        // Ici, tu peux ajouter la logique pour désactiver la réception d'emails
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
