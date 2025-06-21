#include "authentification.h"
#include "ui_authentification.h"
#include <QRegularExpression>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QEvent>

Authentification::Authentification(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Authentification)
    , m_currentUserId("")
    , m_gestionBD("banque.db")
{
    ui->setupUi(this);

    // Initialiser la base de données
    if (!m_gestionBD.ouvrirConnexion()) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir la base de données");
    }

    // Connexions pour vider les messages d'erreur
    connect(ui->lineEdit_nom_complet_zone_creer_compte, &QLineEdit::textChanged, this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_email_zone_creer_compte, &QLineEdit::textChanged, this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_mot_de_passe_zone_creer_compte, &QLineEdit::textChanged, this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_confirme_mot_de_passe_zone_creer_compte, &QLineEdit::textChanged, this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_email_connexion, &QLineEdit::textChanged, this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_mot_de_passe_connexion, &QLineEdit::textChanged, this, &Authentification::viderMessagesErreur);

    // Transformation automatique pour le NOM COMPLET (création de compte) - MAJUSCULES
    connect(ui->lineEdit_nom_complet_zone_creer_compte, &QLineEdit::textChanged, [this](const QString &text) {
        QSignalBlocker blocker(ui->lineEdit_nom_complet_zone_creer_compte);
        ui->lineEdit_nom_complet_zone_creer_compte->setText(text.toUpper());
    });

    // Transformation automatique pour l'EMAIL (création de compte) - minuscules
    connect(ui->lineEdit_email_zone_creer_compte, &QLineEdit::textChanged, [this](const QString &text) {
        QSignalBlocker blocker(ui->lineEdit_email_zone_creer_compte);
        ui->lineEdit_email_zone_creer_compte->setText(text.toLower());
    });

    // Transformation automatique pour l'EMAIL (connexion) - minuscules
    connect(ui->lineEdit_email_connexion, &QLineEdit::textChanged, [this](const QString &text) {
        QSignalBlocker blocker(ui->lineEdit_email_connexion);
        ui->lineEdit_email_connexion->setText(text.toLower());
    });

    // Configurer les boutons d'affichage du mot de passe
    setupPasswordVisibilityToggle(ui->lineEdit_mot_de_passe_connexion);
    setupPasswordVisibilityToggle(ui->lineEdit_mot_de_passe_zone_creer_compte);
    setupPasswordVisibilityToggle(ui->lineEdit_confirme_mot_de_passe_zone_creer_compte);
}

Authentification::~Authentification()
{
    delete ui;
}

bool Authentification::validerNom(const QString& nom) {
    if (nom.isEmpty()) return false;
    QChar premierChar = nom.at(0);
    return premierChar.isLetter();
}

bool Authentification::validerEmail(const QString& email) {
    QRegularExpression regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return regex.match(email).hasMatch();
}

bool Authentification::validerMotDePasse(const QString& motDePasse) {
    return motDePasse.length() >= 8;
}

void Authentification::viderMessagesErreur() {
    ui->zone_message_erreur_page_creer_compte->clear();
    ui->zone_message_erreur_page_se_connecter->clear();
}

void Authentification::on_btn_creer_compte_zone_connexion_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void Authentification::on_btn_se_connecter_zone_creer_compte_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void Authentification::on_btn_creer_compte_zone_creer_compte_clicked() {
    QString nom = ui->lineEdit_nom_complet_zone_creer_compte->text().trimmed();
    QString email = ui->lineEdit_email_zone_creer_compte->text().trimmed();
    QString mdp = ui->lineEdit_mot_de_passe_zone_creer_compte->text();
    QString confirmMdp = ui->lineEdit_confirme_mot_de_passe_zone_creer_compte->text();

    // Validation des champs
    if (!validerNom(nom)) {
        ui->zone_message_erreur_page_creer_compte->setText("Le nom doit commencer par une lettre.");
        return;
    }

    if (!validerEmail(email)) {
        ui->zone_message_erreur_page_creer_compte->setText("Email invalide.");
        return;
    }

    if (!validerMotDePasse(mdp)) {
        ui->zone_message_erreur_page_creer_compte->setText("Le mot de passe doit avoir 8 caractères minimum.");
        return;
    }

    if (mdp != confirmMdp) {
        ui->zone_message_erreur_page_creer_compte->setText("Les mots de passe ne correspondent pas.");
        return;
    }

    // Vérifier si l'utilisateur existe déjà
    if (m_gestionBD.existeUtilisateur(email)) {
        ui->zone_message_erreur_page_creer_compte->setText("Un compte avec cet email existe déjà.");
        return;
    }

    // Créer le nouvel utilisateur
    if (m_gestionBD.creerUtilisateur(nom, email, mdp)) {
        QMessageBox::information(this, "Succès", "Compte créé avec succès. Vous pouvez maintenant vous connecter.");
        ui->stackedWidget->setCurrentIndex(0);
        ui->lineEdit_email_connexion->setText(email);
        ui->lineEdit_mot_de_passe_connexion->clear();
    } else {
        ui->zone_message_erreur_page_creer_compte->setText("Erreur lors de la création du compte.");
    }
}

