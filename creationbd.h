#ifndef CREATIONBD_H
#define CREATIONBD_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QSqlError>
#include <QCryptographicHash>
#include <QFile>

class CreationBD : public QObject
{
    Q_OBJECT

public:
    explicit CreationBD(const QString& nomFichier, QObject *parent = nullptr);
    ~CreationBD();

    // Méthodes principales
    bool creerDossierBD();
    void ouvrirBD();
    bool creerTables();
    void fermerBD();

    bool sauvegarderBD(const QString& cheminSauvegarde);
    bool insererBanqueParDefaut();

    // Getters
    QSqlDatabase getDatabase() const;
    bool estOuverte() const;
    QString obtenirCheminBD() const;

signals:
    void baseOuverte();
    void baseFermee();
    void erreurBD(const QString& message);

private:
    QSqlDatabase m_db;
    QString m_dbPath;
    QString m_nomFichier;
    bool m_estOuverte;

    // Méthodes privées d'assistance
    void emettrErreur(const QString& message);
};

#endif // CREATIONBD_H
