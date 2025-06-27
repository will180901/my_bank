#include "authentification.h"
#include "ui_authentification.h"
#include <QRegularExpression>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>
#include <QEvent>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QSignalBlocker>

const QString Authentification::ADMIN_EMAIL = "admin@mybank.com";

Authentification::Authentification(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Authentification),
    m_utilisateurID(""),
    m_creationbd("banque.db")
{
    ui->setupUi(this);

    InitialisationConnexion();

    initialisationBD();

    UtilitairesMotDePasse::configurerBoutonVisibilite(ui->lineEdit_confirme_mot_de_passe_zone_creer_compte);
    UtilitairesMotDePasse::configurerBoutonVisibilite(ui->lineEdit_mot_de_passe_zone_creer_compte);
    UtilitairesMotDePasse::configurerBoutonVisibilite(ui->lineEdit_mot_de_passe_connexion);

}

Authentification::~Authentification()
{
    delete ui;
}



void Authentification::InitialisationConnexion()
{
    // Connexions pour vider les messages d'erreur
    connect(ui->lineEdit_nom_complet_zone_creer_compte, &QLineEdit::textChanged,
            this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_email_zone_creer_compte, &QLineEdit::textChanged,
            this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_mot_de_passe_zone_creer_compte, &QLineEdit::textChanged,
            this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_confirme_mot_de_passe_zone_creer_compte, &QLineEdit::textChanged,
            this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_email_connexion, &QLineEdit::textChanged,
            this, &Authentification::viderMessagesErreur);
    connect(ui->lineEdit_mot_de_passe_connexion, &QLineEdit::textChanged,
            this, &Authentification::viderMessagesErreur);

    // Transformation automatique pour le NOM COMPLET (création de compte) - MAJUSCULES
    connect(ui->lineEdit_nom_complet_zone_creer_compte, &QLineEdit::textChanged,
            [this](const QString &text) {
                QSignalBlocker blocker(ui->lineEdit_nom_complet_zone_creer_compte);
                ui->lineEdit_nom_complet_zone_creer_compte->setText(text.toUpper());
            });

    // Transformation automatique pour l'EMAIL (création de compte) - minuscules
    connect(ui->lineEdit_email_zone_creer_compte, &QLineEdit::textChanged,
            [this](const QString &text) {
                QSignalBlocker blocker(ui->lineEdit_email_zone_creer_compte);
                ui->lineEdit_email_zone_creer_compte->setText(text.toLower());
            });

    // Transformation automatique pour l'EMAIL (connexion) - minuscules
    connect(ui->lineEdit_email_connexion, &QLineEdit::textChanged,
            [this](const QString &text) {
                QSignalBlocker blocker(ui->lineEdit_email_connexion);
                ui->lineEdit_email_connexion->setText(text.toLower());
            });
}


void Authentification::verifierEtCreerAdminSiNecessaire()
{
    if (!m_creationbd.estOuverte()) {
        qWarning() << "Base de données non ouverte";
        return;
    }

    QSqlQuery query(m_creationbd.getDatabase());
    query.prepare("SELECT COUNT(*) FROM utilisateurs");

    if (!query.exec() || !query.next()) {
        qCritical() << "Erreur vérification utilisateurs:" << query.lastError().text();
        return;
    }

    int count = query.value(0).toInt();
    if (count == 0) {
        QString nom = "Administrateur";
        QString email = "admin@mybank.com";
        QString mdp = ""; // Mot de passe fort par défaut

        // Créer la banque en premier
        if (!m_creationbd.insererBanqueParDefaut()) {
            qCritical() << "Échec création banque par défaut";
            return;
        }

        // Créer l'utilisateur admin
        if (creerUtilisateur(nom, email, mdp)) {
            ui->lineEdit_email_connexion->setText(email);
            qDebug() << "Administrateur par défaut créé avec succès";

            QMessageBox::information(
                this,
                "Administrateur créé",
                "Un compte administrateur par défaut a été créé :\n"
                "Email: admin@mybank.com\n"
                "Une banque par défaut a également été créée."
                );
        } else {
            qCritical() << "Échec création administrateur par défaut";
        }
    } else {
        remplirEmailPremierUtilisateur();
    }
}




