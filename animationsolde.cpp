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

void AnimationSolde::restaurerLabel(QLabel* label)
{
    if (!label || !m_stylesOriginaux.contains(label)) return;

    // Restaurer le texte original
    label->setText(m_stylesOriginaux[label].texte);
    label->setStyleSheet(m_stylesOriginaux[label].feuilleStyle);
    label->setFont(m_stylesOriginaux[label].police);

    // Supprimer l'effet de flou
    if (label->graphicsEffect()) {
        label->setGraphicsEffect(nullptr);
    }

    label->setProperty("class", "solde-label");
    label->style()->unpolish(label);
    label->style()->polish(label);
    label->update();
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

void AnimationSolde::animerTransition(QLabel* label, bool masquer)
{
    if (!label) return;

    if (masquer) {
        // Générer le texte masqué basé sur l'original
        QString texteOriginal = m_stylesOriginaux.contains(label) ?
                                    m_stylesOriginaux[label].texte : label->text();
        QString texteMasque = genererTexteMasque(texteOriginal);

        label->setText(texteMasque);
        label->setProperty("class", "solde-label solde-masque");
    } else {
        // Restaurer le texte original
        if (m_stylesOriginaux.contains(label)) {
            label->setText(m_stylesOriginaux[label].texte);
        }
        label->setProperty("class", "solde-label");
    }

    label->style()->unpolish(label);
    label->style()->polish(label);
    label->update();
}

QString AnimationSolde::genererTexteMasque(const QString& original)
{
    // Extraire le montant numérique du texte original
    QString texteNettoye = original.trimmed();

    // Si le texte commence par "XAF", compter les caractères après
    if (texteNettoye.startsWith("XAF", Qt::CaseInsensitive)) {
        int longueurMontant = texteNettoye.length() - 4; // Enlever "XAF "
        if (longueurMontant <= 0) return "XAF *****";

        return "XAF " + QString("*").repeated(qMax(5, longueurMontant));
    }

    // Sinon, créer un masque basé sur la longueur totale
    int nombreCaracteres = texteNettoye.length();
    if (nombreCaracteres <= 4) return "XAF *****";

    return "XAF " + QString("*").repeated(nombreCaracteres - 4);
}
