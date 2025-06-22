#ifndef AUTHENTIFICATION_H
#define AUTHENTIFICATION_H

#include <QDialog>
#include <QRegularExpression>
#include <QLineEdit>
#include "gestionbd.h"

namespace Ui {
class Authentification;
}

class Authentification : public QDialog
{
    Q_OBJECT

public:
    explicit Authentification(QWidget *parent = nullptr);
    ~Authentification();

    static bool validerNom(const QString& nom);
    static bool validerEmail(const QString& email);
    static bool validerMotDePasse(const QString& motDePasse);

signals:
    void authentificationReussie(const QString& userId);

private slots:
    void on_btn_creer_compte_zone_connexion_clicked();
    void on_btn_se_connecter_zone_creer_compte_clicked();
    void on_btn_creer_compte_zone_creer_compte_clicked();
    void on_btn_se_connecter_zone_connexion_clicked();
    void viderMessagesErreur();

private:
    Ui::Authentification *ui;
    QString m_currentUserId;
    GestionBD m_gestionBD;

    void setupPasswordVisibilityToggle(QLineEdit* passwordLineEdit);
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // AUTHENTIFICATION_H
