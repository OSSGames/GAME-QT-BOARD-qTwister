#include "mainwindow.h"
#include <QHBoxLayout>
#include <QMenu>
#include <QSound>
#include "qtwistersettingsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //зададим иконку приложения
    this->setWindowIcon(QIcon("icons/qtwister_logo_256.png"));
    //создадим твистер и добавим его в горизонтальную раскладку
    twister = new QTwisterWidget();
    QHBoxLayout * layout = new QHBoxLayout();
    layout->addWidget(twister);

    //создадим настройки
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "qTwister", "qTwister", this);
    loadSettings();

    //грузим нужный язык
    loadLanguage();

    //добавляем меню
    playMenu = this->menuBar()->addMenu(tr("Play"));
    startTimerAction = playMenu->addAction(tr("Play"),this,SLOT(startTimerPlay()));
    stopTimerAction = playMenu->addAction(tr("Stop"),this,SLOT(stopTimerPlay()));
    settingsAction = playMenu->addAction(tr("Settings"),this,SLOT(showSettingsDialog()));
    playMenu->addSeparator();
    playMenu->addAction(tr("Quit"),qApp,SLOT(quit()));
    stopTimerAction->setEnabled(false);

    //создадим обычный виджет, чтобы сделать его центральным в MainWindow
    QWidget * pwdg = new QWidget();
    this->setCentralWidget(pwdg);
    this->centralWidget()->setLayout(layout);

    //подключаем звук для кнопки
    connect(twister->getRoundButtonPtr(), SIGNAL(pressed()), this, SLOT(buttonSound()));

    //подключаем звук для твистера
    connect(twister, SIGNAL(valueChanged(TTwisterValue)), this, SLOT(twisterSound(TTwisterValue)));

    //подключаем сигнал, что таймер выключен
    connect(twister, SIGNAL(timerDead()), this, SLOT(onTimerDeath()));

    //загрузим настройки окна
    settings->beginGroup("mainwindow");
    if (settings->contains("size")) {
        resize(settings->value("size").toSize());
    }
    if (settings->contains("position")) {
        move(settings->value("position").toPoint());
    }
    settings->endGroup();
}

MainWindow::~MainWindow()
{
    //сохраним настройки окна
    settings->beginGroup("mainwindow");
    settings->setValue("size",size());
    settings->setValue("position",pos());
    settings->endGroup();
}

//слот для начала игры по таймеру
void MainWindow::startTimerPlay() {
    startTimerAction->setEnabled(false);
    stopTimerAction->setEnabled(true);
    twister->startTimerSlot();
}

//слот для конца игры по таймеру
void MainWindow::stopTimerPlay() {
    twister->killTimerSlot();
}

//слот, включающийся при убийстве таймера
void MainWindow::onTimerDeath() {
    stopTimerAction->setEnabled(false);
    startTimerAction->setEnabled(true);
}

//слот для показа диалога настроек
void MainWindow::showSettingsDialog() {
    QTwisterSettingsDialog * twistersettings = new QTwisterSettingsDialog(this);
    //устанавливаем текущую настройку таймера (переведя в секунды)
    twistersettings->setTimerSetting(twister->getTimerValue()/1000);
    //устанавливаем настройку скорости анимации
    twistersettings->setArrowSpeed(twister->getAnimationSpeed());
    //устанавливаем настройку звука
    twistersettings->setSoundOn(soundOn);
    //устанавливаем разрешение/запрет генерации одинаковых результатов
    twistersettings->setAllowSameResult(twister->getAllowSameResults());
    //устанавливаем язык
    twistersettings->setLanguage(m_Language);
    //устанавливаем голос
    twistersettings->setVoiceBox(m_Voice);
    if (twistersettings->exec() == QDialog::Accepted) {
        //выставляем новое время таймера
        twister->setTimerValue(twistersettings->timerSetting());
        //выставляем скорость анимации
        twister->setAnimationSpeed(twistersettings->arrowSpeedSetting());
        //выставляем звук
        soundOn = twistersettings->soundOn();
        //выставляем разрешение/запрет одинаковых результатов
        twister->setAllowSameResults(twistersettings->allowSameResult());
        //выставляем язык и перегружаем его, если надо
        QString new_lang = twistersettings->getLanguage();
        if (new_lang != m_Language) {
            m_Language = new_lang;
            loadLanguage();
        }
        //определяем голос
        m_Voice = twistersettings->getVoiceBox();
        //выставляем эти настройки в settings
        saveSettings();
    }
    delete twistersettings;
}

