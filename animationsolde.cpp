#include "animationsolde.h"
#include <QDebug>

AnimationSolde::AnimationSolde(QObject *parent) : QObject(parent) {}

void AnimationSolde::appliquerAvecLabel(QLabel* label, bool masquer)
{
    if (!label) return;

    // Sauvegarder le style original au premier masquage
    if (!m_stylesOriginaux.contains(label)) {
        m_stylesOriginaux[label] = {
            label->text(),
            label->styleSheet(),
            label->font()
        };
    }

    QGraphicsBlurEffect* effetFlou = creerEffetFlou(label);
    animerFlou(effetFlou, masquer);
    animerTransition(label, masquer);
}

QGraphicsBlurEffect* AnimationSolde::creerEffetFlou(QLabel* label)
{
    if (label->graphicsEffect()) {
        return qobject_cast<QGraphicsBlurEffect*>(label->graphicsEffect());
    }

    QGraphicsBlurEffect* effetFlou = new QGraphicsBlurEffect(label);
    effetFlou->setBlurRadius(10);
    label->setGraphicsEffect(effetFlou);
    return effetFlou;
}

void AnimationSolde::animerFlou(QGraphicsBlurEffect* effet, bool masquer)
{
    QPropertyAnimation* animation = new QPropertyAnimation(effet, "blurRadius", this);
    animation->setDuration(300);
    animation->setStartValue(masquer ? 0 : 8);
    animation->setEndValue(masquer ? 8 : 0);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationSolde::animerTransition(QLabel* label, bool masquer)
{
    if (!label) return;

    if (masquer) {
        label->setText("XAF *****");
        label->setProperty("class", "solde-label solde-masque");
    } else {
        label->setProperty("class", "solde-label");
        if (m_stylesOriginaux.contains(label)) {
            label->setText(m_stylesOriginaux[label].texte);
        }
    }

    label->style()->unpolish(label);
    label->style()->polish(label);
    label->update();
}

QString AnimationSolde::genererTexteMasque(const QString& original)
{
    int nombreCaracteres = original.length();
    if (nombreCaracteres <= 4) return "XAF *****";

    QString texteMasque;
    texteMasque.reserve(nombreCaracteres);
    texteMasque += "XAF ";

    for (int i = 4; i < nombreCaracteres; ++i) {
        texteMasque += '*';
    }

    return texteMasque;
}
