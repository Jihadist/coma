/* A1Dialog - dialog providing a main function of the device
 *
*/
#include <QtMath>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QGroupBox>
#include <QRadioButton>
#include <QStringListModel>
#include <QFileDialog>
#include <QPushButton>
#include <QTableView>
#include <QTime>
//#include "../canal.h"
#include "a1dialog.h"
#include "../widgets/messagebox.h"
#include "../widgets/waitwidget.h"
#include "../widgets/wd_func.h"
#include "../commands.h"

A1Dialog::A1Dialog(QWidget *parent) : QDialog(parent)
{
    ChA1 = new CheckA1;
    CA1 = new ConfigA1(S2Config);
    ReportModel = new QStandardItemModel;
    SetupUI();
    MeasurementTimer = new QTimer;
    connect(MeasurementTimer,SIGNAL(timeout()),this,SLOT(MeasTimerTimeout()));
//    ReportHeader.DNDevices = "УКДН сер. номер " + QString::number(pc.ModuleBsi.SerialNum) + ", кл. точн. 0,05";
}

void A1Dialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QString tmps = "QDialog {background-color: "+QString(UCONFCLR)+";}";
    setStyleSheet(tmps);
    QString ValuesFormat = "QLabel {border: 1px solid green; border-radius: 4px; padding: 1px; color: black;"\
            "background-color: "+QString(ACONFOCLR)+"; font: bold 10px;}";
    QGridLayout *glyout = new QGridLayout;
    QGroupBox *gb = new QGroupBox("Измерения в первичном масштабе");
    glyout->addWidget(WDFunc::NewLBL(this, "U1"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu1", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "U2"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednu2", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "δU, %"), 0, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunepercent", ValuesFormat, ""), 0, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Phy"), 1, 4, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednphy", ValuesFormat, ""), 1, 5, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "Freq"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunednfreq", ValuesFormat, ""), 1, 1, 1, 1);
    QPushButton *pb = new QPushButton("Подтвердить");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Accept()));
    QHBoxLayout *hlyout = new QHBoxLayout;
    pb->setObjectName("acceptpb");
    pb->setEnabled(false);
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Decline()));
    pb->setObjectName("cancelpb");
    pb->setEnabled(false);
    hlyout->addWidget(pb);
    glyout->addLayout(hlyout, 2, 0, 1, 6);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    glyout->setColumnStretch(5, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    glyout = new QGridLayout;
    gb = new QGroupBox("СКО");
    glyout->addWidget(WDFunc::NewLBL(this, "dUrms(m), В"), 0, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunedurmsm", ValuesFormat, ""), 0, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "φ(m), срад"), 0, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunephym", ValuesFormat, ""), 0, 3, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "σUrms, %"), 1, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunesurms", ValuesFormat, ""), 1, 1, 1, 1);
    glyout->addWidget(WDFunc::NewLBL(this, "σφ, срад"), 1, 2, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLBLT(this, "", "tunesphy", ValuesFormat, ""), 1, 3, 1, 1);
    glyout->setColumnStretch(1, 10);
    glyout->setColumnStretch(3, 10);
    gb->setLayout(glyout);
    lyout->addWidget(gb);

    pb = new QPushButton("Сформировать протокол из файла ПКДН");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(ParsePKDNFile()));
    lyout->addWidget(pb);
    pb = new QPushButton("Начать поверку делителя");
    pb->setObjectName("StartWorkPb");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(StartWork()));
    lyout->addWidget(pb);
    lyout->addStretch(10);
    setLayout(lyout);
}

int A1Dialog::GetConf()
{
//    cn->Send(CN_GF, BT_NONE,NULL,0,1,&S2Config); // заполнение CA1->Bci_block
//    if (cn->result == NOERROR)
    if (CM_GetFile(1, &S2Config))
    {
        if (CM_GetBac(&Bac_block, sizeof(Bac), BT_MEZONIN) == NOERROR)
/*        cn->Send(CN_GBac, BT_MEZONIN, &Bac_block, sizeof(Bac));
        if (cn->result == NOERROR) */
        {
            Bac_block.U1kDN[0] = 0;
            Bac_block.U2kDN[0] = 0;
            Bac_block.PhyDN[0] = 0;
            return NOERROR;
        }
    }
    return GENERALERROR;
}

