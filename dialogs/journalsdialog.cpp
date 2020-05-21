#include <QGroupBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QApplication>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include "../widgets/emessagebox.h"
#include "../gen/maindef.h"
#include "../widgets/getoscpbdelegate.h"
#include "../widgets/wd_func.h"
#include "../gen/stdfunc.h"
#include "../gen/error.h"
#include "../gen/timefunc.h"
#include "../dialogs/journalsdialog.h"
#include "../gen/colors.h"
#include "../gen/mainwindow.h"
#include "../gen/files.h"
#if PROGSIZE != PROGSIZE_EMUL
#include "../gen/commands.h"
#endif


JournalDialog::JournalDialog() : QDialog()
{
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

void JournalDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString tmps = "QDialog {background-color: "+QString(ACONFCLR)+";}";
    setStyleSheet(tmps);

    QTabWidget *work = new QTabWidget;
    QString ConfTWss = "QTabBar::tab:selected {background-color: "+QString(TABCOLOR)+";}";
    work->tabBar()->setStyleSheet(ConfTWss);
    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(JOURWORK), "Получить рабочий журнал", this, SLOT(GetJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(JOURWORK), "Стереть рабочий журнал", this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(JOURWORK), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, "work", new ETableModel), 89);
    work->setLayout(vlyout);
    work->setStyleSheet(ConfTWss);

    QTabWidget *system = new QTabWidget;
    system->tabBar()->setStyleSheet(ConfTWss);

    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(JOURSYS), "Получить системный журнал", this, SLOT(GetJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(JOURSYS), "Стереть системный журнал", this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(JOURSYS), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, "system", new ETableModel), 89);
    system->setLayout(vlyout);
    system->setStyleSheet(ConfTWss);

    QTabWidget *measure = new QTabWidget;
    measure->tabBar()->setStyleSheet(ConfTWss);
    hlyout = new QHBoxLayout;
    vlyout = new QVBoxLayout;
    hlyout->addWidget(WDFunc::NewPB(this, "gj." + QString::number(JOURMEAS), "Получить журнал измерений", this, SLOT(GetJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "ej." + QString::number(JOURMEAS), "Стереть журнал измерений", this, SLOT(EraseJour())));
    hlyout->addWidget(WDFunc::NewPB(this, "sj." + QString::number(JOURMEAS), "Сохранить журнал в файл", this, SLOT(SaveJour())));
    vlyout->addLayout(hlyout);
    vlyout->addWidget(WDFunc::NewTV(this, "meas", new ETableModel), 89);
    measure->setLayout(vlyout);
    measure->setStyleSheet(ConfTWss);

    QTabWidget *ConfTW = new QTabWidget;
    ConfTW->setObjectName("conftw4");
    ConfTW->tabBar()->setStyleSheet(ConfTWss);
    ConfTW->addTab(work,"Рабочий журнал");
    ConfTW->addTab(system,"Системный журнал");
    ConfTW->addTab(measure,"Журнал измерений");

    lyout->addWidget(ConfTW);
    setLayout(lyout);
    if (StdFunc::IsInEmulateMode())
    {
        WDFunc::SetEnabled(this, "gj." + QString::number(JOURSYS), false);
        WDFunc::SetEnabled(this, "gj." + QString::number(JOURMEAS), false);
        WDFunc::SetEnabled(this, "gj." + QString::number(JOURWORK), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(JOURSYS), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(JOURMEAS), false);
        WDFunc::SetEnabled(this, "ej." + QString::number(JOURWORK), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(JOURSYS), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(JOURMEAS), false);
        WDFunc::SetEnabled(this, "sj." + QString::number(JOURWORK), false);
    }
}

#if PROGSIZE != PROGSIZE_EMUL
void JournalDialog::GetJour()
{
    int jourtype = GetJourNum(sender()->objectName());
    if (jourtype == Error::ER_GENERALERROR)
        return;
    char num = jourtype + 4;
    if (MainWindow::MainInterface == "Ethernet")
    {
        emit ReadJour(num);
    }
    else if (MainWindow::MainInterface == "USB")
    {
//        QVector<S2::DataRec> *Jour = new QVector<S2::DataRec>;
        QByteArray ba;
        if(Commands::GetFile(num, ba) == Error::ER_NOERROR)
        {
            switch(jourtype)
            {
            case JOURSYS:
                FillEventsTable(ba.data(), JOURSYS);
                break;
            case JOURWORK:
                FillEventsTable(ba.data(), JOURWORK);
                break;
            case JOURMEAS:
                FillMeasTable(ba.data());
                break;
            default:
                break;
            }
        }
        else
            EMessageBox::information(this, "Ошибка", "Ошибка чтения журнала");
    }
}

