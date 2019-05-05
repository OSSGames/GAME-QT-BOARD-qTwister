#ifndef QROUNDBUTTON_H
#define QROUNDBUTTON_H

#include <QPushButton>
#include <QPixmap>
#include <QString>
#include <QtEvents>

class QRoundButton : public QPushButton
{
Q_OBJECT
public:
    explicit QRoundButton(QString btnPictureFile, QString btnActivePictureFile = "", QString btnPressedPictureFile = "", QString btnDisabledPictureFile = "", QWidget *parent = 0);
    ~QRoundButton();

    virtual QSize sizeHint() const;

protected:
    //pixmap-ы для кэширования масштабированных изображений
    QPixmap * p_ButtonPixmap;
    QPixmap * p_ActiveButtonPixmap;
    QPixmap * p_PressedButtonPixmap;
    QPixmap * p_DisabledButtonPixmap;
    //оригинальные изображения
    QPixmap * p_OriginalButtonPixmap;
    QPixmap * p_OriginalActiveButtonPixmap;
    QPixmap * p_OriginalPressedButtonPixmap;
    QPixmap * p_OriginalDisabledButtonPixmap;
    //активна ли кнопка
    bool m_Active;
    //текст на кнопке


    void resizePixmaps();
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual bool hitButton(const QPoint &pos) const;
    virtual void mouseMoveEvent(QMouseEvent *e);
};

#endif // QROUNDBUTTON_H
