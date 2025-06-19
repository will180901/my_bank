#include "gestionbd.h"
#include <QApplication>
#include <QDateTime>

GestionBD::GestionBD(const QString& nomFichier) : m_nomFichier(nomFichier) {
    QString programDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(programDataPath);

    if (programDataPath.isEmpty() || !dir.exists()) {
        programDataPath = QDir::homePath() + "/AppData/Local/" + QApplication::applicationName();
        dir.setPath(programDataPath);
    }

    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qCritical() << "Impossible de créer le répertoire de données:" << programDataPath;
            return;
        }
        QFile::setPermissions(programDataPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
    }

    m_dbPath = dir.filePath(m_nomFichier);
    qDebug() << "Chemin de la base de données:" << m_dbPath;
}

GestionBD::~GestionBD() {
    fermerConnexion();
}

bool GestionBD::ouvrirConnexion() {
    if (m_dbPath.isEmpty()) {
        qCritical() << "Chemin de la base de données non initialisé";
        return false;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);

    bool dbExiste = QFile::exists(m_dbPath);

    if (!m_db.open()) {
        qCritical() << "Erreur connexion BD:" << m_db.lastError().text();
        return false;
    }

    if (!dbExiste) {
        if (!creerTables()) {
            qCritical() << "Erreur lors de la création des tables";
            return false;
        }

        if (!initialiserDonneesParDefaut()) {
            qCritical() << "Erreur lors de l'initialisation des données par défaut";
            return false;
        }
        qDebug() << "Nouvelle base de données créée avec données par défaut";
    }

    return true;
}

void GestionBD::fermerConnexion() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool GestionBD::creerTables() {
    QStringList requetes = {
        // Table Parametres
        "CREATE TABLE IF NOT EXISTS Parametres ("
        "cle TEXT PRIMARY KEY,"
        "valeur TEXT NOT NULL,"
        "date_modification TEXT DEFAULT CURRENT_TIMESTAMP)",

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
        "numero TEXT PRIMARY KEY,"
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
        "compte_source TEXT,"
        "compte_dest TEXT NOT NULL,"
        "montant REAL NOT NULL,"
        "description TEXT,"
        "date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(compte_source) REFERENCES Comptes(numero),"
        "FOREIGN KEY(compte_dest) REFERENCES Comptes(numero))",

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
    return true;
}

bool GestionBD::initialiserDonneesParDefaut() {
    // Paramètres par défaut
    QVariantMap parametresDefaut;
    parametresDefaut["theme"] = "system";
    parametresDefaut["langue"] = "fr";
    parametresDefaut["devise"] = "XAF";
    parametresDefaut["date_format"] = "dd/MM/yyyy";
    parametresDefaut["notifications_activees"] = true;
    parametresDefaut["taille_texte"] = "medium";
    parametresDefaut["banque_par_defaut"] = "BANQ001";

    for (auto it = parametresDefaut.constBegin(); it != parametresDefaut.constEnd(); ++it) {
        if (!enregistrerParametre(it.key(), it.value())) {
            return false;
        }
    }

    // Banque par défaut
    if (!creerBanque("MyBank", "BANQ001")) {
        return false;
    }

    return true;
}

bool GestionBD::executerRequete(const QString& requete) {
    QSqlQuery query;
    if (!query.exec(requete)) {
        qDebug() << "Erreur requête SQL:" << query.lastError().text();
        qDebug() << "Requête:" << requete;
        return false;
    }
    return true;
}

bool GestionBD::enregistrerParametre(const QString& cle, const QVariant& valeur) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO Parametres(cle, valeur) VALUES (?, ?)");
    query.addBindValue(cle);
    query.addBindValue(valeur.toString());

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'enregistrement du paramètre:" << query.lastError().text();
        return false;
    }
    return true;
}

QVariant GestionBD::obtenirParametre(const QString& cle, const QVariant& defaut) {
    QSqlQuery query;
    query.prepare("SELECT valeur FROM Parametres WHERE cle = ?");
    query.addBindValue(cle);

    if (query.exec() && query.next()) {
        return query.value(0);
    }
    return defaut;
}

