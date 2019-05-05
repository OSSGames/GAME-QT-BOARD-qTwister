#include "qtwisterwidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPixmap>
#include <QApplication>
#include <QResizeEvent>
#include <QFont>
#include <QBitmap>
#include <stdlib.h>
#include <QTime>
#include <time.h>

QTwisterWidget::QTwisterWidget(QWidget *parent) :
    QWidget(parent)
{
    //размерная политика
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    //значение по-умолчанию
    m_twValue.Color = tcRed;
    m_twValue.Part = tpRightHand;
    //инициализируем генератор
    qsrand(time(0));
    //обнуляем таймер
    m_Timerid = 0;
    m_TimerValue = 2000;
    //разрешаем одинаковые результаты
    m_AllowSameResults = true;
    //создаем изображения
    m_Angle = calculateAngle();
    //оригинальные изображения
    p_SourceRhand = new QPixmap("pictures/right_hand.svg");
    p_SourceRfoot = new QPixmap("pictures/right_foot.svg");
    p_SourceLhand = new QPixmap("pictures/left_hand.svg");
    p_SourceLfoot = new QPixmap("pictures/left_foot.svg");
    //масштабированные изображения
    p_Rhand = new QPixmap();
    p_Rfoot = new QPixmap();
    p_Lhand = new QPixmap();
    p_Lfoot = new QPixmap();
    resizeObjects();
    //буфер с кругом твистера
    p_TwisterCircle = 0;
    repaintTwisterCircle();
    //анимация еще не включена
    m_Animation = false;
    m_ArrowAnimationSpeed = 50;
    m_Smoothness = 15;
    //создаем лейбл с информацией
    QString strInfo("<P align=\"center\">qTwister v. 1.02</P>"
                    "<P>by Graker: <A href=\"http://graker.ru\">http://graker.ru</A></P>"
                    "<P align=\"center\">2010, <a href=\"http://www.gnu.org/licenses/gpl.html\">GPL v.3</a></P>");
    infoLabel = new QLabel(strInfo,this);
    infoLabel->setOpenExternalLinks(true);
    //создаем круглую кнопку
    roundButton = new QRoundButton("pictures/button1.png","pictures/button1_active.png","pictures/button1_pressed.png","pictures/button1_disabled.png",this);
    roundButton->setText(tr("Twist!"));
    //кнопка должна запускать твистер
    connect(roundButton,SIGNAL(clicked()),this,SLOT(roundButtonClick()));
}

//деструктор
QTwisterWidget::~QTwisterWidget() {
    delete p_SourceRhand;
    delete p_SourceLhand;
    delete p_SourceRfoot;
    delete p_SourceLfoot;
    delete p_Rhand;
    delete p_Lhand;
    delete p_Rfoot;
    delete p_Lfoot;
    delete p_TwisterCircle;
}

/*virtual*/ QSize QTwisterWidget::sizeHint() const {
    return QSize(600,600);
}

/*virtual*/ void QTwisterWidget::paintEvent(QPaintEvent *) {
    QPainter painter;
    painter.begin(this);
    //отрисовка игрового поля, буферизованного в QPixmap
    painter.drawPixmap(0,0,*p_TwisterCircle);
    //отрисовка результата
    if (!m_Animation) { //не отрисовываем результат, пока анимация не готова
        paintResult(&painter);
    }    
    //отрисовка стрелки
    paintArrow(&painter, m_Angle);
    painter.end();
}

/*virtual*/ void QTwisterWidget::resizeEvent(QResizeEvent * e) {
    //чтобы запретить эллипсы, нужно чтобы ширина была равна длине
    QSize newSize = e->size();
    if (newSize.height() != newSize.width()) {
        (newSize.height() < newSize.width()) ? resize(newSize.height(), newSize.height()) : resize(newSize.width(), newSize.width());
    }
    //изменяем положение лейбла с информацией
    infoLabel->setGeometry(width()/2-4*width()/32-width()/64,height()/2+3*height()/32,8*width()/32,3*height()/16);
    //изменяем шрифт лейбла
    QFont font = infoLabel->font();
    font.setPixelSize(3*height()/136);
    infoLabel->setFont(font);
    infoLabel->adjustSize();
    //изменяем размеры кнопки
    int bW = 3*width()/32;
    int bH = 3*height()/32;
    roundButton->setGeometry(width()/2-bW/2-width()/64,height()/2-bH/2,bW,bH);
    //изменяем размеры объектов
    resizeObjects();
    repaintTwisterCircle();
}

/**
  *
  * По событию таймера функция генерирует новые значения
  *
  */
/*virtual*/ void QTwisterWidget::timerEvent(QTimerEvent *) {
    generate();
}

void QTwisterWidget::changeValue(TTwisterValue value) {
    m_twValue = value;
    //включаем анимацию
    int angle = calculateAngle();
    //тестовая анимация с тремя оборотами
    animateArrow(angle+360);
    //обновляем p_Angle
    m_Angle = calculateAngle();
    //перерисовываем с новым значением
    repaint();
    emit valueChanged(value);
}


