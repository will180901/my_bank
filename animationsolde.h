#ifndef ANIMATIONSOLDE_H
#define ANIMATIONSOLDE_H
#include <QObject>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QLabel>
#include <QStyle>

class AnimationSolde : public QObject
{
    Q_OBJECT
public:
    explicit AnimationSolde(QObject *parent = nullptr);
    void appliquerAvecLabel(QLabel* label, bool masquer);

private:
    struct StyleLabel {
        QString texte;
        QString feuilleStyle;
        QFont police;
    };

    QGraphicsBlurEffect* creerEffetFlou(QLabel* label);
    void animerFlou(QGraphicsBlurEffect* effet, bool masquer);
    void animerTransition(QLabel* label, bool masquer);
    QString genererTexteMasque(const QString& original);

    QMap<QLabel*, StyleLabel> m_stylesOriginaux;
};


#endif // ANIMATIONSOLDE_H