QString GestionBD::hasherMotDePasse(const QString& motDePasse) {
    return QCryptographicHash::hash(
               motDePasse.toUtf8(),
               QCryptographicHash::Sha256
               ).toHex();
}

QString GestionBD::genererNumeroCompte(const QString& prefixe) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM Comptes WHERE numero LIKE ?");
    query.addBindValue(prefixe + "%");

    if (query.exec() && query.next()) {
        int count = query.value(0).toInt() + 1;
        return QString("%1%2").arg(prefixe).arg(count, 6, 10, QChar('0'));
    }
    return QString();
}

bool GestionBD::creerBanque(const QString& nom, const QString& codeBanque) {
    QSqlQuery query;
    query.prepare("INSERT INTO Banques(nom, code_banque) VALUES (?, ?)");
    query.addBindValue(nom);
    query.addBindValue(codeBanque);
    return query.exec();
}

int GestionBD::getBanqueId(const QString& codeBanque) {
    QSqlQuery query;
    query.prepare("SELECT id FROM Banques WHERE code_banque = ?");
    query.addBindValue(codeBanque);
    return (query.exec() && query.next()) ? query.value(0).toInt() : -1;
}

QString GestionBD::getNomBanque(const QString& codeBanque) {
    QSqlQuery query;
    query.prepare("SELECT nom FROM Banques WHERE code_banque = ?");
    query.addBindValue(codeBanque);
    return (query.exec() && query.next()) ? query.value(0).toString() : "";
}