void A1Dialog::FillBdOut()
{
    WDFunc::SetLBLText(this, "tunednu1", QString::number(ChA1->Bda_out.Uef_filt[0], 'f', 5));
    WDFunc::SetLBLText(this, "tunednu2", QString::number(ChA1->Bda_out.Uef_filt[1], 'f', 5));
    WDFunc::SetLBLText(this, "tunednphy", QString::number((-ChA1->Bda_out.Phy), 'f', 5));
    WDFunc::SetLBLText(this, "tunednfreq", QString::number(ChA1->Bda_out.Frequency, 'f', 5));
    WDFunc::SetLBLText(this, "tunepercent", QString::number((-ChA1->Bda_out.dUrms), 'f', 5));
}

void A1Dialog::FillMedian()
{
    WDFunc::SetLBLText(this, "tunedurmsm", QString::number(Dd_Block.dUrms, 'f', 5));
    WDFunc::SetLBLText(this, "tunephym", QString::number(Dd_Block.Phy, 'f', 5));
    WDFunc::SetLBLText(this, "tunesurms", QString::number(Dd_Block.sU, 'f', 5));
    WDFunc::SetLBLText(this, "tunesphy", QString::number(Dd_Block.sPhy, 'f', 5));
}

void A1Dialog::GenerateReport()
{
    try
    {
        // запрос блока Bda_h, чтобы выдать KNI в протокол
        if (CM_GetBd(A1_BDA_H_BN, &ChA1->Bda_h, sizeof(CheckA1::A1_Bd3)) != NOERROR)
/*        cn->Send(CN_GBd, A1_BDA_H_BN, &ChA1->Bda_h, sizeof(CheckA1::A1_Bd3));
        if (cn->result != NOERROR) */
            report->dataManager()->setReportVariable("KNI", ChA1->Bda_h.HarmBuf[0][0]);
        ResultsStruct Results;
        Results.Frequency = Results.Humidity = Results.Temp = Results.THD = 0;
        Results.GOST = Results.SerNum = Results.Time = 0;
        ConditionDataDialog(Results); // задаём условия поверки
        DNDialog(pc.PovDev, Results); // вводим данные по делителю
        // данные в таблицу уже получены или из файла, или в процессе работы
        // отобразим таблицу
        ShowTable();
        QString GOST = (PovType == GOST_1983) ? "1983" : "23625";
        report = new LimeReport::ReportEngine(this);
        report->loadFromFile(pc.SystemHomeDir+"a1_"+GOST+".lrxml");
        report->dataManager()->addModel("maindata", ReportModel, false);
        report->dataManager()->setReportVariable("Organization", pc.OrganizationString);
        QString day = QDateTime::currentDateTime().toString("dd");
        QString month = QDateTime::currentDateTime().toString("MM");
        QString yr = QDateTime::currentDateTime().toString("yy");
        report->dataManager()->setReportVariable("Day", day);
        report->dataManager()->setReportVariable("Month", month);
        report->dataManager()->setReportVariable("Yr", yr);
        report->dataManager()->setReportVariable("DNType", ReportHeader.DNType);
        report->dataManager()->setReportVariable("DNSerNum", ReportHeader.DNSerNum);
        report->dataManager()->setReportVariable("DNTol", ReportHeader.DNTol);
        report->dataManager()->setReportVariable("DNU1", ReportHeader.DNU1);
        report->dataManager()->setReportVariable("DNU2", ReportHeader.DNU2);
        report->dataManager()->setReportVariable("DNP", ReportHeader.DNP);
        report->dataManager()->setReportVariable("DNF", ReportHeader.DNF);
        report->dataManager()->setReportVariable("DNOrganization", ReportHeader.DNOrganization);
        report->dataManager()->setReportVariable("DNPlace", ReportHeader.DNPlace);
        report->dataManager()->setReportVariable("DNDevices", ReportHeader.DNDevices);
        report->dataManager()->setReportVariable("Temp", ReportHeader.Temp);
        report->dataManager()->setReportVariable("Humidity", ReportHeader.Humidity);
        report->dataManager()->setReportVariable("Pressure", ReportHeader.Pressure);
        report->dataManager()->setReportVariable("Voltage", ReportHeader.Voltage);
        report->dataManager()->setReportVariable("Freq", ReportHeader.Freq);
        report->dataManager()->setReportVariable("OuterInsp", ReportHeader.OuterInsp);
        report->dataManager()->setReportVariable("WindingsInsp", ReportHeader.WindingsInsp);
        QFileDialog *dlg = new QFileDialog;
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        QString Filename = dlg->getSaveFileName(this, "", pc.HomeDir, "*.pdf");
        dlg->close();
        report->printToPDF(Filename);
    //    report->previewReport();
    //    report->designReport();
    }
    catch(...)
    {
        ERMSG("Report exception!");
    }
}

