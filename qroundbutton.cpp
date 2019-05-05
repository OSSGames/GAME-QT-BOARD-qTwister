#include "qroundbutton.h"
#include <QPainter>
#include <QCursor>
#include <QFont>

QRoundButton::QRoundButton(QString btnPictureFile, QString btnActivePictureFile, QString btnPressedPictureFile, QString btnDisabledPictureFile, QWidget *parent) :
    QPushButton(parent)
{
    //размерная политика
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    //курсор
    setCursor(QCursor(Qt::ArrowCursor));
    m_Active = false;
    setMouseTracking(true);
    //создаем пиксмапы
    p_OriginalButtonPixmap = new QPixmap(btnPictureFile);
    p_ButtonPixmap = new QPixmap();
    if (btnActivePictureFile != "") {
        p_OriginalActiveButtonPixmap = new QPixmap(btnActivePictureFile);
        p_ActiveButtonPixmap = new QPixmap();
    } else {
        p_OriginalActiveButtonPixmap = 0;
        p_ActiveButtonPixmap = 0;
    }
    if (btnPressedPictureFile != "") {
        p_OriginalPressedButtonPixmap = new QPixmap(btnPressedPictureFile);
        p_PressedButtonPixmap = new QPixmap();
    } else {
        p_OriginalPressedButtonPixmap = 0;
        p_PressedButtonPixmap = 0;
    }
    if (btnDisabledPictureFile != "") {
        p_OriginalDisabledButtonPixmap = new QPixmap(btnDisabledPictureFile);
        p_DisabledButtonPixmap = new QPixmap();
    } else {
        p_OriginalDisabledButtonPixmap = 0;
        p_DisabledButtonPixmap = 0;
    }
    //масштабируем
    resizePixmaps();
}

QRoundButton::~QRoundButton() {
    if (p_ButtonPixmap) {
        delete p_ButtonPixmap;
    }
    if (p_ActiveButtonPixmap) {
        delete p_ActiveButtonPixmap;
    }
    if (p_PressedButtonPixmap) {
        delete p_PressedButtonPixmap;
    }
    if (p_DisabledButtonPixmap) {
        delete p_DisabledButtonPixmap;
    }
    if (p_OriginalActiveButtonPixmap) {
        delete p_OriginalActiveButtonPixmap;
    }
    if (p_OriginalButtonPixmap) {
        delete p_OriginalButtonPixmap;
    }
    if (p_OriginalPressedButtonPixmap) {
        delete p_OriginalPressedButtonPixmap;
    }
    if (p_OriginalDisabledButtonPixmap) {
        delete p_OriginalDisabledButtonPixmap;
    }
}

/*virtual*/ QSize QRoundButton::sizeHint() const {
    return QSize(32,32);
}

/*virtual*/ void QRoundButton::paintEvent(QPaintEvent *) {
    //отрисовываем кнопку в зависимости от состояния
    QPainter painter(this);
    painter.setFont( QFont ( "Helvetica", 5*height()/32, QFont::DemiBold) );
    //отрисовываем отключенную кнопку, если есть такая картинка
    if ((!isEnabled()) && (p_DisabledButtonPixmap)) {
        painter.drawPixmap(0,0,*p_DisabledButtonPixmap);
        painter.drawText(width()/8, 3*height()/8, 6*width()/8, height()/4, Qt::AlignCenter, text());
        return ;
    }
    if (isDown()) {
        //кнопка нажата
        if (p_PressedButtonPixmap) {
            painter.drawPixmap(0,0,*p_PressedButtonPixmap);
        } else {
            painter.drawPixmap(0,0,*p_ButtonPixmap);
        }
    } else {
        //кнопка отжата
        if (m_Active) {
            //кнопка активна
            if (p_ActiveButtonPixmap) {
                painter.drawPixmap(0,0,*p_ActiveButtonPixmap);
            } else {
                painter.drawPixmap(0,0,*p_ButtonPixmap);
            }
        } else {
            //кнопка не активна
            painter.drawPixmap(0,0,*p_ButtonPixmap);
        }
    }
    //пишем текст
    painter.drawText(width()/8, 3*height()/8, 6*width()/8, height()/4, Qt::AlignCenter, text());
}

/*virtual*/ void QRoundButton::resizeEvent(QResizeEvent * e) {
    //перекэшируем изображения
    resizePixmaps();
}

/*virtual*/ void QRoundButton::mouseMoveEvent(QMouseEvent *e) {
    //запомним предыдущее состояние m_Active,
    //так как нужно перерисовывать кнопку при смене состояний
    bool oldActive = m_Active;
    if (hitButton(e->pos())) {
        //указующий курсор и подсветка появляются только
        //в области эллипса кнопки
        m_Active = true;
        setCursor(QCursor(Qt::PointingHandCursor));
    } else {
        m_Active = false;
        setCursor(QCursor(Qt::ArrowCursor));
    }
    //если активность сменила состояние, перерисовываем
    if (oldActive != m_Active) {
        repaint();
    }
}

/*virtual*/ bool QRoundButton::hitButton(const QPoint &pos) const {
    //кликабелен должен быть только эллипс
    QRegion region(0,0,width(),height(),QRegion::Ellipse);
    if (region.contains(pos)) {
        return true;
    } else {
        return false;
    }
}

//изменяем размеры кэшированных пиксмапов
void QRoundButton::resizePixmaps() {
    *p_ButtonPixmap = p_OriginalButtonPixmap->scaled(width()-1,height()-1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    if (p_OriginalActiveButtonPixmap) {
        *p_ActiveButtonPixmap = p_OriginalActiveButtonPixmap->scaled(width()-1,height()-1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    if (p_OriginalPressedButtonPixmap) {
        *p_PressedButtonPixmap = p_OriginalPressedButtonPixmap->scaled(width()-1,height()-1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    if (p_OriginalDisabledButtonPixmap) {
        *p_DisabledButtonPixmap = p_OriginalDisabledButtonPixmap->scaled(width()-1,height()-1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
}