bool GestionBD::creerUtilisateur(const QString& nomComplet,
                                 const QString& email,
                                 const QString& motDePasse) {
    QSqlQuery query;
    query.prepare("INSERT INTO Utilisateurs(nom_complet, email, mot_de_passe) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(nomComplet);
    query.addBindValue(email);
    query.addBindValue(hasherMotDePasse(motDePasse));
    return query.exec();
}

bool GestionBD::authentifierUtilisateur(const QString& email,
                                        const QString& motDePasse,
                                        QString& idUtilisateur) {
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

bool GestionBD::existeUtilisateur(const QString& email) {
    QSqlQuery query;
    query.prepare("SELECT 1 FROM Utilisateurs WHERE email = ?");
    query.addBindValue(email);
    return query.exec() && query.next();
}

bool GestionBD::creerCompte(const QString& typeCompte,
                            const QString& idUtilisateur,
                            const QString& codeBanque,
                            double soldeInitial,
                            double parametreSupplementaire) {
    int idBanque = getBanqueId(codeBanque);
    if (idBanque == -1) return false;

    QString prefixe = (typeCompte == "COURANT") ? "CC" : "CE";
    QString numeroCompte = genererNumeroCompte(prefixe);

    QSqlQuery query;
    query.prepare("INSERT INTO Comptes VALUES (?, ?, ?, ?, ?, ?, ?, CURRENT_TIMESTAMP)");
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

QList<CompteBancaire*> GestionBD::getComptesUtilisateur(const QString& idUtilisateur) {
    QList<CompteBancaire*> comptes;
    QSqlQuery query;
    query.prepare("SELECT numero FROM Comptes WHERE id_utilisateur = ?");
    query.addBindValue(idUtilisateur);

    if (query.exec()) {
        while (query.next()) {
            CompteBancaire* compte = getCompte(query.value(0).toString());
            if (compte) comptes.append(compte);
        }
    }
    return comptes;
}

CompteBancaire* GestionBD::getCompte(const QString& numeroCompte) {
    QSqlQuery query;
    query.prepare("SELECT c.*, b.nom as nom_banque FROM Comptes c "
                  "JOIN Banques b ON c.id_banque = b.id "
                  "WHERE c.numero = ?");
    query.addBindValue(numeroCompte);

    if (!query.exec() || !query.next()) return nullptr;

    QString type = query.value("type").toString();
    QString numero = query.value("numero").toString();
    double solde = query.value("solde").toDouble();
    QString banque = query.value("nom_banque").toString();

    // Récupération du titulaire
    QSqlQuery queryTitulaire;
    queryTitulaire.prepare("SELECT nom_complet FROM Utilisateurs WHERE id = ?");
    queryTitulaire.addBindValue(query.value("id_utilisateur"));
    QString titulaire = queryTitulaire.exec() && queryTitulaire.next()
                            ? queryTitulaire.value(0).toString()
                            : "Inconnu";

    if (type == "COURANT") {
        double decouvert = query.value("decouvert_autorise").toDouble();
        return new CompteCourant(numero, titulaire, solde, decouvert, banque);
    } else {
        double taux = query.value("taux_interet").toDouble();
        return new CompteEpargne(numero, titulaire, solde, taux, banque);
    }
}

bool GestionBD::effectuerTransaction(const QString& compteSource,
                                     const QString& compteDest,
                                     double montant,
                                     const QString& description) {
    m_db.transaction();

    try {
        // Vérification des comptes
        CompteBancaire* source = getCompte(compteSource);
        CompteBancaire* dest = getCompte(compteDest);
        if (!source || !dest || montant <= 0) throw std::runtime_error("Paramètres invalides");

        // Exécution du retrait
        if (!source->retirer(montant)) throw std::runtime_error("Retrait impossible");

        // Exécution du dépôt
        dest->deposer(montant);

        // Enregistrement de la transaction
        QSqlQuery query;
        query.prepare("INSERT INTO Transactions(compte_source, compte_dest, montant, description) "
                      "VALUES (?, ?, ?, ?)");
        query.addBindValue(compteSource);
        query.addBindValue(compteDest);
        query.addBindValue(montant);
        query.addBindValue(description);
        if (!query.exec()) throw std::runtime_error("Erreur enregistrement transaction");

        // Mise à jour des soldes
        QSqlQuery updateSource, updateDest;
        updateSource.prepare("UPDATE Comptes SET solde = ? WHERE numero = ?");
        updateSource.addBindValue(source->getSolde());
        updateSource.addBindValue(compteSource);

        updateDest.prepare("UPDATE Comptes SET solde = ? WHERE numero = ?");
        updateDest.addBindValue(dest->getSolde());
        updateDest.addBindValue(compteDest);

        if (!updateSource.exec() || !updateDest.exec()) {
            throw std::runtime_error("Erreur mise à jour soldes");
        }

        m_db.commit();
        return true;
    } catch (const std::exception& e) {
        m_db.rollback();
        qDebug() << "Erreur transaction:" << e.what();
        return false;
    }
}

QList<QString> GestionBD::getHistoriqueTransactions(const QString& numeroCompte) {
    QList<QString> historique;
    QSqlQuery query;
    query.prepare("SELECT t.*, "
                  "cs.numero as num_source, "
                  "cd.numero as num_dest "
                  "FROM Transactions t "
                  "LEFT JOIN Comptes cs ON t.compte_source = cs.numero "
                  "JOIN Comptes cd ON t.compte_dest = cd.numero "
                  "WHERE t.compte_source = ? OR t.compte_dest = ? "
                  "ORDER BY t.date DESC");
    query.addBindValue(numeroCompte);
    query.addBindValue(numeroCompte);

    if (query.exec()) {
        while (query.next()) {
            QString transaction = QString("[%1] %2 -> %3 | %4 | %5")
            .arg(query.value("date").toDateTime().toString("dd/MM/yyyy hh:mm"))
                .arg(query.value("num_source").toString())
                .arg(query.value("num_dest").toString())
                .arg(query.value("montant").toDouble(), 0, 'f', 2)
                .arg(query.value("description").toString());
            historique.append(transaction);
        }
    }
    return historique;
}

bool GestionBD::enregistrerConnexion(const QString& idUtilisateur, bool succes) {
    QSqlQuery query;
    query.prepare("INSERT INTO HistoriqueConnexion(id_utilisateur, succes) "
                  "VALUES (?, ?)");
    query.addBindValue(idUtilisateur);
    query.addBindValue(succes ? 1 : 0);
    return query.exec();
}