void A1Dialog::ConditionDataDialog(ResultsStruct &Results)
{
    int row = 0;
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Условия поверки"), Qt::AlignCenter);
    if ((CA1->Bci_block.DTCanal == 0)  || (ChA1->Bda_out_an.Tamb == FLT_MAX) || (Results.Temp != 0))
    {
        glyout->addWidget(WDFunc::NewLBL(this, "Температура окружающей среды, °С"), row, 0, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLEF(this, "Temp", Results.Temp), row++, 1, 1, 1, Qt::AlignLeft);
    }
    if ((CA1->Bci_block.DHCanal == 0) || (ChA1->Bda_out_an.Hamb == FLT_MAX) || (Results.Humidity != 0))
    {
        glyout->addWidget(WDFunc::NewLBL(this, "Влажность воздуха, %"), row, 0, 1, 1, Qt::AlignRight);
        glyout->addWidget(WDFunc::NewLEF(this, "Humidity", Results.Humidity), row++, 1, 1, 1, Qt::AlignLeft);
    }
    glyout->addWidget(WDFunc::NewLBL(this, "Атмосферное давление, кПа"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "Pressure", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Напряжение питания сети, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "Voltage", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Частота питания сети, Гц"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "Frequency", Results.Frequency), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetConditionData()));
    connect(this,SIGNAL(CloseDialog()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void A1Dialog::DNDialog(publicclass::PovDevStruct &PovDev, ResultsStruct &Results)
{
    int row = 0;
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Данные ТН(ДН)"), Qt::AlignCenter);
    glyout->addWidget(WDFunc::NewLBL(this, "Организация, проводившая поверку"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "UKDNOrganization", pc.OrganizationString), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Тип ТН(ДН)"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNType", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Обозначение по схеме, фаза"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNNamePhase", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Заводской номер"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNSerialNum", QString::number(Results.SerNum)), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Класс точности, %"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNTolerance", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное первичное напряжение, кВ"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNU1", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальное вторичное напряжение, В"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNU2", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная мощность нагрузки, ВА"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNP", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Номинальная частота, Гц"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNFreq", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Предприятие-изготовитель"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNOrganization", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Место установки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNPlace", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Наименование средства поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "PovDev", PovDev.DevName), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Заводской номер средства поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "PovDevSN", PovDev.DevSN), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Класс точности средства поверки"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "PovDevPrecision", PovDev.DevPrecision), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Результаты внешнего осмотра"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNInspection", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->addWidget(WDFunc::NewLBL(this, "Результаты проверки правильности обозначения\nвыводов и групп соединений обмоток"), row, 0, 1, 1, Qt::AlignRight);
    glyout->addWidget(WDFunc::NewLEF(this, "DNWindingInspection", ""), row++, 1, 1, 1, Qt::AlignLeft);
    glyout->setColumnStretch(1, 1);
    lyout->addLayout(glyout);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SetDNData()));
    connect(this,SIGNAL(CloseDialog()),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void A1Dialog::UpdateItemInModel(int row, int column, QVariant value)
{
    QStandardItem *item = ReportModel->item(row, column);
    item->setText(value.toString());
    ReportModel->setItem(row, column, item);
}

void A1Dialog::ShowTable()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTableView *tw = new QTableView;
    tw->setModel(ReportModel);
    lyout->addWidget(tw);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),dlg,SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
}

void A1Dialog::StartWork()
{
    float VoltageInkV, VoltageInV;
    pc.Cancelled = false;
    if (GetConf() != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Ошибка чтения конфигурации или настроечных параметров из модуля");
        return;
    }
    WDFunc::SetEnabled(this, "StartWorkPb", false);
    PovType = TempPovType = GOST_NONE;
    QDialog *dlg = new QDialog(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this, "Выберите тип поверяемого оборудования"));
    QRadioButton *rb = new QRadioButton("Трансформаторы напряжения измерительные лабораторные по ГОСТ 23625-2001");
    rb->setObjectName("rb1");
    connect(rb,SIGNAL(toggled(bool)),this,SLOT(RBToggled()));
    lyout->addWidget(rb);
    rb = new QRadioButton("Трансформаторы напряжения по ГОСТ 1983-2001");
    rb->setObjectName("rb2");
    connect(rb,SIGNAL(toggled(bool)),this,SLOT(RBToggled()));
    lyout->addWidget(rb);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Proceed()));
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(Cancel()));
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    dlg->setLayout(lyout);
    dlg->show();
    while ((PovType == GOST_NONE) && !pc.Cancelled)
    {
        QTime tme;
        tme.start();
        while (tme.elapsed() < SLEEPINT)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    dlg->close();
    RowCount = (PovType == GOST_1983) ? GOST1983ROWCOUNT : GOST23625ROWCOUNT;
    ColumnCount = (PovType == GOST_1983) ? GOST1983COLCOUNT : GOST23625COLCOUNT;
    ReportModel->setColumnCount(ColumnCount);
    ReportModel->setRowCount(RowCount);
    for (int i=0; i<RowCount; ++i)
    {
        for (int j=0; j<ColumnCount; ++j)
        {
            QStandardItem *item = new QStandardItem("");
            ReportModel->setItem(i, j, item);
        }
    }
    if (!pc.Cancelled)
    {
        if (MessageBox2::question(this, "Подтверждение", "Подключите вывод нижнего плеча \"своего\" делителя напряжения ко входу U1 прибора\n"
                                  "Вывод нижнего плеча поверяемого делителя или выход низшего напряжения поверяемого ТН - ко входу U2\n"
                                  "На нагрузочном устройстве поверяемого ТН установите значение мощности, равное 0,25·Sном") == true)
        {
            CurrentS = 0.25;
            if (PovType == GOST_1983)
            {
                VoltageInkV = static_cast<float>(Bac_block.K_DN) * 80 / 1732;
                VoltageInV = static_cast<float>(1000 * 80) / 1732;
            }
            else
            {
                VoltageInkV = static_cast<float>(Bac_block.K_DN) * 20 / 1732;
                VoltageInV = static_cast<float>(1000 * 20) / 1732;
            }
            if (MessageBox2::question(this, "Подтверждение", "Подайте на делители напряжение " + \
                                      QString::number(VoltageInkV, 'f', 1) + " кВ ("+QString::number(VoltageInV, 'f', 1)+" В)") == true)
            {
                Index = 0;
                Counter = 0;
                MeasurementTimer->start();
                WDFunc::SetEnabled(this, "cancelpb", true);
                WDFunc::SetEnabled(this, "acceptpb", true);
                return;
            }
        }
    }
    WDFunc::SetEnabled(this, "StartWorkPb", true);
    MessageBox2::information(this, "Информация", "Операция прервана");
    return;
}

