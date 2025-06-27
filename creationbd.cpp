#include "creationbd.h"
#include <QDateTime>
#include <QDebug>
#include <QSqlError>

CreationBD::CreationBD(const QString& nomFichier, QObject *parent)
    : QObject(parent),
    m_estOuverte(false),
    m_nomFichier(nomFichier)
{
}

CreationBD::~CreationBD()
{
    fermerBD();
}



bool CreationBD::insererBanqueParDefaut()
{
    if (!m_estOuverte) {
        return false;
    }

    QSqlQuery query(m_db);

    // Vérifier si une banque existe déjà
    if (!query.exec("SELECT COUNT(*) FROM banques")) {
        qWarning() << "Erreur lors de la vérification des banques:" << query.lastError().text();
        return false;
    }

    if (query.next() && query.value(0).toInt() == 0) {
        // CORRECTION: Requête préparée correctement avec 2 valeurs
        if (!query.prepare("INSERT INTO banques (nom, date_creation) "
                           "VALUES (:nom, :date)")) {
            qWarning() << "Erreur préparation insertion banque:" << query.lastError().text();
            return false;
        }

        query.bindValue(":nom", "Mybank");
        query.bindValue(":date", QDateTime::currentDateTime().toString(Qt::ISODate));

        if (!query.exec()) {
            qWarning() << "Erreur insertion banque par défaut:" << query.lastError().text();
            return false;
        }

        qDebug() << "Banque par défaut créée avec succès";
    }

    return true;
}

bool CreationBD::creerDossierBD()
{
    // Chemin par défaut pour les données d'application
    QString dbDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dbDir(dbDirPath);

    if (!dbDir.exists()) {
        if (!dbDir.mkpath(".")) {
            qCritical() << "Erreur création dossier BD:" << dbDirPath;
            return false;
        }

    }

    // Chemin complet du fichier de base de données
    m_dbPath = dbDir.filePath(m_nomFichier);

    return true;
}

void CreationBD::ouvrirBD()
{
    if (m_estOuverte) {
        qWarning() << "La base de données est déjà ouverte";
        return;
    }

    // Créer le dossier si nécessaire
    if (!creerDossierBD()) {
        qCritical() << "Impossible de créer le dossier de la base de données";
        return;
    }

    // Utiliser un nom de connexion unique pour éviter les conflits
    QString connectionName = QString("BanqueDB_%1").arg(QDateTime::currentMSecsSinceEpoch());

    m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        qCritical() << "Erreur ouverture BD:" << m_db.lastError().text();
        return;
    }

    m_estOuverte = true;
    qDebug() << "Base de données ouverte avec succès:" << m_dbPath;

    // Créer les tables si elles n'existent pas
    creerTables();
}

