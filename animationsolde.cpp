#include "animationsolde.h"
#include <QDebug>

AnimationSolde::AnimationSolde(QObject *parent) : QObject(parent) {}

void AnimationSolde::appliquerAvecLabel(QLabel* label, bool masquer)
{
    if (!label) return;
    QGraphicsBlurEffect* effetFlou = creerEffetFlou(label);
    animerFlou(effetFlou, masquer);
    animerTransition(label, masquer);
}

QGraphicsBlurEffect* AnimationSolde::creerEffetFlou(QLabel* label)
{
    QGraphicsBlurEffect* effetFlou = qobject_cast<QGraphicsBlurEffect*>(label->graphicsEffect());

    if (!effetFlou) {
        effetFlou = new QGraphicsBlurEffect(label);
        effetFlou->setBlurRadius(0);
        label->setGraphicsEffect(effetFlou);
    }

    return effetFlou;
}

void AnimationSolde::animerFlou(QGraphicsBlurEffect* effet, bool masquer)
{
    if (!effet) return;

    QPropertyAnimation* animation = new QPropertyAnimation(effet, "blurRadius", this);
    animation->setDuration(300);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    if (masquer) {
        animation->setStartValue(0);
        animation->setEndValue(8);
    } else {
        animation->setStartValue(8);
        animation->setEndValue(0);
    }

    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationSolde::animerTransition(QLabel* label, bool masquer=false)
{
    if (!label) return;

    // Q_UNUSED(masquer);
    // Sauvegarder le texte original dans une propriété dynamique
    static const char* PROP_ORIGINAL_TEXT = "originalText";

    if (masquer) {
        // Sauvegarde du texte original et masquage
        label->setProperty(PROP_ORIGINAL_TEXT, label->text());
        QString texteMasque = genererTexteMasque(label->text());
        label->setText(texteMasque);
        label->setProperty("class", "solde-label solde-masque");
    } else {
        // Restauration du texte original
        QVariant original = label->property(PROP_ORIGINAL_TEXT);
        if (original.isValid()) {
            label->setText(original.toString());
        }
        label->setProperty("class", "solde-label");
    }

    label->style()->unpolish(label);
    label->style()->polish(label);
    label->update();
}

QString AnimationSolde::genererTexteMasque(const QString& original)
{
    Q_UNUSED(original);
    // On remplace simplement tout le texte par une chaîne fixe d'étoiles
    // pour éviter de révéler la magnitude du solde.
    return QString("**** FCFA");
}
