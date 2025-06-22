#include "gestionbd.h"
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include <stdexcept>

GestionBD::GestionBD(const QString& nomFichier, QObject *parent)
    : QObject(parent), m_nomFichier(nomFichier)
{
    // Définir le chemin par défaut pour la base de données
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDataDir(appDataPath);

    if (!appDataDir.exists()) {
        appDataDir.mkpath(appDataPath);
    }

    m_cheminBD = appDataPath + QDir::separator() + m_nomFichier;
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_cheminBD);
}

GestionBD::~GestionBD()
{
    fermerConnexion();
}

bool GestionBD::creerDossierBD()
{
    QFileInfo fileInfo(m_cheminBD);
    QDir dir(fileInfo.path());

    if (!dir.exists()) {
        return dir.mkpath(fileInfo.path());
    }
    return true;
}

bool GestionBD::ouvrirConnexion()
{
    if (!creerDossierBD()) {
        qDebug() << "Erreur création dossier BD";
        return false;
    }

    if (!m_db.open()) {
        qDebug() << "Erreur connexion BD:" << m_db.lastError().text();
        return false;
    }
    return creerTables();
}

void GestionBD::fermerConnexion()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

QString GestionBD::hasherMotDePasse(const QString& motDePasse)
{
    return QCryptographicHash::hash(
               motDePasse.toUtf8(),
               QCryptographicHash::Sha256
               ).toHex();
}

bool GestionBD::creerTables()
{
    QStringList requetes = {
        // Table Banques
        "CREATE TABLE IF NOT EXISTS Banques ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nom TEXT UNIQUE NOT NULL,"
        "code_banque TEXT UNIQUE NOT NULL,"
        "date_creation TEXT DEFAULT CURRENT_TIMESTAMP)",

        // Table Utilisateurs
        "CREATE TABLE IF NOT EXISTS Utilisateurs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nom_complet TEXT NOT NULL,"
        "email TEXT UNIQUE NOT NULL,"
        "mot_de_passe TEXT NOT NULL,"
        "date_creation TEXT DEFAULT CURRENT_TIMESTAMP)",

        // Table Comptes
        "CREATE TABLE IF NOT EXISTS Comptes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "numero TEXT UNIQUE NOT NULL,"
        "type TEXT NOT NULL CHECK(type IN ('COURANT', 'EPARGNE')),"
        "id_utilisateur INTEGER NOT NULL,"
        "id_banque INTEGER NOT NULL,"
        "solde REAL NOT NULL,"
        "decouvert_autorise REAL DEFAULT 0,"
        "taux_interet REAL DEFAULT 0,"
        "date_creation TEXT DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(id_utilisateur) REFERENCES Utilisateurs(id),"
        "FOREIGN KEY(id_banque) REFERENCES Banques(id))",

        // Table Transactions
        "CREATE TABLE IF NOT EXISTS Transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "compte_id INTEGER NOT NULL,"
        "type TEXT NOT NULL CHECK(type IN ('DEPOT', 'RETRAIT', 'VIREMENT')),"
        "montant REAL NOT NULL,"
        "date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(compte_id) REFERENCES Comptes(id))",

        // Table HistoriqueConnexion
        "CREATE TABLE IF NOT EXISTS HistoriqueConnexion ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "id_utilisateur INTEGER NOT NULL,"
        "date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "succes INTEGER NOT NULL,"
        "FOREIGN KEY(id_utilisateur) REFERENCES Utilisateurs(id))"
    };

    for (const QString& requete : requetes) {
        if (!executerRequete(requete)) {
            return false;
        }
    }

    // Créer une banque par défaut si elle n'existe pas
    if (!existeUtilisateur("admin@banque.com")) {
        creerBanque("Ma Banque", "BANK001");
    }

    return true;
}

bool GestionBD::executerRequete(const QString& requete)
{
    QSqlQuery query;
    if (!query.exec(requete)) {
        qDebug() << "Erreur requête SQL:" << query.lastError().text();
        return false;
    }
    return true;
}