void A1Dialog::ParsePKDNFile()
{
    QByteArray ba;
    publicclass::PovDevStruct PovDev;
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getOpenFileName(parent, "Открыть файл", pc.HomeDir, "PKDN verification files (*.vrf)", Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    dlg->close();
    int res = LoadFromFile(filename, ba);
    if (res != NOERROR)
    {
        MessageBox2::error(this, "Ошибка", "Ошибка загрузки файла");
        return;
    }
    // заполняем ReportHeader
    QStringList sl = filename.split("-");
    if (!sl.isEmpty())
    {
        filename = sl.at(0);
        if (filename.size() < 5)
        {
            MessageBox2::error(this, "Ошибка", "Ошибка в имени файла");
            return;
        }
        PovDev.DevSN = filename.right(filename.size()-4);
    }
    else
    {
        MessageBox2::error(this, "Ошибка", "Ошибка в имени файла");
        return;
    }
    PovDev.DevName = pc.PovDev.DevName;
    PovDev.DevPrecision = pc.PovDev.DevPrecision;
    if (ba.size() >= sizeof(ResultsStruct))
    {
        ResultsStruct Results;
        memcpy(&Results, &ba.data()[0], sizeof(ResultsStruct));
        MainDataStruct MDS;
        int MDSSize;
        if (Results.GOST == 0) // GOST 1983
            MDSSize = 6;
        else if (Results.GOST == 1) // GOST 23625
            MDSSize = 18;
        else
        {
            MessageBox2::error(this, "Ошибка", "Ошибочный тип трансформатора в файле");
            return;
        }
        // разберём время

        ChA1->Bda_h.HarmBuf[0][0] = Results.THD;

        RowCount = (Results.GOST == 0) ? GOST1983ROWCOUNT : GOST23625ROWCOUNT;
        ColumnCount = (Results.GOST == 0) ? GOST1983COLCOUNT : GOST23625COLCOUNT;
        ReportModel->setColumnCount(ColumnCount);
        ReportModel->setRowCount(RowCount);
        for (int i=0; i<RowCount; ++i)
        {
            for (int j=0; j<ColumnCount; ++j)
            {
                QStandardItem *item = new QStandardItem("");
                ReportModel->setItem(i, j, item);
            }
        }
        int memptr, MDSs;
        memptr = MDSs = sizeof(MainDataStruct);
        for (int i=0; i<MDSSize; ++i)
        {
            if (memptr >= (ba.size() - MDSs))
            {
                MessageBox2::error(this, "Ошибка", "Неожиданный конец файла");
                return;
            }
            memcpy(&MDS, &ba.data()[memptr], MDSs);
            // заполним модель

            memptr += MDSs;
        }
        DNDialog(PovDev, Results);
        ConditionDataDialog(Results);
        GenerateReport();
    }
    else
    {
        MessageBox2::error(this, "Ошибка", "Неожиданный конец файла");
        return;
    }
}

void A1Dialog::MeasTimerTimeout()
{
/*    cn->Send(CN_GBd, A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1));
    if (cn->result == NOERROR) */
    if (CM_GetBd(A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1)) == NOERROR)
        FillBdOut();
}

