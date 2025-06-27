#ifndef ANIMATIONSOLDE_H
#define ANIMATIONSOLDE_H
#include <QObject>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QLabel>
#include <QStyle>
#include <QMap>

class AnimationSolde : public QObject
{
    Q_OBJECT
public:
    explicit AnimationSolde(QObject *parent = nullptr);
    void appliquerAvecLabel(QLabel* label, bool masquer);

private:
    QGraphicsBlurEffect* creerEffetFlou(QLabel* label);
    void animerFlou(QGraphicsBlurEffect* effet, bool masquer);
    void animerTransition(QLabel* label, bool masquer);
    QString genererTexteMasque(const QString& original);
};

#endif // ANIMATIONSOLDE_H
