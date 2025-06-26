#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QSqlError>

class CreationBD : public QObject
{
    Q_OBJECT

public:
    explicit CreationBD(QObject *parent = nullptr);
    ~CreationBD();

    // Méthodes principales
    bool creerDossierBD();
    bool ouvrirBD();
    void fermerBD();
    bool creerTables();

    // Getters
    QSqlDatabase getDatabase() const;
    bool estOuverte() const;

private:
    QSqlDatabase m_db;
    QString m_dbPath;
    bool m_estOuverte;
};
