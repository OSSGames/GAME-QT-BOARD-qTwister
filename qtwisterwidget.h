#ifndef QTWISTERWIDGET_H
#define QTWISTERWIDGET_H

#include <QWidget>
#include <QPicture>
#include <QLabel>
#include "qroundbutton.h"

enum TTwisterParts {tpRightHand = 0, tpRightFoot = 1, tpLeftFoot = 2, tpLeftHand = 3};
enum TTwisterColors {tcRed = 0, tcGreen = 1, tcYellow = 2, tcBlue = 3};

typedef struct {
    TTwisterParts Part;
    TTwisterColors Color;
} TTwisterValue;

class QTwisterWidget : public QWidget
{
Q_OBJECT
public:
    explicit QTwisterWidget(QWidget *parent = 0);
    ~QTwisterWidget();

    virtual QSize sizeHint() const;

    int getTimerValue() {return m_TimerValue;};
    void setTimerValue(int value) {m_TimerValue = value;};
    int getAnimationSpeed() {return m_ArrowAnimationSpeed;};
    int getSmoothness() {return m_Smoothness;};
    void setAnimationSpeed(int value) {m_ArrowAnimationSpeed = value; m_Smoothness = calculateSmoothness(value);};
    bool getAllowSameResults() {return m_AllowSameResults;};
    void setAllowSameResults(bool value) {m_AllowSameResults = value;};
    QRoundButton * getRoundButtonPtr() {return roundButton;};

signals:
    void valueChanged(TTwisterValue value);
    void timerDead();

public slots:
    void changeValue(TTwisterValue value);
    void generate();
    void startTimerSlot();
    void killTimerSlot();
    void roundButtonClick();

protected:
    TTwisterValue m_twValue;
    int m_Timerid;
    int m_TimerValue;
    int m_ArrowAnimationSpeed;
    int m_Smoothness;
    bool m_AllowSameResults;

    QPixmap * p_SourceRhand;
    QPixmap * p_SourceLhand;
    QPixmap * p_SourceRfoot;
    QPixmap * p_SourceLfoot;

    QPixmap * p_Rhand;
    QPixmap * p_Lhand;
    QPixmap * p_Rfoot;
    QPixmap * p_Lfoot;

    QPixmap * p_TwisterCircle;

    QLabel * infoLabel;
    QRoundButton * roundButton;

    int m_Angle;
    bool m_Animation;

    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual void changeEvent(QEvent *event);

    void paintResult(QPainter * painter);
    void paintArrow(QPainter * painter, int angle);
    void animateArrow(int angle);
    void resizeObjects();
    void repaintTwisterCircle();
    int calculateAngle();
    int calculateSmoothness(int speed);
};

#endif // QTWISTERWIDGET_H
