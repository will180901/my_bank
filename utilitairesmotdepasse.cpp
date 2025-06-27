#include "utilitairesmotdepasse.h"
#include <QStyle>
#include <QEvent>
#include <QResizeEvent>
#include <QObject>
#include <QTimer>
#include <QApplication>
#include <QRegularExpression>

class FiltreEvenementUtilitaires : public QObject
{
public:
    FiltreEvenementUtilitaires(QLineEdit* champ, QObject* parent = nullptr)
        : QObject(parent), m_champ(champ) {}

protected:
    bool eventFilter(QObject* obj, QEvent* event) override
    {
        if (event->type() == QEvent::Resize) {
            UtilitairesMotDePasse::repositionnerBouton(m_champ);
        }
        return QObject::eventFilter(obj, event);
    }

private:
    QLineEdit* m_champ;
};

void UtilitairesMotDePasse::configurerBoutonVisibilite(QLineEdit* champMotDePasse)
{
    QPushButton* bouton = new QPushButton(champMotDePasse);
    bouton->setCursor(Qt::PointingHandCursor);
    bouton->setCheckable(true);
    bouton->setChecked(false);

    // Style simplifié
    bouton->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   background: transparent;"
        "   padding: 0px;"
        "   margin: 0px;"
        "   width: 24px;"
        "}");

    // Icône
    QIcon icone;
    icone.addFile(":/icon_gris/eye.svg", QSize(), QIcon::Normal, QIcon::Off);
    icone.addFile(":/icon_gris/eye-off.svg", QSize(), QIcon::Normal, QIcon::On);
    bouton->setIcon(icone);
    bouton->setIconSize(QSize(16, 16));

    // Positionnement initial après un court délai
    QTimer::singleShot(10, [champMotDePasse]() {
        repositionnerBouton(champMotDePasse);
    });

    // Connexion pour basculer la visibilité
    QObject::connect(bouton, &QPushButton::toggled, [champMotDePasse]() {
        champMotDePasse->setEchoMode(champMotDePasse->echoMode() == QLineEdit::Password
                                         ? QLineEdit::Normal
                                         : QLineEdit::Password);
    });

    // Installer le filtre d'événements
    champMotDePasse->installEventFilter(new FiltreEvenementUtilitaires(champMotDePasse, champMotDePasse));
}

void UtilitairesMotDePasse::repositionnerBouton(QLineEdit* champMotDePasse)
{
    QPushButton* bouton = nullptr;
    for (QObject* enfant : champMotDePasse->children()) {
        if ((bouton = qobject_cast<QPushButton*>(enfant))) {
            break;
        }
    }

    if (!bouton) {
        return;
    }

    // 1. Forcer la taille du bouton pour une consistance visuelle
    bouton->setFixedSize(18, 18);

    // 2. Calcul précis en tenant compte des styles de l'application
    QStyle* style = QApplication::style();
    int largeurBordure = style->pixelMetric(QStyle::PM_DefaultFrameWidth);
    int margeDroite = largeurBordure + 4; // Marge réduite

    // Position X: extrême droite (en tenant compte des bordures)
    int posX = champMotDePasse->width() - bouton->width() - margeDroite;

    // Position Y: centré verticalement avec ajustement pour les champs hauts
    int hauteurChamp = champMotDePasse->height();
    int hauteurBouton = bouton->height();
    int posY = (hauteurChamp - hauteurBouton) / 2;

    // Ajustement pour les styles avec bordures épaisses
    if (hauteurChamp > 40) { // Champ plus grand dans la fenêtre principale
        posY = (hauteurChamp - hauteurBouton) / 2 - 2;
    }

    bouton->move(posX, posY);

    // 3. Ajustement du padding sans écraser les styles existants
    int paddingDroit = bouton->width() + margeDroite * 2;

    // Récupérer le style actuel
    QString styleActuel = champMotDePasse->styleSheet();

    // Supprimer uniquement les règles de padding-right existantes avec QRegularExpression
    QRegularExpression regex("padding-right\\s*:\\s*\\d+px\\s*;");
    styleActuel.remove(regex);

    // Ajouter le nouveau padding
    styleActuel += QString("padding-right: %1px;").arg(paddingDroit);

    // Appliquer uniquement la modification de padding
    champMotDePasse->setStyleSheet("QLineEdit { " + styleActuel + " }");
}
