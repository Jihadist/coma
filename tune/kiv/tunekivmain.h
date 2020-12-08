#ifndef TUNEKIVMAIN_H
#define TUNEKIVMAIN_H

#include "../../config/configkiv.h"
#include "../abstracttunedialog.h"
#include "tunekiv.h"

#include <QWidget>

class TuneKIVMain : public AbstractTuneDialog
{
    Q_OBJECT
public:
    TuneKIVMain(int tuneStep, ConfigKIV *ckiv, TuneKIV *kiv, QWidget *parent = nullptr);

private:
    TuneKIV *TKIV;
    ConfigKIV *CKIV;
    double m_pt100;
    bool m_isEnergoMonitorDialogCreated;
    int m_curTuneStep;
    TuneKIV::Bda_in m_bdain;
    TuneKIV::Bac m_bac;

    void setMessages();
    void setTuneFunctions();
    void FillBac(int bacnum);
    void FillBackBac(int bacnum);
    QWidget *MainUI();

    Error::Msg showPreWarning();
    Error::Msg checkTuneCoefs();
    Error::Msg setR80();
    Error::Msg processR80();
    Error::Msg setR120();
    Error::Msg processR120();
    Error::Msg ADCCoef1();
    Error::Msg ADCCoef2();
    Error::Msg ADCCoef4();
    Error::Msg ADCCoef8();
    Error::Msg ADCCoef16();
    Error::Msg ADCCoef32();
    Error::Msg Tmk0();
    Error::Msg SendBac();
    Error::Msg CheckTune();

    void setR(int r);
    double processR(); // get BDA.Pt100 several times and return back value
    Error::Msg ADCCoef(int coef);
    Error::Msg setADCCoef(int coef);
    Error::Msg showRetomDialog(int coef);
    void showTWTab(int num);
    QWidget *BdWidget();
    bool checkBdaIn();
    Error::Msg showEnergomonitorInputDialog();
    void saveIntermediateResults();

private slots:
    void CalcTuneCoefs();
};

#endif // TUNEKIVMAIN_H