void A1Dialog::Accept()
{
    float VoltageInkV, VoltageInV;
    int endcounter = (PovType == GOST_1983) ? 3 : 9;
    MeasurementTimer->stop();
    int Pindex = (Index > 4) ? (8 - Index) : Index;
    const int Percents23625[] = {20, 50, 80, 100, 120};
    const int Percents1983[] = {80, 100, 120};
    const int *Percents = (PovType == GOST_1983) ? Percents1983 : Percents23625;

    // заполняем модель по полученным измерениям:
    // 0 - U/Un (%), 1 - S, 2 -
    int row = Pindex;
    if (CurrentS == 1)
    {
        if (PovType == GOST_1983)
            row += GOST1983ROWCOUNT/2;
        else
            row += GOST23625ROWCOUNT/2;
    }
    if (GetStatistics() == GENERALERROR) // набираем статистику измерений и вычисляем средние значения
    {
        pc.Cancelled = true;
        Decline();
        return;
    }
    if (Index > 4) // на нисходящем отрезке по ГОСТ 23625
    {
        UpdateItemInModel(row, 4, QString::number(ChA1->Bda_out.dUrms,'f',5));
        UpdateItemInModel(row, 5, QString::number(ChA1->Bda_out.Phy,'f',5));
        float dUrmsU = ReportModel->item(row, 2)->text().toFloat();
        float PhyU = ReportModel->item(row, 3)->text().toFloat();
        float UrmsM = (dUrmsU + ChA1->Bda_out.dUrms) / 2;
        float PhyM = (PhyU + ChA1->Bda_out.Phy) / 2;
        UpdateItemInModel(row, 6, QString::number(UrmsM,'f',5));
        UpdateItemInModel(row, 7, QString::number(PhyM,'f',5));
        UpdateItemInModel(row, 8, QString::number((ChA1->Bda_out.dUrms - dUrmsU),'f',5));
        UpdateItemInModel(row, 9, QString::number((ChA1->Bda_out.Phy - PhyU),'f',5));
        UpdateItemInModel(row, 10, QString::number(Dd_Block.dUrms,'f',5));
        UpdateItemInModel(row, 11, QString::number(Dd_Block.Phy,'f',5));
        UpdateItemInModel(row, 12, QString::number(Dd_Block.sPhy,'f',5));
        UpdateItemInModel(row, 13, QString::number(Dd_Block.sU,'f',5));
    }
    else
    {
        int column = 0;
        UpdateItemInModel(row, column++, QString::number(Percents[Pindex], 10));
        UpdateItemInModel(row, column++, QString::number(CurrentS, 'f', 3));
        UpdateItemInModel(row, column++, QString::number(ChA1->Bda_out.dUrms, 'f', 5));
        UpdateItemInModel(row, column++, QString::number(ChA1->Bda_out.Phy, 'f', 5));
        if (PovType == GOST_23625)
            column += 10;
        UpdateItemInModel(row, column++, QString::number(Bac_block.dU_cor[Pindex],'f',5));
        UpdateItemInModel(row, column++, QString::number(Bac_block.dPhy_cor[Pindex],'f',5));
        UpdateItemInModel(row, column++, QString::number(ChA1->Bda_out.dUrms,'f',5));
        UpdateItemInModel(row, column++, QString::number(ChA1->Bda_out.Phy,'f',5));
        UpdateItemInModel(row, column++, QString::number(Dd_Block.dUrms,'f',5));
        UpdateItemInModel(row, column++, QString::number(Dd_Block.Phy,'f',5));
        UpdateItemInModel(row, column++, QString::number(Dd_Block.sPhy,'f',5));
        UpdateItemInModel(row, column++, QString::number(Dd_Block.sU,'f',5));
    }
    ++Index;
    if (Index >= endcounter)
    {
        if (CurrentS == 0.25)
        {
            Index = 0;
            CurrentS = 1;
            if (MessageBox2::question(this, "Подтверждение", "На нагрузочном устройстве поверяемого ТН установите значение мощности, равное 1,0·Sном") == false)
            {
                pc.Cancelled = true;
                Decline();
            }
        }
        else
        {
            int section = 0;
            ReportModel->setHeaderData(section++, Qt::Horizontal, "Проц");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "S/Sном");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "dUrms(u)");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "Phy(u)");
            if (PovType == GOST_23625)
            {
                ReportModel->setHeaderData(section++, Qt::Horizontal, "dUrms(d)");
                ReportModel->setHeaderData(section++, Qt::Horizontal, "Phy(d)");
                ReportModel->setHeaderData(section++, Qt::Horizontal, "dUrms(ud)");
                ReportModel->setHeaderData(section++, Qt::Horizontal, "Phy(ud)");
                ReportModel->setHeaderData(section++, Qt::Horizontal, "dUrms(u-d)");
                ReportModel->setHeaderData(section++, Qt::Horizontal, "Phy(u-d)");
                ReportModel->setHeaderData(section++, Qt::Horizontal, "dUrms(md)");
                ReportModel->setHeaderData(section++, Qt::Horizontal, "Phy(md)");
                ReportModel->setHeaderData(section++, Qt::Horizontal, "sUrms(d)");
                ReportModel->setHeaderData(section++, Qt::Horizontal, "sPhy(d)");
            }
            ReportModel->setHeaderData(section++, Qt::Horizontal, "dUcor");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "dPhycor");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "Bda_out.dUrms");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "Bda_out.Phy");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "dUrms(mu)");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "Phy(mu)");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "sUrms(u)");
            ReportModel->setHeaderData(section++, Qt::Horizontal, "sPhy(u)");
            // запись файла протокола
            GenerateReport();
            // вывод протокола на экран
            // формирование отчёта
            Decline();
            return;
        }
    }
    Pindex = (Index > 4) ? (8 - Index) : Index;
    VoltageInkV = static_cast<float>(Bac_block.K_DN) * Percents[Pindex] / 1732;
    VoltageInV = static_cast<float>(1000 * Percents[Pindex]) / 1732;
    if (MessageBox2::question(this, "Подтверждение", "Подайте на делители напряжение " + \
                              QString::number(VoltageInkV, 'f', 1) + " кВ ("+QString::number(VoltageInV, 'f', 1)+" В)") == false)
        Decline();
    MeasurementTimer->start();
}

