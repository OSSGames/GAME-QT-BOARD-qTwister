#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "qtwisterwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void startTimerPlay();
    void stopTimerPlay();
    void onTimerDeath();
    void showSettingsDialog();
    void buttonSound();
    void twisterSound(TTwisterValue value);

protected:
    QAction * startTimerAction;
    QAction * stopTimerAction;
    QAction * settingsAction;
    QMenu * playMenu;
    QTwisterWidget * twister;
    QSettings * settings;

    bool soundOn;

    QString m_Language;
    int m_Voice;
    QTranslator m_Translator;

    void loadSettings();
    void saveSettings();
    void loadLanguage();

    virtual void changeEvent(QEvent *event);
};

#endif // MAINWINDOW_H