bool CreationBD::creerTables()
{
    if (!m_estOuverte) {
        qCritical() << "Base non ouverte!";
        return false;
    }

    QSqlQuery query(m_db);
    bool success = true;

    // Activer les contraintes de clés étrangères
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        qWarning() << "Impossible d'activer les contraintes de clés étrangères:" << query.lastError().text();
    }

    // Table Banque (corrigée - virgule manquante)
    if (!query.exec("CREATE TABLE IF NOT EXISTS banques ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "nom TEXT NOT NULL, "
                    "date_creation TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP)")) {
        qCritical() << "Erreur création table banques:" << query.lastError().text();
        success = false;
    }

    // Table Utilisateur
    if (!query.exec("CREATE TABLE IF NOT EXISTS utilisateurs ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "nom_complet TEXT NOT NULL, "
                    "email TEXT NOT NULL UNIQUE, "
                    "mot_de_passe TEXT NOT NULL, "
                    "date_creation TEXT NOT NULL, "
                    "est_actif INTEGER DEFAULT 1)")) {
        qCritical() << "Erreur création table utilisateurs:" << query.lastError().text();
        success = false;
    }

    // Table Compte avec contraintes améliorées
    if (!query.exec("CREATE TABLE IF NOT EXISTS comptes ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "numero_compte TEXT NOT NULL UNIQUE, "
                    "nom_titulaire TEXT NOT NULL, "
                    "solde REAL NOT NULL DEFAULT 0, "
                    "type_compte TEXT NOT NULL CHECK(type_compte IN ('courant', 'epargne')), "
                    "decouvert_autorise REAL DEFAULT 0, "
                    "taux_interet REAL DEFAULT 0, "
                    "date_creation TEXT NOT NULL, "
                    "derniere_operation TEXT, "
                    "id_utilisateur INTEGER NOT NULL, "
                    "id_banque INTEGER NOT NULL, "
                    "est_actif INTEGER DEFAULT 1, "
                    "FOREIGN KEY(id_utilisateur) REFERENCES utilisateurs(id) ON DELETE CASCADE, "
                    "FOREIGN KEY(id_banque) REFERENCES banques(id) ON DELETE RESTRICT)")) {
        qCritical() << "Erreur création table comptes:" << query.lastError().text();
        success = false;
    }

    // Table Transaction avec plus de détails
    if (!query.exec("CREATE TABLE IF NOT EXISTS transactions ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "type_operation TEXT NOT NULL CHECK(type_operation IN ('depot', 'retrait', 'virement')), "
                    "montant REAL NOT NULL CHECK(montant > 0), "
                    "id_compte_source INTEGER NOT NULL, "
                    "id_compte_beneficiaire INTEGER, "
                    "date_operation TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP, "
                    "libelle TEXT, "
                    "statut TEXT DEFAULT 'valide' CHECK(statut IN ('valide', 'annule', 'en_attente')), "
                    "FOREIGN KEY(id_compte_source) REFERENCES comptes(id) ON DELETE CASCADE, "
                    "FOREIGN KEY(id_compte_beneficiaire) REFERENCES comptes(id) ON DELETE CASCADE)")) {
        qCritical() << "Erreur création table transactions:" << query.lastError().text();
        success = false;
    }

    // Table Historique des connexions
    if (!query.exec("CREATE TABLE IF NOT EXISTS historiqueConnexions ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "email TEXT NOT NULL, "
                    "date TEXT DEFAULT CURRENT_TIMESTAMP, "
                    "succes INTEGER NOT NULL, "
                    "adresse_ip TEXT, "
                    "user_agent TEXT)")) {
        qCritical() << "Erreur création table historiqueConnexions:" << query.lastError().text();
        success = false;
    }


        insererBanqueParDefaut();


    if (success) {
        qDebug() << "Toutes les tables ont été créées avec succès";
    }

    return success;
}




QSqlDatabase CreationBD::getDatabase() const
{
    return m_db;
}

bool CreationBD::estOuverte() const
{
    return m_estOuverte && m_db.isOpen();
}

void CreationBD::fermerBD()
{
    if (m_estOuverte && m_db.isOpen()) {
        QString connectionName = m_db.connectionName();
        m_db.close();
        QSqlDatabase::removeDatabase(connectionName);
        m_estOuverte = false;
        qDebug() << "Base de données fermée";
    }
}

QString CreationBD::obtenirCheminBD() const
{
    return m_dbPath;
}

bool CreationBD::sauvegarderBD(const QString& cheminSauvegarde)
{
    if (!m_estOuverte) {
        qWarning() << "Base de données non ouverte";
        return false;
    }

    QFile fichierSource(m_dbPath);
    if (!fichierSource.exists()) {
        qWarning() << "Fichier de base de données introuvable:" << m_dbPath;
        return false;
    }

    // Supprimer le fichier de destination s'il existe
    if (QFile::exists(cheminSauvegarde)) {
        QFile::remove(cheminSauvegarde);
    }

    if (fichierSource.copy(cheminSauvegarde)) {
        qDebug() << "Sauvegarde réussie vers:" << cheminSauvegarde;
        return true;
    } else {
        qWarning() << "Erreur lors de la sauvegarde:" << fichierSource.errorString();
        return false;
    }
}