void A1Dialog::Decline()
{
    WDFunc::SetEnabled(this, "cancelpb", false);
    WDFunc::SetEnabled(this, "acceptpb", false);
    WDFunc::SetEnabled(this, "StartWorkPb", true);
    MeasurementTimer->stop();
}

void A1Dialog::Proceed()
{
    PovType = TempPovType;
}

void A1Dialog::Cancel()
{
    pc.Cancelled = true;
}

void A1Dialog::SetDNData()
{
    QString PovDev, PovDevSN, PovDevPrecision;
    WDFunc::LEData(this, "UKDNOrganization", pc.OrganizationString);
    WDFunc::LEData(this, "DNType", ReportHeader.DNType);
    WDFunc::LEData(this, "DNNamePhase", ReportHeader.DNNamePhase);
    WDFunc::LEData(this, "DNSerialNum", ReportHeader.DNSerNum);
    WDFunc::LEData(this, "DNTolerance", ReportHeader.DNTol);
    WDFunc::LEData(this, "DNU1", ReportHeader.DNU1);
    WDFunc::LEData(this, "DNU2", ReportHeader.DNU2);
    WDFunc::LEData(this, "DNP", ReportHeader.DNP);
    WDFunc::LEData(this, "DNFreq", ReportHeader.DNF);
    WDFunc::LEData(this, "DNOrganization", ReportHeader.DNOrganization);
    WDFunc::LEData(this, "DNPlace", ReportHeader.DNPlace);
    WDFunc::LEData(this, "DNInspection", ReportHeader.OuterInsp);
    WDFunc::LEData(this, "DNWindingInspection", ReportHeader.WindingsInsp);
    WDFunc::LEData(this, "PovDev", PovDev);
    WDFunc::LEData(this, "PovDevSN", PovDevSN);
    WDFunc::LEData(this, "PovDevPrecision", PovDevPrecision);
    ReportHeader.DNDevices = PovDev + " сер. номер " + PovDevSN + ", кл. точн. " + PovDevPrecision;
    emit CloseDialog();
}

