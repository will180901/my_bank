#include "CreationBD.h"

CreationBD::CreationBD(QObject *parent) : QObject(parent), m_estOuverte(false)
{
}

CreationBD::~CreationBD()
{
    fermerBD();
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
    m_dbPath = dbDir.filePath("banque.db");
    return true;
}

bool CreationBD::ouvrirBD()
{
    if (m_estOuverte) {
        qWarning() << "La base est déjà ouverte";
        return true;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        qCritical() << "Erreur ouverture BD:" << m_db.lastError();
        return false;
    }

    m_estOuverte = true;
    return true;
}

void CreationBD::fermerBD()
{
    if (m_estOuverte) {
        m_db.close();
        m_estOuverte = false;
    }
}

QSqlDatabase CreationBD::getDatabase() const
{
    return m_db;
}

bool CreationBD::estOuverte() const
{
    return m_estOuverte;
}

bool CreationBD::creerTables()
{
    if (!m_estOuverte) {
        qCritical() << "Base non ouverte!";
        return false;
    }

    QSqlQuery query;
    bool success = true;

    // Table Utilisateur
    success = success && query.exec("CREATE TABLE IF NOT EXISTS utilisateur ("
                                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "nom TEXT NOT NULL, "
                                    "email TEXT NOT NULL UNIQUE, "
                                    "mot_de_passe TEXT NOT NULL)");

    // Table Compte
    success = success && query.exec("CREATE TABLE IF NOT EXISTS compte ("
                                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "numero_compte TEXT NOT NULL UNIQUE, "
                                    "nom_titulaire TEXT NOT NULL, "
                                    "solde REAL NOT NULL DEFAULT 0, "
                                    "type_compte TEXT NOT NULL, "
                                    "decouvert_autorise REAL DEFAULT 0, "
                                    "taux_interet REAL DEFAULT 0, "
                                    "date_creation TEXT, " // Colonne ajoutée
                                    "derniere_operation TEXT)"); // Colonne ajoutée

    // Table Transaction (avec date système)
    success = success && query.exec("CREATE TABLE IF NOT EXISTS transactions (" // Changé transaction -> transactions
                                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "id_compte INTEGER NOT NULL, "
                                    "type_operation TEXT NOT NULL, " // 'depot', 'retrait', 'virement'
                                    "compte_source TEXT, "
                                    "compte_beneficiaire TEXT, "
                                    "montant REAL NOT NULL, "
                                    "date_operation TEXT NOT NULL, " // Stockage en texte
                                    "FOREIGN KEY(id_compte) REFERENCES compte(id))");

    // Table Historique Connexion (avec date système)
    success = success && query.exec("CREATE TABLE IF NOT EXISTS historique_connexion ("
                                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "id_utilisateur INTEGER NOT NULL, "
                                    "date_connexion TEXT NOT NULL, " // Stockage en texte
                                    "FOREIGN KEY(id_utilisateur) REFERENCES utilisateur(id))");

    if (!success) {
        qCritical() << "Erreur création tables:" << query.lastError();
    }

    return success;
}
