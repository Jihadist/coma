#include "tunekivmain.h"

TuneKIVMain::TuneKIVMain(TuneKIV *kiv, QWidget *parent) : EAbstractTuneDialog(parent)
{
    TKIV = kiv;
    m_tuneStep = 1;
    AddBac(&TKIV->m_Bac_block, 0, sizeof(TKIV->m_Bac_block));
    SetupUI();
}

void TuneKIVMain::SetupUI() { }

void TuneKIVMain::SetLbls() { }

void TuneKIVMain::SetPf() { }

void TuneKIVMain::FillBac(int bacnum) { Q_UNUSED(bacnum) }

void TuneKIVMain::FillBackBac(int bacnum) { Q_UNUSED(bacnum) }

void TuneKIVMain::GetBdAndFill() { }