QString GestionBD::genererNumeroCompte(const QString& prefixe)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Comptes WHERE numero LIKE ?");
    query.addBindValue(prefixe + "%");

    if (query.exec() && query.next()) {
        int count = query.value(0).toInt() + 1;
        return QString("%1%2").arg(prefixe).arg(count, 3, 10, QChar('0'));
    }
    return QString();
}

bool GestionBD::creerBanque(const QString& nom, const QString& codeBanque)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Banques(nom, code_banque) VALUES (?, ?)");
    query.addBindValue(nom);
    query.addBindValue(codeBanque);
    return query.exec();
}

int GestionBD::getBanqueId(const QString& codeBanque)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM Banques WHERE code_banque = ?");
    query.addBindValue(codeBanque);
    return (query.exec() && query.next()) ? query.value(0).toInt() : -1;
}

QString GestionBD::getNomBanque(const QString& codeBanque)
{
    QSqlQuery query;
    query.prepare("SELECT nom FROM Banques WHERE code_banque = ?");
    query.addBindValue(codeBanque);
    return (query.exec() && query.next()) ? query.value(0).toString() : "";
}

bool GestionBD::creerUtilisateur(const QString& nomComplet, const QString& email, const QString& motDePasse)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Utilisateurs(nom_complet, email, mot_de_passe) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(nomComplet);
    query.addBindValue(email);
    query.addBindValue(hasherMotDePasse(motDePasse));
    return query.exec();
}

bool GestionBD::authentifierUtilisateur(const QString& email, const QString& motDePasse, QString& idUtilisateur)
{
    QSqlQuery query;
    query.prepare("SELECT id, mot_de_passe FROM Utilisateurs WHERE email = ?");
    query.addBindValue(email);

    if (query.exec() && query.next()) {
        QString motDePasseHash = query.value("mot_de_passe").toString();
        if (motDePasseHash == hasherMotDePasse(motDePasse)) {
            idUtilisateur = query.value("id").toString();
            return true;
        }
    }
    return false;
}

bool GestionBD::existeUtilisateur(const QString& email)
{
    QSqlQuery query;
    query.prepare("SELECT 1 FROM Utilisateurs WHERE email = ?");
    query.addBindValue(email);
    return query.exec() && query.next();
}