//слот для загрузки настоек
void MainWindow::loadSettings() {
    //функция загружает настройки, если ключи существуют
    //и устанавливает дефолтные - если нет
    settings->beginGroup("twister");
    if (settings->contains("timer")) {
        twister->setTimerValue(settings->value("timer").toInt());
    } else {
        twister->setTimerValue(60000); //60 seconds
        settings->setValue("timer",60000);
    }
    if (settings->contains("arrow_speed")) {
        twister->setAnimationSpeed(settings->value("arrow_speed").toInt());
    } else {
        twister->setAnimationSpeed(50);
        settings->setValue("arrow_speed",50);
    }
    if (settings->contains("sound_on")) {
        this->soundOn = settings->value("sound_on").toBool();
    } else {
        this->soundOn = true;
        settings->setValue("sound_on",true);
    }
    if (settings->contains("allow_same_in_row")) {
        twister->setAllowSameResults(settings->value("allow_same_in_row").toBool());
    } else {
        twister->setAllowSameResults(true);
        settings->setValue("allow_same_in_row",true);
    }
    if (settings->contains("language")) {
        m_Language = settings->value("language").toString();
    } else {
        m_Language = QLocale::system().name();
    }
    if (settings->contains("voice")) {
        m_Voice = settings->value("voice").toInt();
    } else {
        m_Voice = 0;
    }
    settings->endGroup();
}

//слот для сохранения настроек
void MainWindow::saveSettings() {
    settings->beginGroup("twister");
    settings->setValue("timer", twister->getTimerValue());
    settings->setValue("arrow_speed", twister->getAnimationSpeed());
    settings->setValue("sound_on", soundOn);
    settings->setValue("allow_same_in_row",twister->getAllowSameResults());
    settings->setValue("language", m_Language);
    settings->setValue("voice", m_Voice);
    settings->endGroup();
}

//слот для озвучивания кнопки
//работает по-разному для разных ОС
void MainWindow::buttonSound() {
    if (soundOn) {
#ifdef Q_WS_X11
        QProcess::startDetached("play sounds/btn.wav");
#else
        QSound::play("sounds/btn.wav");
#endif
    }
}

//слот озвучивания нового значения твистера
//работает по-разному для разных ОС
void MainWindow::twisterSound(TTwisterValue value) {
    QString filename = "sounds/";
    //язык звуков
    if (m_Language == "ru_RU") {
        filename += "ru/";
    } else {
        filename += "en/";
    }
    //голос
    if (m_Voice) {
        filename += "female/";
    } else {
        filename += "male/";
    }
    //варианты
    switch (value.Part) {
    case tpLeftFoot :
        filename += "lf";
        break;
    case tpRightFoot :
        filename += "rf";
        break;
    case tpLeftHand :
        filename += "lh";
        break;
    case tpRightHand :
        filename += "rh";
        break;
    }
    switch (value.Color) {
    case tcBlue :
        filename += "b.wav";
        break;
    case tcGreen :
        filename += "g.wav";
        break;
    case tcYellow :
        filename += "y.wav";
        break;
    case tcRed :
        filename += "r.wav";
        break;
    }
    //проигрываем
    if (soundOn) {
#ifdef Q_WS_X11
        QProcess::startDetached("play " + filename);
#else
        QSound::play(filename);
#endif
    }
}

void MainWindow::loadLanguage() {
    qApp->removeTranslator(&m_Translator);
    if (m_Language == "ru_RU") {
      //для русского загружаем перевод
      m_Translator.load("ru.qm",".");
      qApp->installTranslator(&m_Translator);
    }
}

//обработчик события по смене языка
void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        playMenu->setTitle(tr("Play"));
        startTimerAction->setText(tr("Play"));
        stopTimerAction->setText(tr("Stop"));
        settingsAction->setText(tr("Settings"));
    } else {
        QWidget::changeEvent(event);
    }
}
