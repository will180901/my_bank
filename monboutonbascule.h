#ifndef MONBOUTONBASCULE_H
#define MONBOUTONBASCULE_H

#include <QPushButton>
#include <QPropertyAnimation>

class MonBoutonBascule : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int positionXCercleAnimee READ obtenirPositionXCercleAnimee WRITE definirPositionXCercleAnimee)

public:
    explicit MonBoutonBascule(QWidget *parent = nullptr);
    void definirEtatBascule(bool etat);
    bool etatBascule() const { return isChecked(); }

signals:
    void aBascule(bool etat);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void basculerEtatInterne(bool estCoche);

private:
    int m_positionXCercleAnimee;
    QPropertyAnimation *m_animation;

    int obtenirPositionXCercleAnimee() const;
    void definirPositionXCercleAnimee(int pos);
};

#endif
