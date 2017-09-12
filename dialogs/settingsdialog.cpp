#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QPushButton>
#include <QString>
#include "settingsdialog.h"
#include "../config/config.h"
#include "../widgets/s_tqspinbox.h"
#include "../widgets/wd_func.h"
#include "../publicclass.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Настройки");
    SetupUI();
    Fill();
}

void SettingsDialog::SetupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QLabel *lbl;
    QLineEdit *le;
    if (pc.ModuleBsi.MTypeB == MTB_A1)
    {
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLT(this, "Наименование организации, эксплуатирующей прибор"));
        hlyout->addWidget(WDFunc::NewLE(this, "orgle"));
        vlyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLT(this, "Наименование средства поверки"));
        hlyout->addWidget(WDFunc::NewLE(this, "povdev"));
        vlyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLT(this, "Заводской номер средства поверки"));
        hlyout->addWidget(WDFunc::NewLE(this, "povdevsn"));
        vlyout->addLayout(hlyout);
        hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBLT(this, "Класс точности средства поверки"));
        hlyout->addWidget(WDFunc::NewLE(this, "povdevprecision"));
        vlyout->addLayout(hlyout);
    }
    hlyout = new QHBoxLayout;
    lbl = new QLabel("Рабочий каталог программы");
    hlyout->addWidget(lbl, 0);
    le = new QLineEdit;
    le->setObjectName("pathle");
    hlyout->addWidget(le, 1);
    QPushButton *pb = new QPushButton("...");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetHomeDir()));
    hlyout->addWidget(pb, 0);
    vlyout->addLayout(hlyout);
    if (pc.ModuleBsi.MTypeB == MTB_80)
    {
        hlyout = new QHBoxLayout;
        lbl = new QLabel("IP-адрес МИП:");
        hlyout->addWidget(lbl);
        le = new QLineEdit;
        le->setObjectName("miple");
        hlyout->addWidget(le);
        vlyout->addLayout(hlyout);
    }
#if PROGSIZE >= PROGSIZE_FULL
    hlyout = new QHBoxLayout;
    hlyout->addWidget(WDFunc::NewChB(this, "writelogchb", "Запись обмена данными в файл"));
    vlyout->addLayout(hlyout);
#endif
    pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked()),this,SLOT(AcceptSettings()));
    vlyout->addWidget(pb);
    setLayout(vlyout);
}

void SettingsDialog::Fill()
{
    if (PROGNAME == "PKDN-S")
        WDFunc::SetLEData(this,"orgle",pc.OrganizationString);
    WDFunc::SetLEData(this,"pathle",pc.HomeDir);
    WDFunc::SetLEData(this,"povdev",pc.PovDev.DevName);
    WDFunc::SetLEData(this,"povdevsn",pc.PovDev.DevSN);
    WDFunc::SetLEData(this,"povdevprecision",pc.PovDev.DevPrecision);
    if (pc.ModuleBsi.MTypeB == MTB_80)
    {
        QString restring = "^[0-2]{0,1}[0-9]{1,2}{\\.[0-2]{0,1}[0-9]{1,2}}{3}$";
        WDFunc::SetLEData(this,"miple",pc.MIPIP,restring);
    }
    WDFunc::SetChBData(this, "writelogchb", pc.WriteUSBLog);
}

void SettingsDialog::AcceptSettings()
{
    if (PROGNAME == "PKDN-S")
        WDFunc::LEData(this, "orgle", pc.OrganizationString);
    WDFunc::LEData(this, "pathle", pc.HomeDir);
    WDFunc::LEData(this, "povdev", pc.PovDev.DevName);
    WDFunc::LEData(this, "povdevsn", pc.PovDev.DevSN);
    WDFunc::LEData(this, "povdevprecision", pc.PovDev.DevPrecision);
    if (pc.ModuleBsi.MTypeB == MTB_80)
        WDFunc::LEData(this, "miple", pc.MIPIP);
    WDFunc::ChBData(this, "writelogchb", pc.WriteUSBLog);
    this->close();
}

void SettingsDialog::SetHomeDir()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString dir = dlg->getExistingDirectory(this, "Домашний каталог", pc.HomeDir);
    if (!dir.isEmpty())
        WDFunc::SetLEData(this,"pathle",dir);
}