bool GestionBD::creerCompte(const QString& typeCompte, const QString& idUtilisateur,
                            const QString& codeBanque, double soldeInitial, double parametreSupplementaire)
{
    int idBanque = getBanqueId(codeBanque);
    if (idBanque == -1) return false;

    QString prefixe = (typeCompte == "COURANT") ? "CC" : "CE";
    QString numeroCompte = genererNumeroCompte(prefixe);

    QSqlQuery query;
    query.prepare("INSERT INTO Comptes (numero, type, id_utilisateur, id_banque, solde, decouvert_autorise, taux_interet) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(numeroCompte);
    query.addBindValue(typeCompte);
    query.addBindValue(idUtilisateur);
    query.addBindValue(idBanque);
    query.addBindValue(soldeInitial);

    if (typeCompte == "COURANT") {
        query.addBindValue(parametreSupplementaire); // découvert
        query.addBindValue(0); // taux
    } else {
        query.addBindValue(0); // découvert
        query.addBindValue(parametreSupplementaire); // taux
    }

    return query.exec();
}

bool GestionBD::supprimerCompte(const QString& idCompte)
{
    QSqlQuery query;
    query.prepare("DELETE FROM Comptes WHERE id = ?");
    query.addBindValue(idCompte);
    return query.exec();
}

bool GestionBD::compteExiste(const QString& idUtilisateur, const QString& typeCompte)
{
    QSqlQuery query;
    query.prepare("SELECT 1 FROM Comptes WHERE id_utilisateur = ? AND type = ?");
    query.addBindValue(idUtilisateur);
    query.addBindValue(typeCompte);
    return query.exec() && query.next();
}

QList<CompteBancaire*> GestionBD::getComptesUtilisateur(const QString& idUtilisateur)
{
    QList<CompteBancaire*> comptes;
    QSqlQuery query;
    query.prepare("SELECT c.id, c.numero, c.type, c.solde, c.decouvert_autorise, c.taux_interet, "
                  "b.nom as nom_banque, u.nom_complet as nom_titulaire "
                  "FROM Comptes c "
                  "JOIN Banques b ON c.id_banque = b.id "
                  "JOIN Utilisateurs u ON c.id_utilisateur = u.id "
                  "WHERE c.id_utilisateur = ?");
    query.addBindValue(idUtilisateur);

    if (query.exec()) {
        while (query.next()) {
            QString id = query.value("id").toString();
            QString numero = query.value("numero").toString();
            QString type = query.value("type").toString();
            double solde = query.value("solde").toDouble();
            QString banque = query.value("nom_banque").toString();
            QString titulaire = query.value("nom_titulaire").toString();

            if (type == "COURANT") {
                double decouvert = query.value("decouvert_autorise").toDouble();
                CompteCourant* cc = new CompteCourant(id, numero, titulaire, solde, decouvert, banque);
                comptes.append(cc);
            } else {
                double taux = query.value("taux_interet").toDouble();
                CompteEpargne* ce = new CompteEpargne(id, numero, titulaire, solde, taux, banque);
                comptes.append(ce);
            }
        }
    }
    return comptes;
}

CompteBancaire* GestionBD::getCompte(const QString& numeroCompte)
{
    QSqlQuery query;
    query.prepare("SELECT c.id, c.type, c.solde, c.decouvert_autorise, c.taux_interet, "
                  "b.nom as nom_banque, u.nom_complet as nom_titulaire "
                  "FROM Comptes c "
                  "JOIN Banques b ON c.id_banque = b.id "
                  "JOIN Utilisateurs u ON c.id_utilisateur = u.id "
                  "WHERE c.numero = ?");
    query.addBindValue(numeroCompte);

    if (!query.exec() || !query.next()) return nullptr;

    QString id = query.value("id").toString();
    QString type = query.value("type").toString();
    double solde = query.value("solde").toDouble();
    QString banque = query.value("nom_banque").toString();
    QString titulaire = query.value("nom_titulaire").toString();

    if (type == "COURANT") {
        double decouvert = query.value("decouvert_autorise").toDouble();
        return new CompteCourant(id, numeroCompte, titulaire, solde, decouvert, banque);
    } else {
        double taux = query.value("taux_interet").toDouble();
        return new CompteEpargne(id, numeroCompte, titulaire, solde, taux, banque);
    }
}

bool GestionBD::enregistrerTransaction(const QString& compteId, const QString& type, double montant)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Transactions(compte_id, type, montant) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(compteId);
    query.addBindValue(type);
    query.addBindValue(montant);
    return query.exec();
}

bool GestionBD::effectuerDepot(const QString& numeroCompte, double montant)
{
    CompteBancaire* compte = getCompte(numeroCompte);
    if (!compte) return false;

    compte->deposer(montant);

    QSqlQuery query;
    query.prepare("UPDATE Comptes SET solde = ? WHERE numero = ?");
    query.addBindValue(compte->getSolde());
    query.addBindValue(numeroCompte);

    if (!query.exec()) return false;

    return enregistrerTransaction(compte->getId(), "DEPOT", montant);
}

bool GestionBD::effectuerRetrait(const QString& numeroCompte, double montant)
{
    CompteBancaire* compte = getCompte(numeroCompte);
    if (!compte || !compte->retirer(montant)) return false;

    QSqlQuery query;
    query.prepare("UPDATE Comptes SET solde = ? WHERE numero = ?");
    query.addBindValue(compte->getSolde());
    query.addBindValue(numeroCompte);

    if (!query.exec()) return false;

    return enregistrerTransaction(compte->getId(), "RETRAIT", montant);
}