void JournalDialog::EraseJour()
{
    if (MainWindow::MainInterface == "USB")
    {
        int jourtype = GetJourNum(sender()->objectName());
        if (jourtype == Error::ER_GENERALERROR)
            return;
        char num = jourtype + 4;

        if(Commands::EraseTechBlock(num) == Error::ER_NOERROR)
        {
          EMessageBox::information(this, "Успешно", "Стирание прошло успешно");
        }
        else
        {
          EMessageBox::information(this, "Ошибка", "Ошибка");
        }
    }
}

void JournalDialog::SaveJour()
{
    QXlsx::Format cellformat;
    QString tvname, jourtypestr, jourfilestr;
    Qt::SortOrder order = Qt::AscendingOrder;
    int jourtype = GetJourNum(sender()->objectName());
    if (jourtype == Error::ER_GENERALERROR)
        return;
    jourfilestr = "KIV #" + QString("%1").arg(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 8, 10, QChar('0')) + " ";
    switch(jourtype)
    {
    case JOURSYS:
        tvname = "system";
        jourtypestr = "Системный журнал";
        jourfilestr += "SysJ ";
        order = Qt::DescendingOrder;
//        cellformat.setNumberFormat("");
        break;
    case JOURMEAS:
        tvname = "meas";
        jourtypestr = "Журнал измерений";
        jourfilestr += "MeasJ ";
        order = Qt::AscendingOrder;
        cellformat.setNumberFormat("#.####");
        break;
    case JOURWORK:
        tvname = "work";
        jourtypestr = "Журнал событий";
        jourfilestr += "WorkJ ";
        order = Qt::DescendingOrder;
//        cellformat.setNumberFormat("");
        break;
    default:
        break;
    }
    ETableModel *mdl = WDFunc::TVModel(this, tvname);
    QSortFilterProxyModel *pmdl = new QSortFilterProxyModel;
    pmdl->setSourceModel(mdl);
    int dateidx = mdl->Headers().indexOf("Дата/Время");
    pmdl->sort(dateidx, order);
    jourfilestr += QDate::currentDate().toString("dd-MM-yyyy") + " .xlsx";
    // запрашиваем имя файла для сохранения
    QString filename = Files::ChooseFileForSave(this, "Excel documents (*.xlsx)", "xlsx", jourfilestr);
    QXlsx::Document *xlsx = new QXlsx::Document(filename);
    xlsx->write(1,1,QVariant(jourtypestr));
    xlsx->write(2,1,QVariant("Модуль: " + ModuleBSI::GetModuleTypeString() + " сер. ном. " + \
                             QString::number(ModuleBSI::SerialNum(BoardTypes::BT_MODULE), 10)));
    xlsx->write(3,1,QVariant("Дата сохранения журнала: "+QDateTime::currentDateTime().toString("dd-MM-yyyy")));
    xlsx->write(4,1,QVariant("Время сохранения журнала: "+QDateTime::currentDateTime().toString("hh:mm:ss")));

    // пишем в файл заголовки
    for (int i=0; i<pmdl->columnCount(); ++i)
        xlsx->write(5, (i+1), pmdl->headerData(i, Qt::Horizontal, Qt::DisplayRole));

//    QXlsx::Format numformat; //, dateformat;
//    numformat.setNumberFormat("0");
//    dateformat.setNumberFormat("dd-mm-yyyy hh:mm:ss");
    // теперь по всем строкам модели пишем данные
    for (int i=0; i<pmdl->rowCount(); ++i)
    {
        // номер события
        xlsx->write((6+i), 1, pmdl->data(pmdl->index(i, 0), Qt::DisplayRole).toString());
        // время события
        xlsx->write((6+i), 2, pmdl->data(pmdl->index(i, 1), Qt::DisplayRole).toString());
        for (int j=2; j<pmdl->columnCount(); ++j)
//            xlsx->write((6+i), (1+j), pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toString(), cellformat);
            xlsx->write((6+i), (1+j), pmdl->data(pmdl->index(i, j), Qt::DisplayRole).toString());
    }
    xlsx->save();
    EMessageBox::information(this, "Внимание", "Файл создан успешно");
}

void JournalDialog::FillSysJour(QByteArray ba)
{
    FillEventsTable(ba.data(), JOURSYS);
}

void JournalDialog::FillMeasJour(QByteArray ba)
{
    FillMeasTable(ba.data());
}

void JournalDialog::FillWorkJour(QByteArray ba)
{
    FillEventsTable(ba.data(), JOURWORK);
}