void Authentification::remplirEmailPremierUtilisateur()
{
    if (!m_creationbd.estOuverte()) {
        qWarning() << "Base de données non ouverte";
        return;
    }

    QSqlQuery query(m_creationbd.getDatabase());

    if (!query.prepare("SELECT email FROM utilisateurs ORDER BY id ASC LIMIT 1")) {
        qWarning() << "Erreur préparation requête:" << query.lastError().text();
        return;
    }

    if (!query.exec()) {
        qWarning() << "Erreur exécution requête:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        QString email = query.value(0).toString();
        ui->lineEdit_email_connexion->setText(email);
    }
}

void Authentification::initialisationBD()
{
    if (m_creationbd.estOuverte()) {
        qWarning() << "La base est déjà ouverte";
        return;
    }

    if (!m_creationbd.creerDossierBD()) {
        qCritical() << "Impossible de créer le dossier de la base de données";
        return;
    }

    m_creationbd.ouvrirBD();

    if (!m_creationbd.estOuverte()) {
        qCritical() << "Impossible d'ouvrir la base de données";
        return;
    }

    if (!m_creationbd.creerTables()) {
        qCritical() << "Impossible de créer les tables";
        return;
    }


    verifierEtCreerAdminSiNecessaire();
}

bool Authentification::validerNom(const QString& nom)
{
    if (nom.isEmpty()) return false;

    // Vérifier que le nom ne contient que des lettres et espaces
    QRegularExpression regex("^[A-Za-zÀ-ÿ\\s-]+$");
    return regex.match(nom).hasMatch() && nom.at(0).isLetter();
}

bool Authentification::validerEmail(const QString& email)
{
    if (email.isEmpty()) return false;

    QRegularExpression regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return regex.match(email).hasMatch();
}

// Modifier la fonction validerMotDePasse
bool Authentification::validerMotDePasse(const QString& motDePasse)
{
    // Autoriser mot de passe vide uniquement pour l'admin
    return motDePasse.length() >= 8 || motDePasse.isEmpty();
}

void Authentification::viderMessagesErreur()
{
    ui->zone_message_erreur_page_creer_compte->clear();
    ui->zone_message_erreur_page_se_connecter->clear();
}

void Authentification::on_btn_creer_compte_zone_connexion_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    viderMessagesErreur();
}

void Authentification::on_btn_se_connecter_zone_creer_compte_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    viderMessagesErreur();
}

bool Authentification::existeUtilisateur(const QString& email)
{
    if (!m_creationbd.estOuverte()) {
        qWarning() << "Base de données non ouverte";
        return false;
    }

    QSqlQuery query(m_creationbd.getDatabase());

    if (!query.prepare("SELECT COUNT(*) FROM utilisateurs WHERE email = :email")) {
        qWarning() << "Erreur de préparation requête existeUtilisateur:" << query.lastError().text();
        return false;
    }

    query.bindValue(":email", email);

    if (!query.exec()) {
        qWarning() << "Erreur d'exécution requête existeUtilisateur:" << query.lastError().text();
        return false;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        qDebug() << "Un utilisateur avec cet email existe déjà:" << email;
        return true;
    }

    return false;
}

QString Authentification::hasherMotDePasse(const QString& motDePasse)
{
    return QCryptographicHash::hash(
               motDePasse.toUtf8(),
               QCryptographicHash::Sha256
               ).toHex();
}

bool Authentification::creerUtilisateur(const QString& nomComplet, const QString& email, const QString& motDePasse)
{
    if (!m_creationbd.estOuverte()) {
        qWarning() << "Base de données non ouverte";
        return false;
    }

    if (existeUtilisateur(email)) {
        qWarning() << "Utilisateur déjà existant:" << email;
        return false;
    }

    QSqlQuery query(m_creationbd.getDatabase());

    if (!query.prepare("INSERT INTO utilisateurs (nom_complet, email, mot_de_passe, date_creation, est_actif) "
                       "VALUES (:nom_complet, :email, :mdp, :date, 1)")) {
        qCritical() << "Erreur de préparation création utilisateur:" << query.lastError().text();
        return false;
    }


    query.bindValue(":nom_complet", nomComplet);
    query.bindValue(":email", email);
    query.bindValue(":mdp", hasherMotDePasse(motDePasse));
    query.bindValue(":date", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        qCritical() << "Erreur création utilisateur:" << query.lastError().text();
        return false;
    }

    m_utilisateurID = query.lastInsertId().toString();
    qDebug() << "Utilisateur créé avec succès, ID:" << m_utilisateurID;

    return true;
}