void A1Dialog::SetConditionData()
{
//    cn->Send(CN_GBd, A1_BDA_OUT_AN_BN, &ChA1->Bda_out_an, sizeof(CheckA1::A1_Bd4));
    CM_GetBd(A1_BDA_OUT_AN_BN, &ChA1->Bda_out_an, sizeof(CheckA1::A1_Bd4));
    if ((CA1->Bci_block.DTCanal == 0) || (ChA1->Bda_out_an.Tamb == FLT_MAX))
        WDFunc::LEData(this, "Temp", ReportHeader.Temp);
    else
        ReportHeader.Temp = QString::number(ChA1->Bda_out_an.Tamb, 'f', 5);
    if ((CA1->Bci_block.DHCanal == 0) || (ChA1->Bda_out_an.Hamb == FLT_MAX))
        WDFunc::LEData(this, "Humidity", ReportHeader.Humidity);
    else
        ReportHeader.Humidity = QString::number(ChA1->Bda_out_an.Hamb, 'f', 5);
    WDFunc::LEData(this, "Pressure", ReportHeader.Pressure);
    WDFunc::LEData(this, "Voltage", ReportHeader.Voltage);
    WDFunc::LEData(this, "Frequency", ReportHeader.Freq);
    emit CloseDialog();
}

void A1Dialog::RBToggled()
{
    QString tmps = sender()->objectName();
    if (tmps == "rb1")
        TempPovType = GOST_23625;
    else
        TempPovType = GOST_1983;
}