void JournalDialog::FillEventsTable(char *file, int jourtype)
{
    QVector<QVector<QVariant>> lsl;
    QVector<QVariant> EventNum, Num, Time, Type;
    ETableModel *model = new ETableModel;
    EventStruct event;
    const QStringList sl = (jourtype == JOURSYS) ? SysJourDescription : WorkJourDescription;
    int mineventid = (jourtype == JOURSYS) ? SYSJOURID : WORKJOURID;
    const QString tvname = (jourtype == JOURSYS) ? "system" : "work";
    int N = 0;
    int joursize = 0; // размер считанного буфера с информацией
    int recordsize = sizeof(EventStruct);
    int fhsize = sizeof(S2::FileHeader);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    file += fhsize;
    S2::DataRec jour;
    int drsize = sizeof(S2::DataRec) - sizeof(void *);
    memcpy(&jour, file, drsize);
    joursize = jour.num_byte;
    file += drsize; // move file pointer to thedata
    int counter = 0;
    int i = 0;
    while (i < joursize)
    {
        memcpy(&event, file, recordsize);
        file += recordsize;
        i += recordsize;
        if(event.Time != 0xFFFFFFFFFFFFFFFF)
        {
            memcpy(&N, &event.EvNum, sizeof(event.EvNum));
            N = (N & 0x00FFFFFF) - mineventid;
            if ((N <= sl.size()) && (N > 0))
            {
                --N;
                Num << sl.at(N);
            }
            else
                Num << "Некорректный номер события";
            Time << TimeFunc::UnixTime64ToInvStringFractional(event.Time);
            if(event.EvType)
                Type << "Пришло";
            else
                Type << "Ушло";
            ++counter;
            EventNum << counter;
        }
    }
    lsl.append(EventNum);
    lsl.append(Time);
    lsl.append(Num);
    lsl.append(Type);
    model->ClearModel();
    model->addColumn(" № ");
    model->addColumn("Дата/Время");
    model->addColumn("Описание события");
    model->addColumn("Тип события");
    model->fillModel(lsl);
    WDFunc::SetTVModel(this, tvname, model);
    QApplication::restoreOverrideCursor();
}