void Authentification::on_btn_se_connecter_zone_connexion_clicked()
{
    QString email = ui->lineEdit_email_connexion->text().trimmed();
    QString mdp = ui->lineEdit_mot_de_passe_connexion->text();

    // Validation des champs
    if (!validerEmail(email))
    {
        ui->zone_message_erreur_page_se_connecter->setText("Email invalide.");
        return;
    }

    if (!validerMotDePasse(mdp))
    {
        ui->zone_message_erreur_page_se_connecter->setText("Mot de passe incorrect.");
        return;
    }

    // Authentification
    QString idUtilisateur;
    if (m_gestionBD.authentifierUtilisateur(email, mdp, idUtilisateur))
    {
        m_currentUserId = idUtilisateur;
        m_gestionBD.enregistrerConnexion(idUtilisateur, true);

        // Émettre le signal avant accept() pour garantir que le récepteur reçoive le userId
        emit authentificationReussie(m_currentUserId);

        // Fermer la fenêtre avec le statut Accepted
        accept();
    }
    else
    {
        m_gestionBD.enregistrerConnexion("0", false); // "0" pour utilisateur inconnu
        ui->zone_message_erreur_page_se_connecter->setText("Email ou mot de passe incorrect.");
    }
}

void Authentification::setupPasswordVisibilityToggle(QLineEdit* passwordLineEdit)
{
    // Créer le bouton de bascule
    QPushButton* toggleButton = new QPushButton(passwordLineEdit);
    toggleButton->setCursor(Qt::PointingHandCursor);
    toggleButton->setCheckable(true);
    toggleButton->setChecked(false);

    // Style minimal sans bordure ni fond
    toggleButton->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   background: none;"
        "   padding: 0px;"
        "   margin: 0px;"
        "   width: 24px;"
        "}"
        );

    // Définir l'icône (utiliser vos propres icônes ou caractères Unicode)
    QIcon visibilityIcon;
    visibilityIcon.addFile(":/icon_gris/eye.svg", QSize(), QIcon::Normal, QIcon::Off);
    visibilityIcon.addFile(":/icon_gris/eye-off.svg", QSize(), QIcon::Normal, QIcon::On);
    toggleButton->setIcon(visibilityIcon);
    toggleButton->setIconSize(QSize(16, 16));

    // Ajuster les marges pour faire de la place pour le bouton
    int frameWidth = passwordLineEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    passwordLineEdit->setStyleSheet(QString(
        "QLineEdit {"
        "   padding-right: 24px;"
        "}"
        ));

    // Position initiale du bouton
    toggleButton->move(
        passwordLineEdit->width() - 24 - frameWidth,
        (passwordLineEdit->height() - 24) / 2
        );

    // Connecter le signal de bascule
    connect(toggleButton, &QPushButton::toggled, [passwordLineEdit]() {
        passwordLineEdit->setEchoMode(passwordLineEdit->echoMode() == QLineEdit::Password
                                          ? QLineEdit::Normal
                                          : QLineEdit::Password);
    });

    // Installer le filtre d'événements pour gérer le redimensionnement
    passwordLineEdit->installEventFilter(this);
}

bool Authentification::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Resize) {
        if (auto lineEdit = qobject_cast<QLineEdit*>(obj)) {
            if (auto button = lineEdit->findChild<QPushButton*>()) {
                int frameWidth = lineEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
                button->move(
                    lineEdit->width() - 24 - frameWidth,
                    (lineEdit->height() - 24) / 2
                    );
            }
        }
    }
    return QDialog::eventFilter(obj, event);
}
