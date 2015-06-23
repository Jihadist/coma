#ifndef E_CHECKDIALOG_H
#define E_CHECKDIALOG_H

#include <QDialog>
#include <QTimer>
//#include "canal.h"

class e_checkdialog : public QDialog
{
    Q_OBJECT
public:
    explicit e_checkdialog(QWidget *parent = 0);

signals:

public slots:

private:
//    canal *cn;
    struct Bd
    {
        float ain[16];
        float in[16];
        float at;
        float ainn[16];
        float ainx[16];
    };

    struct Bda
    {
        quint32 sin[16];
    };

    struct Bip
    {
        quint8 ip[4];
    };

    Bd Bd_block;
    Bda Bda_block;
    Bip Bip_block;
    bool BdMeasurementsActive, BdaMeasurementsActive, OddTimeout;
    QTimer *timer;

//    void SetupUI();

private slots:
/*    void StartMeasurements();
    void StartBdaMeasurements();
    void StopMeasurements();
    void StopBdaMeasurements();
    void SetTimerPeriod(int);
    void CheckLEDOn();
    void CheckLEDOff();
    void GetIP();
    void CheckIP();
    void Check1PPS();
    void TimerTimeout();
    void RefreshBd();
    void RefreshBda(); */
};

#endif // E_CHECKDIALOG_H