void JournalDialog::FillMeasTable(char *file)
{
    QVector<QVector<QVariant> > lsl;
    ETableModel *model = new ETableModel;
    QVector<QVariant> EventNum, Time, UeffA, UeffB, UeffC, IeffA, IeffB, IeffC, U0, U1, U2,
                      I0, I1, I2, CbushA, CbushB, CbushC, Tg_dA, Tg_dB, Tg_dC, dCbushA, dCbushB,
                      dCbushC, dTg_dA, dTg_dB, dTg_dC, Iunb, Phy_unb, Tmk, Tokr;

    MeasureStruct meas;
    int recordsize = sizeof(MeasureStruct);

    int joursize = 0; // размер считанного буфера с информацией
    int fhsize = sizeof(S2::FileHeader);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    file += fhsize;
    S2::DataRec jour;
    int drsize = sizeof(S2::DataRec) - sizeof(void *);
    memcpy(&jour, file, drsize);
    joursize = jour.num_byte;
    file += drsize; // move file pointer to thedata
    int i = 0;
    while (i < joursize)
    {
        memcpy(&meas, file, recordsize);
        file += recordsize;
        i += recordsize;


        if(meas.Time != 0xFFFFFFFF)
        {
            EventNum << meas.NUM;
            Time << TimeFunc::UnixTime32ToInvString(meas.Time);
            UeffA << meas.Ueff[0];
            UeffB << meas.Ueff[1];
            UeffC << meas.Ueff[2];
            IeffA << meas.Ieff[0];
            IeffB << meas.Ieff[1];
            IeffC << meas.Ieff[2];
            U0 << meas.U0;
            U1 << meas.U1;
            U2 << meas.U2;
            I0 << meas.I0;
            I1 << meas.I1;
            I2 << meas.I2;
            CbushA << meas.Cbush[0];
            CbushB << meas.Cbush[1];
            CbushC << meas.Cbush[2];
            Tg_dA << meas.Tg_d[0];
            Tg_dB << meas.Tg_d[1];
            Tg_dC << meas.Tg_d[2];
            dCbushA << meas.dCbush[0];
            dCbushB << meas.dCbush[1];
            dCbushC << meas.dCbush[2];
            dTg_dA << meas.dTg_d[0];
            dTg_dB << meas.dTg_d[1];
            dTg_dC << meas.dTg_d[2];
            Iunb << meas.Iunb;
            Phy_unb << meas.Phy_unb;
            Tmk << meas.Tmk;
            Tokr << meas.Tamb;
        }
    }
   lsl.append(EventNum);
   lsl.append(Time);
   lsl.append(UeffA);
   lsl.append(UeffB);
   lsl.append(UeffC);
   lsl.append(IeffA);
   lsl.append(IeffB);
   lsl.append(IeffC);
   lsl.append(U0);
   lsl.append(U1);
   lsl.append(U2);
   lsl.append(I0);
   lsl.append(I1);
   lsl.append(I2);
   lsl.append(CbushA);
   lsl.append(CbushB);
   lsl.append(CbushC);
   lsl.append(Tg_dA);
   lsl.append(Tg_dB);
   lsl.append(Tg_dC);
   lsl.append(dCbushA);
   lsl.append(dCbushB);
   lsl.append(dCbushC);
   lsl.append(dTg_dA);
   lsl.append(dTg_dB);
   lsl.append(dTg_dC);
   lsl.append(Iunb);
   lsl.append(Phy_unb);
   lsl.append(Tmk);
   lsl.append(Tokr);

   model->ClearModel();
   model->addColumn("Номер события");
   model->addColumn("Дата/Время");
/*   model->addColumn("Действующее значение напряжения фазы А, кВ");
   model->addColumn("Действующее значение напряжения фазы B, кВ");
   model->addColumn("Действующее значение напряжения фазы C, кВ");
   model->addColumn("Действующее значение тока фазы А, мА");
   model->addColumn("Действующее значение тока фазы B, мА");
   model->addColumn("Действующее значение тока фазы C, мА");
   model->addColumn("Напряжение нулевой последовательности");
   model->addColumn("Напряжение прямой последовательности");
   model->addColumn("Напряжение обратной последовательности");
   model->addColumn("Ток нулевой последовательности");
   model->addColumn("Ток прямой последовательности");
   model->addColumn("Ток обратной последовательности");
   model->addColumn("Емкость ввода фазы А, пФ");
   model->addColumn("Емкость ввода фазы B, пФ");
   model->addColumn("Емкость ввода фазы C, пФ");
   model->addColumn("tg delta ввода фазы А, %");
   model->addColumn("tg delta ввода фазы B, %");
   model->addColumn("tg delta ввода фазы C, %");
   model->addColumn("Изменение емкости ввода фазы А, % от C_init");
   model->addColumn("Изменение емкости ввода фазы B, % от C_init");
   model->addColumn("Изменение емкости ввода фазы C, % от C_init");
   model->addColumn("Изменение tg delta ввода фазы А, %");
   model->addColumn("Изменение tg delta ввода фазы B, %");
   model->addColumn("Изменение tg delta ввода фазы C, %");
   model->addColumn("Действующее значение 1-й гармоники тока небаланса, мА");
   model->addColumn("Угол тока небаланса относительно тока ф.А, град");
   model->addColumn("Температура кристалла микроконтроллера");
   model->addColumn("Температура окружающей среды"); */
   model->addColumn("Ueff фA");
   model->addColumn("Ueff фB");
   model->addColumn("Ueff фC");
   model->addColumn("Ieff фA");
   model->addColumn("Ieff фB");
   model->addColumn("Ieff фC");
   model->addColumn("U0");
   model->addColumn("U1");
   model->addColumn("U2");
   model->addColumn("I0");
   model->addColumn("I1");
   model->addColumn("I2");
   model->addColumn("Cbush фA");
   model->addColumn("Cbush фB");
   model->addColumn("Cbush фC");
   model->addColumn("Tg_d фA");
   model->addColumn("Tg_d фB");
   model->addColumn("Tg_d фC");
   model->addColumn("dCbush фA");
   model->addColumn("dCbush фB");
   model->addColumn("dCbush фC");
   model->addColumn("dTg_d фA");
   model->addColumn("dTg_d фB");
   model->addColumn("dTg_d фC");
   model->addColumn("Iunb");
   model->addColumn("Phy_unb");
   model->addColumn("Tmk, °С");
   model->addColumn("Tamb, °С");
   model->fillModel(lsl);
   WDFunc::SetTVModel(this, "meas", model);
   QApplication::restoreOverrideCursor();
}

int JournalDialog::GetJourNum(const QString &objname)
{
    bool ok;
    QStringList sl = objname.split(".");

    if (sl.size() < 2)
    {
        DBGMSG;
        return Error::ER_GENERALERROR;
    }
    int jourtype = sl.at(1).toInt(&ok);
    if (((sl.at(0) != "gj") && (sl.at(0) != "ej") && (sl.at(0) != "sj")) || !ok)
    {
        DBGMSG;
        return Error::ER_GENERALERROR;
    }
    return jourtype;
}
#endif