bool GestionBD::effectuerVirement(const QString& compteSource, const QString& compteDest, double montant)
{
    if (!effectuerRetrait(compteSource, montant)) return false;
    if (!effectuerDepot(compteDest, montant)) {
        effectuerDepot(compteSource, montant); // Compensation
        return false;
    }
    return true;
}

QMap<QString, QVariant> GestionBD::getDerniereTransaction(const QString& compteId)
{
    QMap<QString, QVariant> transaction;
    QSqlQuery query;
    query.prepare("SELECT type, montant, strftime('%d/%m/%Y', date) as date_formatee "
                  "FROM Transactions "
                  "WHERE compte_id = ? "
                  "ORDER BY date DESC "
                  "LIMIT 1");
    query.addBindValue(compteId);

    if (query.exec() && query.next()) {
        transaction["type"] = query.value("type").toString();
        transaction["montant"] = query.value("montant").toDouble();
        transaction["date"] = query.value("date_formatee").toString();
    }
    return transaction;
}

QList<QString> GestionBD::getHistoriqueTransactions(const QString& numeroCompte)
{
    QList<QString> historique;
    CompteBancaire* compte = getCompte(numeroCompte);
    if (!compte) return historique;

    QSqlQuery query;
    query.prepare("SELECT t.type, t.montant, strftime('%d/%m/%Y', t.date) as date_formatee "
                  "FROM Transactions t "
                  "WHERE t.compte_id = ? "
                  "ORDER BY t.date DESC");
    query.addBindValue(compte->getId());

    if (query.exec()) {
        while (query.next()) {
            QString transaction = QString("[%1] %2 - %3 FCFA")
            .arg(query.value("date_formatee").toString())
                .arg(query.value("type").toString())
                .arg(query.value("montant").toDouble(), 0, 'f', 2);
            historique.append(transaction);
        }
    }
    return historique;
}

bool GestionBD::enregistrerConnexion(const QString& idUtilisateur, bool succes)
{
    QSqlQuery query;
    query.prepare("INSERT INTO HistoriqueConnexion(id_utilisateur, succes) "
                  "VALUES (?, ?)");
    query.addBindValue(idUtilisateur);
    query.addBindValue(succes ? 1 : 0);
    return query.exec();
}

bool GestionBD::modifierUtilisateur(const QString& idUtilisateur,
                                    const QString& nouveauNomComplet,
                                    const QString& nouvelEmail,
                                    const QString& nouveauMotDePasse)
{
    m_db.transaction();

    try {
        if (!nouvelEmail.isEmpty()) {
            QSqlQuery checkEmail;
            checkEmail.prepare("SELECT id FROM Utilisateurs WHERE email = ? AND id != ?");
            checkEmail.addBindValue(nouvelEmail);
            checkEmail.addBindValue(idUtilisateur);
            if (checkEmail.exec() && checkEmail.next()) {
                throw std::runtime_error("L'email est déjà utilisé par un autre compte");
            }
        }

        QSqlQuery query;
        if (nouveauMotDePasse.isEmpty()) {
            query.prepare("UPDATE Utilisateurs SET nom_complet = ?, email = ? WHERE id = ?");
            query.addBindValue(nouveauNomComplet);
            query.addBindValue(nouvelEmail);
            query.addBindValue(idUtilisateur);
        } else {
            query.prepare("UPDATE Utilisateurs SET nom_complet = ?, email = ?, mot_de_passe = ? WHERE id = ?");
            query.addBindValue(nouveauNomComplet);
            query.addBindValue(nouvelEmail);
            query.addBindValue(hasherMotDePasse(nouveauMotDePasse));
            query.addBindValue(idUtilisateur);
        }

        if (!query.exec()) {
            throw std::runtime_error("Erreur lors de la mise à jour");
        }

        m_db.commit();
        return true;
    } catch (const std::exception& e) {
        m_db.rollback();
        qDebug() << "Erreur modification utilisateur:" << e.what();
        return false;
    }
}
