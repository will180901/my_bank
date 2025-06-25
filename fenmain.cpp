#include "fenmain.h"
#include "ui_fenmain.h"
#include <QApplication>
#include <QEvent>
#include <QDebug>

fenMain::fenMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::fenMain)
    , m_soldeVisibleCompteCourant(true)
    , m_soldeVisibleCompteEpargne(true)
    , m_soldeAnimation(new AnimationSolde(this))
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
}

fenMain::~fenMain()
{
    delete ui;
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
    // CORRECTION: Utiliser la bonne variable
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
