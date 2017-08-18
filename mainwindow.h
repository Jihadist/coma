#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QMap>
#include "publicclass.h"
#include "../config/confdialog.h"

#define C_TE_MAXSIZE    10000
#define MAXERRORFLAGNUM 32

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = 0);

    bool SWHide;
    QRect SWGeometry;
    quint32 PrbSize;
    QVector<publicclass::DataRec> S2Config;
    AbstractConfDialog *ConfB, *ConfM;

    static QStringList Hth()
    {
        QStringList sl;
        sl.append("!OK");
        sl.append("FLS");
        sl.append("TUP");
        sl.append("REL");
        sl.append("1PPS");
        sl.append("ADC");
        sl.append("REG");
        sl.append("CNF");
        sl.append("LS");
        sl.append("FNC");
        sl.append("FW");
        sl.append("BAT");
        int ts = sl.size();
        for (int i=ts; i<MAXERRORFLAGNUM; ++i)
            sl.append("");
        return sl;
    }

    static QStringList HthToolTip()
    {
        QStringList sl;
        sl.append("Что-то не в порядке");
        sl.append("Ошибка внешней FLASH-памяти");
        sl.append("Перегрев модуля");
        sl.append("Неисправность выходных реле");
        sl.append("Нет сигнала 1PPS с антенны");
        sl.append("Нет связи с АЦП");
        sl.append("Нет настроечных параметров в памяти модуля");
        sl.append("Ошибка конфигурации в памяти модуля, взята конфигурация по умолчанию");
        sl.append("Сигналы слишком малы");
        sl.append("Частота находится вне диапазона");
        sl.append("Ошибка файла фирменного ПО");
        sl.append("Требуется замена батарейки");
        int ts = sl.size();
        for (int i=ts; i<MAXERRORFLAGNUM; ++i)
            sl.append("");
        return sl;
    }

    QMap<int, QString> ModuleNames()
    {
        QMap<int, QString> map;
        map[MTB_A1] = "ПКДН";
        map[MTB_80] = "АВТУК";
        return map;
    }

    virtual void SetupUI() = 0;
    virtual void Stage3() = 0;
    void ClearTW();
    QWidget *MainInfoWidget(QWidget *parent);
    QWidget *HthWidget();
    QWidget *Least();

signals:
    void CloseConnectDialog();
    void Retry();
    void FinishAll();

private:
    publicclass::Bsi Bsi_block;

    void SetSlideWidget();
    void SetupMenubar();
    void PrepareTimers();
    void LoadSettings();
    void SaveSettings();
    void FillBsi();
    void ClearBsi();
    void ShowOrHideSlideSW();


private slots:
    void Stage1();
    void Stage1_5();
    void Stage2();
    void SetDefConf();
    void Fill();
    void UpdateMainTE(QByteArray &ba);
    void SetPort(QString str);
    void OpenBhbDialog();
    void StartEmul();
    void StartSettingsDialog();
    void ShowErrorDialog();
    void SetProgressBarSize(quint32 size);
    void SetProgressBar(quint32 cursize);
    void DisableProgressBar();
    void GetAbout();
    void Disconnect();
    void MouseMove();

protected:
    void resizeEvent(QResizeEvent *e);
};

#endif // MAINWINDOW_H
