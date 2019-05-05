#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QIntValidator>
#include <QTranslator>
#include "qtwistersettingsdialog.h"

QTwisterSettingsDialog::QTwisterSettingsDialog(QWidget *parent) :
        QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    //создаем виджеты
    QLabel * pTimerLabel = new QLabel(tr("&Timer settings (seconds)"));
    //настройка таймера в секундах
    p_TimerEdit = new QLineEdit();
    QIntValidator * intValidator = new QIntValidator(NULL);
    p_TimerEdit->setValidator(intValidator);
    pTimerLabel->setBuddy(p_TimerEdit);

    QLabel * pArrowSpeedLabel = new QLabel(tr("&Arrow animation speed"));
    //создаем слайдер скорости анимации
    p_ArrowSpeedSlider = new QSlider(Qt::Horizontal);
    p_ArrowSpeedSlider->setRange(1,100);
    p_ArrowSpeedSlider->setPageStep(10);
    p_ArrowSpeedSlider->setSingleStep(10);
    p_ArrowSpeedSlider->setValue(50);
    p_ArrowSpeedSlider->setTickInterval(10);
    p_ArrowSpeedSlider->setTickPosition(QSlider::TicksBelow);
    pArrowSpeedLabel->setBuddy(p_ArrowSpeedSlider);

    //настройка включения/выключения звука
    p_SoundOnCheckBox = new QCheckBox(tr("&Sound on"));

    //настройка разрешения/запрещения генерации одинаковых подряд результатов
    p_AllowSameResultsCheckBox = new QCheckBox(tr("Allow same &results in a row"));

    //создаем кнопки Ok и Cancel, соединяем с сигналами
    QPushButton * pOkBtn = new QPushButton(tr("&OK"));
    connect(pOkBtn,SIGNAL(clicked()),SLOT(accept()));
    QPushButton * pCancelBtn = new QPushButton(tr("&Cancel"));
    connect(pCancelBtn,SIGNAL(clicked()),SLOT(reject()));

    //селектор выбора языка
    QLabel * pSetLanguageLabel = new QLabel(tr("&Language"));
    p_LanguageBox = new QComboBox();
    p_LanguageBox->addItem(tr("English"));
    p_LanguageBox->addItem(tr("Russian"));
    pSetLanguageLabel->setBuddy(p_LanguageBox);

    //селектор выбора голоса
    QLabel * pVoiceLabel = new QLabel(tr("&Voice"));
    p_VoiceBox = new QComboBox();
    p_VoiceBox->addItem(tr("Male"));
    p_VoiceBox->addItem(tr("Female"));
    pVoiceLabel->setBuddy(p_VoiceBox);

    //добавляем все в раскладку
    QGridLayout * pLayout = new QGridLayout();
    pLayout->addWidget(pTimerLabel,0,0);
    pLayout->addWidget(p_TimerEdit,1,0);
    pLayout->addWidget(pArrowSpeedLabel,0,1);
    pLayout->addWidget(p_ArrowSpeedSlider,1,1,1,2);
    pLayout->addWidget(p_SoundOnCheckBox,2,0);
    pLayout->addWidget(pSetLanguageLabel,2,1);
    pLayout->addWidget(p_LanguageBox,3,1);
    pLayout->addWidget(p_AllowSameResultsCheckBox,3,0);
    pLayout->addWidget(pVoiceLabel,4,1);
    pLayout->addWidget(p_VoiceBox,5,1);
    pLayout->addWidget(pOkBtn,6,0);
    pLayout->addWidget(pCancelBtn,6,1);

    //устанавливаем раскладку
    setLayout(pLayout);

    //заголовок
    setWindowTitle(tr("Twister Settings"));
}

int QTwisterSettingsDialog::timerSetting() {
    //настройку возвращаем в миллисекундах
    bool trok;
    int ts = p_TimerEdit->text().toInt(&trok);
    return ts*1000;
}

//функция для установки умолчательного значения настройки таймера
void QTwisterSettingsDialog::setTimerSetting(int value) {
    p_TimerEdit->setText(QString::number(value));
}

int QTwisterSettingsDialog::arrowSpeedSetting() {
    return p_ArrowSpeedSlider->value();
}

void QTwisterSettingsDialog::setArrowSpeed(int value) {
    p_ArrowSpeedSlider->setValue(value);
}

bool QTwisterSettingsDialog::soundOn() {
    return p_SoundOnCheckBox->isChecked();
}

void QTwisterSettingsDialog::setSoundOn(bool value) {
    p_SoundOnCheckBox->setChecked(value);
}

bool QTwisterSettingsDialog::allowSameResult() {
    return p_AllowSameResultsCheckBox->isChecked();
}

void QTwisterSettingsDialog::setAllowSameResult(bool value) {
    p_AllowSameResultsCheckBox->setChecked(value);
}

void QTwisterSettingsDialog::setLanguage(QString language) {
    if (language == "ru_RU") {
        p_LanguageBox->setCurrentIndex(1);
    } else {
        p_LanguageBox->setCurrentIndex(0);
    }
}

QString QTwisterSettingsDialog::getLanguage() {
    switch (p_LanguageBox->currentIndex()) {
        case 1: return "ru_RU";
        default: return "en_US";
    }
}

int QTwisterSettingsDialog::getVoiceBox() {
    return p_VoiceBox->currentIndex();
}

void QTwisterSettingsDialog::setVoiceBox(int value) {
    p_VoiceBox->setCurrentIndex(value);
}