/**
  *
  * Функция генерирует новое значение
  *
  */
void QTwisterWidget::generate() {
    TTwisterValue twVal;
    int randval = qrand() % 16;
    twVal.Color = TTwisterColors(randval % 4);
    twVal.Part = TTwisterParts(randval / 4);
    //проверяем, можно ли генерировать одно и то же значение подряд
    if (!m_AllowSameResults) {
        //если нельзя, будем генерировать новый результат до тех пор,
        //пока он не станет отличен от старого
        while ((twVal.Color == m_twValue.Color) && (twVal.Part == m_twValue.Part)) {
            randval = qrand() % 16;
            twVal.Color = TTwisterColors(randval % 4);
            twVal.Part = TTwisterParts(randval / 4);
        }
    }
    changeValue(twVal);
}


/**
  *
  * Функция реагирует на нажатие круглой кнопки в зависимости от состояния таймера
  *
  */
void QTwisterWidget::roundButtonClick() {
    if (m_Timerid) {
        //таймер запущен, надо его убивать
        killTimerSlot();
    } else {
        //можно генерировать значение
        generate();
    }
}


/**
  *
  * Слот старта таймера
  *
  */
void QTwisterWidget::startTimerSlot() {
    if (!m_Timerid) { //если таймер еще не запущен
        m_Timerid = startTimer(m_TimerValue);
        //меняем текст на круглой кнопке
        roundButton->setText(tr("Stop"));
    }
}

/**
  *
  * Слот остановки таймера
  *
  */
void QTwisterWidget::killTimerSlot() {
    if (m_Timerid) { //если таймер запущен
        killTimer(m_Timerid);
    }
    m_Timerid = 0;
    //восстанавливаем текст на круглой кнопке
    roundButton->setText(tr("Twist!"));
    //сообщаем, что таймер умер
    emit timerDead();
}

/**
  *
  * Функция отрисовки результата
  *
  */
