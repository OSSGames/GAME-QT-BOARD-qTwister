#ifndef QTWISTERSETTINGSDIALOG_H
#define QTWISTERSETTINGSDIALOG_H

#include <QtGui>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>

class QTwisterSettingsDialog : public QDialog
{
Q_OBJECT
public:
    QTwisterSettingsDialog(QWidget * parent=0);
    int timerSetting();
    void setTimerSetting(int value);
    int arrowSpeedSetting();
    void setArrowSpeed(int value);
    bool soundOn();
    void setSoundOn(bool value);
    bool allowSameResult();
    void setAllowSameResult(bool value);
    QString getLanguage();
    void setLanguage(QString language);
    int getVoiceBox();
    void setVoiceBox(int value);
private:
    QLineEdit * p_TimerEdit;
    QSlider * p_ArrowSpeedSlider;
    QCheckBox * p_SoundOnCheckBox;
    QCheckBox * p_AllowSameResultsCheckBox;
    QComboBox * p_LanguageBox;
    QComboBox * p_VoiceBox; //0 - мужчина, 1 - женщина
};

#endif // QTWISTERSETTINGSDIALOG_H
