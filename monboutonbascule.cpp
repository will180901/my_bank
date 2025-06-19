#include "monboutonbascule.h"
#include <QPainter>
#include <QCursor>
#include <QEnterEvent>

MonBoutonBascule::MonBoutonBascule(QWidget *parent)
    : QPushButton(parent),
    m_positionXCercleAnimee(2), // Position gauche (désactivé) avec marge très réduite
    m_animation(new QPropertyAnimation(this))
{
    setFixedSize(60, 10); // Largeur 60px, hauteur très réduite à 10px
    setText("");
    setCheckable(true);
    setCursor(Qt::ArrowCursor);

    // Force l'état désactivé
    setChecked(false);
    definirPositionXCercleAnimee(2); // Marge très réduite à 2px
    m_animation->setTargetObject(this);
    m_animation->setPropertyName("positionXCercleAnimee");
    m_animation->setDuration(450); // Animation plus rapide pour petite taille
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(this, &QPushButton::toggled, this, &MonBoutonBascule::basculerEtatInterne);
}

int MonBoutonBascule::obtenirPositionXCercleAnimee() const
{
    return m_positionXCercleAnimee;
}

void MonBoutonBascule::definirPositionXCercleAnimee(int pos)
{
    if (m_positionXCercleAnimee != pos) {
        m_positionXCercleAnimee = pos;
        update();
    }
}

void MonBoutonBascule::definirEtatBascule(bool etat)
{
    setChecked(etat);
}

void MonBoutonBascule::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::PointingHandCursor);
}

void MonBoutonBascule::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
}

void MonBoutonBascule::basculerEtatInterne(bool estActive)
{
    QRect zone = rect();
    int marge = 1; // Marge minimale de 1px
    int diametre = zone.height() - 2 * marge; // Diamètre très petit
    int positionFinale = estActive ? (zone.width() - diametre - marge) : marge;

    m_animation->stop();
    m_animation->setStartValue(m_positionXCercleAnimee);
    m_animation->setEndValue(positionFinale);
    m_animation->start();

    emit aBascule(estActive);
}

void MonBoutonBascule::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter peintre(this);
    peintre.setRenderHint(QPainter::Antialiasing);

    QRect zone = rect();
    int marge = 1; // Marge minimale de 1px
    int diametre = zone.height() - 2 * marge; // Cercle de 8px de diamètre (10 - 2*1)

    // Couleurs
    QColor fond = isChecked() ? QColor(41, 98, 255) : QColor(163, 164, 166);
    QColor cercle = Qt::white;

    // Fond
    peintre.setBrush(fond);
    peintre.setPen(Qt::NoPen);
    peintre.drawRoundedRect(zone, zone.height()/2, zone.height()/2);

    // Cercle
    QRect cercleRect(m_positionXCercleAnimee, marge, diametre, diametre);
    peintre.setBrush(cercle);
    peintre.drawEllipse(cercleRect);
}