void QTwisterWidget::paintResult(QPainter *painter) {
    //результат в круге
    int x1;
    int y1;
    int x2;
    int y2;
    int w = 2*width()/16;
    painter->setRenderHint(QPainter::Antialiasing,true);
    //определяем цвет
    switch (m_twValue.Color) {
      case tcRed:
        painter->setBrush(QBrush(Qt::red, Qt::SolidPattern));
        break;
      case tcGreen:
        painter->setBrush(QBrush(Qt::green, Qt::SolidPattern));
        break;
      case tcYellow:
        painter->setBrush(QBrush(Qt::yellow, Qt::SolidPattern));
        break;
      case tcBlue:
        painter->setBrush(QBrush(Qt::blue, Qt::SolidPattern));
        break;
    }
    //определяем конечность
    switch(m_twValue.Part) {
      case tpLeftFoot :
        x1 = width()/2-2*width()/16-width()/64;
        y1 = height()/2-2*width()/32-2*height()/16;
        x2 = width()/2+width()/64-width()/64;
        y2 = height()/2-3*height()/64-2*height()/16;
        painter->drawPixmap(x1,y1,p_Lfoot->scaled(w,w,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        painter->drawEllipse(x2,y2,7*w/8,7*w/8);
        break;
      case tpLeftHand:
        x1 = width()/2-2*width()/16-width()/64;
        y1 = height()/2-2*width()/32-2*height()/16;
        x2 = width()/2+width()/64-width()/64;
        y2 = height()/2-3*height()/64-2*height()/16;
        painter->drawPixmap(x1,y1,p_Lhand->scaled(w,w,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        painter->drawEllipse(x2,y2,7*w/8,7*w/8);
        break;
      case tpRightFoot:
        x1 = width()/2-2*width()/16-width()/64;
        y1 = height()/2-3*height()/64-2*height()/16;
        x2 = width()/2+width()/64-width()/64;
        y2 = height()/2-2*width()/32-2*height()/16;
        painter->drawPixmap(x2,y2,p_Rfoot->scaled(w,w,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        painter->drawEllipse(x1,y1,7*w/8,7*w/8);
        break;
      case tpRightHand:
        x1 = width()/2-2*width()/16-width()/64;
        y1 = height()/2-3*height()/64-2*height()/16;
        x2 = width()/2+width()/64-width()/64;
        y2 = height()/2-2*width()/32-2*height()/16;
        painter->drawPixmap(x2,y2,p_Rhand->scaled(w,w,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        painter->drawEllipse(x1,y1,7*w/8,7*w/8);
        break;
    }
}

/**
  *
  * Функция отрисовки стрелки
  *
  * @param int angle - угол поворота стрелки
  *
  * @return QPicture& - указатель на картинку со стрелкой
  *
  */
void QTwisterWidget::paintArrow(QPainter * painter, int angle) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->translate(width()/2-width()/64,height()/2);
    painter->rotate(angle);
    painter->setPen(QPen(Qt::black,4,Qt::SolidLine));
    painter->drawLine(0,-height()/2+3*height()/16,0,height()/2-3*height()/16);
    painter->setPen(QPen(Qt::black,1,Qt::SolidLine));
    painter->setBrush(QBrush(Qt::black,Qt::SolidPattern));
    int wD = width()/48;
    int hD = height()/24;
    QPoint arrowHead[3] = {
      QPoint(0,-height()/2+3*height()/16-hD/2),
      QPoint(-wD,-height()/2+3*height()/16+hD/2),
      QPoint(+wD,-height()/2+3*height()/16+hD/2),
    };
    painter->drawPolygon(arrowHead,3);
    painter->restore();
}

/**
  *
  * Функция отрисовки
  *
  */
void QTwisterWidget::animateArrow(int angle) {
    //на время анимации выключаем кнопку
    roundButton->setEnabled(false);
    m_Animation = true;
    int dA = (30 - m_Smoothness); //чем меньше угол, тем больше плавность
    QTime time;
    int i;
    for (i = m_Angle; i < angle; i += dA) {
        m_Angle = i;
        repaint();
        //задержка для изменения скорости анимации
        int msec = 100 - m_ArrowAnimationSpeed; //сколько миллисекунд на каждый угол ждать
        time.start();
        for (; time.elapsed() < msec; ) {};
        QApplication::processEvents();
    }
    m_Animation = false;
    //включаем кнопку
    roundButton->setEnabled(true);
}

/**
  *
  * Функция вычисления угла поворота
  *
  */
int QTwisterWidget::calculateAngle() {
    return int(float(m_twValue.Part*4 + m_twValue.Color)*22.5 + 11);
}


/**
  *
  * Функция изменения размеров объектов при изменении размеров виджета
  *
  */
void QTwisterWidget::resizeObjects() {
    *p_Rhand = p_SourceRhand->scaled(3*width()/16,3*width()/16,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    *p_Lhand = p_SourceLhand->scaled(3*width()/16,3*width()/16,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    *p_Rfoot = p_SourceRfoot->scaled(3*width()/16,3*width()/16,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    *p_Lfoot = p_SourceLfoot->scaled(3*width()/16,3*width()/16,Qt::KeepAspectRatio,Qt::SmoothTransformation);
}


/**
  *
  * Функция перерисовывает игровой круг для его буферизации в QPixmap * p_TwisterCircle
  *
  */
void QTwisterWidget::repaintTwisterCircle() {
    if (p_TwisterCircle) delete p_TwisterCircle;
    p_TwisterCircle = new QPixmap(width(),height());
    QPainter painter;
    painter.begin(p_TwisterCircle);
    painter.setRenderHint(QPainter::Antialiasing,true);
    //закрасим поле белым цветом
    painter.fillRect(0,0,width(),height(),QBrush(Qt::white,Qt::SolidPattern));
    //рисуем 4 серых сектора
    int startX = 3*width()/64;
    int startY = height()/16;
    int pieW = 7*width()/8;
    int pieH = 7*height()/8;
    painter.setPen(QPen(Qt::black,2));
    painter.setBrush(QBrush(Qt::lightGray,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH),0*16,90*16);
    painter.drawPie(QRect(startX, startY, pieW, pieH),90*16,90*16);
    painter.drawPie(QRect(startX, startY, pieW, pieH),180*16,90*16);
    painter.drawPie(QRect(startX, startY, pieW, pieH),270*16,90*16);
    //рисуем руки и ноги
    painter.drawPixmap(13*width()/16,0,*p_Rhand);
    painter.drawPixmap(0,0,*p_Lhand);
    painter.drawPixmap(13*width()/16,13*height()/16,*p_Rfoot);
    painter.drawPixmap(0,13*height()/16,*p_Lfoot);
    //рисуем цветные сектора
    int angle = 90*16; //начальный угол
    startX = width()/8-width()/64;
    startY = height()/8;
    pieW = 3*width()/4;
    pieH = 3*height()/4;
    painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::green,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::green,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::green,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::green,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
    painter.drawPie(QRect(startX, startY, pieW, pieH), angle, -360);
    angle -= 360;
    //круг в центре
    painter.setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
    painter.drawEllipse(width()/2-width()/4-width()/64,height()/2-height()/4,width()/2,height()/2);
    painter.end();
    //сделаем белый цвет маской прозрачности
    p_TwisterCircle->setMask(p_TwisterCircle->createMaskFromColor(Qt::white,Qt::MaskInColor));
}


/**
  * Функция задает оптимальную гладкость для заданной скорости анимации стрелки
  *
  */
int QTwisterWidget::calculateSmoothness(int speed) {
    return (abs(speed - 50)*0.01 + 0.4)*29.0;
}

//обработчик события по смене языка
void QTwisterWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        if (m_Timerid) {
            //если таймер жив - один текст
            roundButton->setText(tr("Stop"));
        } else {
            //если мертв - другой
            roundButton->setText(tr("Twist!"));
        }
    } else {
        QWidget::changeEvent(event);
    }
}