int A1Dialog::GetStatistics()
{
    // накопление измерений
    DdStruct tmpst2;
    tmpst2.dUrms = tmpst2.Phy = tmpst2.sPhy = tmpst2.sU = 0;
    QList<float> sPhy, sU;
    int count = 0;
    emit StartPercents(TUNE_COUNTEND);
    WaitWidget *w = new WaitWidget;
    w->SetMessage("Пожалуйста, подождите...");
    w->Start();
    while ((count < TUNE_COUNTEND) && !pc.Cancelled)
    {
        w->SetSeconds(TUNE_COUNTEND-count);
/*        cn->Send(CN_GBd, A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1));
        if (cn->result == NOERROR) */
        if (CM_GetBd(A1_BDA_OUT_BN, &ChA1->Bda_out, sizeof(CheckA1::A1_Bd1)) == NOERROR)
            FillBdOut();
        else
        {
            w->close();
            MessageBox2::information(this, "Внимание", "Ошибка при приёме блока Bda_out");
            return GENERALERROR;
        }
        tmpst2.dUrms += ChA1->Bda_out.dUrms;
        tmpst2.Phy += ChA1->Bda_out.Phy;
        sU.append(tmpst2.dUrms);
        sPhy.append(tmpst2.Phy);
        QTime tme;
        tme.start();
        while (tme.elapsed() < TUNE_POINTSPER)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        ++count;
        emit SetPercent(count);
    }
    w->close();
    if (pc.Cancelled)
        return GENERALERROR;
    // усреднение
    float Um = tmpst2.dUrms / count; // среднее значение погрешности по напряжению
    float Phym = tmpst2.Phy / count; // среднее значение погрешности по углу
    Dd_Block.dUrms = Um;
    Dd_Block.Phy = Phym;
    float sUo, sPhyo; // временные накопительные СКО
    sUo = sPhyo = 0;
    for (int i=0; i<count; ++i)
    {
        if ((i < sPhy.size()) && (i < sU.size()))
        {
            sUo += qPow((sU.at(i) - Um), 2);
            sPhyo += qPow((sPhy.at(i) - Phym), 2);
        }
    }
    sUo = qSqrt(sUo/count);
    sPhyo = qSqrt(sPhyo/count);
    Dd_Block.sPhy = sPhyo;
    Dd_Block.sU = sUo;
    FillMedian();
    return NOERROR;
}
