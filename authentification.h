#ifndef AUTHENTIFICATION_H
#define AUTHENTIFICATION_H

#include <QDialog>
#include <QLineEdit>
#include "gestionbd.h"
#include "fenetreprincipale.h"

namespace Ui {
class Authentification;
}

class Authentification : public QDialog
{
    Q_OBJECT

public:
    explicit Authentification(QWidget *parent = nullptr);
    ~Authentification();

    QString getCurrentUserId() const { return m_currentUserId; }

    // Méthodes de validation rendues publiques et statiques
    static bool validerNom(const QString& nom);
    static bool validerEmail(const QString& email);
    static bool validerMotDePasse(const QString& motDePasse);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void on_btn_creer_compte_zone_connexion_clicked();
    void on_btn_se_connecter_zone_connexion_clicked();
    void on_btn_creer_compte_zone_creer_compte_clicked();
    void on_btn_se_connecter_zone_creer_compte_clicked();

signals:
    void authentificationReussie(const QString& userId);

private:
    Ui::Authentification *ui;
    GestionBD m_gestionBD;
    QString m_currentUserId;

    void setupPasswordVisibilityToggle(QLineEdit* passwordLineEdit);
    void viderMessagesErreur();
};

#endif // AUTHENTIFICATION_H