void Authentification::on_btn_creer_compte_zone_creer_compte_clicked()
{
    QString nom = ui->lineEdit_nom_complet_zone_creer_compte->text().trimmed();
    QString email = ui->lineEdit_email_zone_creer_compte->text().trimmed();
    QString mdp = ui->lineEdit_mot_de_passe_zone_creer_compte->text();
    QString confirmMdp = ui->lineEdit_confirme_mot_de_passe_zone_creer_compte->text();

    // Validation des champs
    if (!validerNom(nom)) {
        ui->zone_message_erreur_page_creer_compte->setText("Le nom doit commencer par une lettre et ne contenir que des lettres, espaces et tirets.");
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

    // Créer l'utilisateur
    if (!creerUtilisateur(nom, email, mdp)) {
        ui->zone_message_erreur_page_creer_compte->setText("Un compte avec cet email existe déjà.");
        return;
    }

    QMessageBox::information(this, "Succès", "Compte créé avec succès. Vous pouvez maintenant vous connecter.");
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEdit_email_connexion->setText(email);
    ui->lineEdit_mot_de_passe_connexion->clear();

    // Vider les champs de création
    ui->lineEdit_nom_complet_zone_creer_compte->clear();
    ui->lineEdit_email_zone_creer_compte->clear();
    ui->lineEdit_mot_de_passe_zone_creer_compte->clear();
    ui->lineEdit_confirme_mot_de_passe_zone_creer_compte->clear();
}

bool Authentification::authentifierUtilisateur(const QString& email, const QString& motDePasse)
{
    if (!m_creationbd.estOuverte()) {
        qWarning() << "Base de données non ouverte";
        return false;
    }

    QSqlQuery query(m_creationbd.getDatabase());

    if (!query.prepare("SELECT id, mot_de_passe FROM utilisateurs WHERE email = :email AND est_actif = 1")) {
        qWarning() << "Erreur de préparation authentification:" << query.lastError().text();
        return false;
    }

    query.bindValue(":email", email);

    if (!query.exec()) {
        qWarning() << "Erreur d'exécution authentification:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        QString motDePasseHash = query.value("mot_de_passe").toString();
        if (motDePasseHash == hasherMotDePasse(motDePasse)) {
            m_utilisateurID = query.value("id").toString();
            return true;
        }
    }

    return false;
}

bool Authentification::enregistrerConnexion(const QString& email, bool succes)
{
    if (!m_creationbd.estOuverte()) {
        qWarning() << "Base de données non ouverte";
        return false;
    }

    QSqlQuery query(m_creationbd.getDatabase());

    if (!query.prepare("INSERT INTO historiqueConnexions(email, succes) VALUES (:email, :succes)")) {
        qWarning() << "Erreur de préparation historique connexion:" << query.lastError().text();
        return false;
    }

    query.bindValue(":email", email);
    query.bindValue(":succes", succes ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "Erreur d'enregistrement historique connexion:" << query.lastError().text();
        return false;
    }

    return true;
}

void Authentification::on_btn_se_connecter_zone_connexion_clicked()
{
    QString email = ui->lineEdit_email_connexion->text().trimmed();
    QString mdp = ui->lineEdit_mot_de_passe_connexion->text();



    // Validation des champs
    if (!validerEmail(email)) {
        ui->zone_message_erreur_page_se_connecter->setText("Email invalide.");
        return;
    }

    if (mdp.isEmpty() && email != ADMIN_EMAIL) {
        ui->zone_message_erreur_page_se_connecter->setText("Veuillez saisir votre mot de passe.");
        return;
    }

    // Tentative d'authentification
    if (authentifierUtilisateur(email, mdp)) {
        enregistrerConnexion(email, true);

        // Émettre le signal avant accept() pour garantir que le récepteur reçoive le userId
        emit authentificationReussie(m_utilisateurID,m_creationbd);

        // Fermer la fenêtre avec le statut Accepted
        accept();
    } else {
        enregistrerConnexion(email, false);
        ui->zone_message_erreur_page_se_connecter->setText("Email ou mot de passe incorrect.");
    }
}

void Authentification::reinitialiserFormulaires()
{
    // Réinitialiser la page de connexion
    ui->lineEdit_email_connexion->clear();
    ui->lineEdit_mot_de_passe_connexion->clear();
    // Réinitialiser la page de création de compte
    ui->lineEdit_nom_complet_zone_creer_compte->clear();
    ui->lineEdit_email_zone_creer_compte->clear();
    ui->lineEdit_mot_de_passe_zone_creer_compte->clear();
    ui->lineEdit_confirme_mot_de_passe_zone_creer_compte->clear();

    viderMessagesErreur();
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
