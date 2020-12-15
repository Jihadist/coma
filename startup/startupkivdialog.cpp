#include "startupkivdialog.h"

#include "../dialogs/keypressdialog.h"
#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/stdfunc.h"
#include "../gen/timefunc.h"
#include "../widgets/etableview.h"
#include "../widgets/wd_func.h"

#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>

StartupKIVDialog::StartupKIVDialog(QWidget *parent) : AbstractStartupDialog(parent)
{
    //    int i;
    // Default initialization
    // Do not need set null value
    CorBlock = new CorData();

    m_regMap[4000] = &CorBlock->C_init[0];
    m_regMap[4001] = &CorBlock->C_init[1];
    m_regMap[4002] = &CorBlock->C_init[2];
    m_regMap[4003] = &CorBlock->Tg_init[0];
    m_regMap[4004] = &CorBlock->C_init[1];
    m_regMap[4005] = &CorBlock->C_init[2];
    m_regMap[4006] = &CorBlock->corTg[0];
    m_regMap[4007] = &CorBlock->corTg[1];
    m_regMap[4008] = &CorBlock->corTg[2];
    m_regMap[4009] = &CorBlock->Iunb_init;
    m_regMap[4010] = &CorBlock->Phy_unb_init;
    m_regMap[4011] = reinterpret_cast<float *>(&CorBlock->stat);

    /*MessageTimer = new QTimer;
    MessageTimer->setInterval(5000);
    connect(MessageTimer,SIGNAL(timeout()),this,SLOT(TimerTimeout()));*/

    setAttribute(Qt::WA_DeleteOnClose);
    SetStartupBlock(7, &CorBlock, sizeof(CorData), KIVSTARTUPINITREG);
    // MessageTimer->start();
}

StartupKIVDialog::~StartupKIVDialog()
{
}

void StartupKIVDialog::SetupUI()
{
    // QWidget *cp2 = new QWidget;
    // QString tmps = "QDialog {background-color: " + QString(Colors::ACONFCLR) + ";}";
    // setStyleSheet(tmps);
    QVBoxLayout *lyout = new QVBoxLayout;
    QGridLayout *glyout = new QGridLayout;
    ETableView *tv = new ETableView;
    tv->setObjectName("cor");
    int row = 0;
    QString paramcolor = Colors::MAINWINCLR;

    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения емкостей вводов, пФ:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewSPB(this, QString::number(4000 + i), 0, 10000, 1, paramcolor), row, 2 + i, 1, 1);
    }

    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Начальные значения tg δ вводов, %:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewSPB(this, QString::number(4003 + i), -10, 10, 2, paramcolor), row, 2 + i, 1, 1);
    }

    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Коррекция  tg δ вводов,%:"), row, 1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        glyout->addWidget(WDFunc::NewSPB(this, QString::number(4006 + i), -10, 10, 2, paramcolor), row, 2 + i, 1, 1);
    }
    row++;

    glyout->addWidget(WDFunc::NewLBL(this, "Начальное действ. значение тока небаланса, мА:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(4009), 0, 10000, 1, paramcolor), row, 2, 1, 3);

    row++;
    glyout->addWidget(WDFunc::NewLBL(this, "Начальное значение угла тока небаланса, град.:"), row, 1, 1, 1);
    glyout->addWidget(WDFunc::NewSPB(this, QString::number(4010), 0, 10000, 1, paramcolor), row, 2, 1, 3);

    //    row++;

    lyout->addLayout(glyout, Qt::AlignTop);
    lyout->addWidget(buttonWidget());
    lyout->addWidget(tv, 89);
    setLayout(lyout);
    setObjectName("corDialog");
    connect(&DataManager::GetInstance(), &DataManager::floatReceived, this, &UWidget::updateFloatData);
}

void StartupKIVDialog::FillBackCor()
{
    auto i = m_regMap.constBegin();
    while (i != m_regMap.constEnd())
    {
        WDFunc::SPBData(this, QString::number(i.key()), *i.value());
        ++i;
    }
}

void StartupKIVDialog::FillCor()
{
    auto i = m_regMap.begin();
    while (i != m_regMap.end())
    {
        WDFunc::SetSPBData(this, QString::number(i.key()), *i.value());
        ++i;
    }
}

void StartupKIVDialog::GetCorBd()
{
    BaseInterface::iface()->reqFloats(4000, 12);
}
// void StartupKIVDialog::GetCorBdButton()
//{
//    switch (Board::GetInstance().interfaceType())
//    {
//    case Board::InterfaceType::USB:
//    {
//        if (Commands::GetBd(7, CorBlock, sizeof(CorData)) == Error::Msg::NoError)
//        {
//            FillCor();
//            QMessageBox::information(this, "INFO", "Прочитано успешно");
//        }
//        break;
//    }
//    case Board::InterfaceType::RS485:
//    {
//        ModBus::Information info;
//        info.size = (sizeof(CorData) / 4);
//        info.adr = 4000;
//        emit RS485ReadCorBd(info);
//        break;
//    }
//    case Board::InterfaceType::Ethernet:
//    {
//        emit CorReadRequest();
//        break;
//    }
//    }
//}

void StartupKIVDialog::WriteCorBd()
{
    int i;
    quint32 adr[11] = { 910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920 };

    FillBackCor();

    if (WriteCheckPassword() != Error::Msg::NoError)
        return;
    switch (Board::GetInstance().interfaceType())
    {
    case Board::InterfaceType::Ethernet:
    {
        for (i = 0; i < 11; i++)
        {
            float corblocki = *(reinterpret_cast<float *>(CorBlock) + i);
            // memcpy(&corblocki, reinterpret_cast<float *>(CorBlock) + i, sizeof(float));
            // emit SendCom50(adr[i], corblocki);
            TimeFunc::Wait(300);
        }
        break;
    }
    case Board::InterfaceType::RS485:
    {
        //            ModBus::Information info;
        //            info.size = (sizeof(CorData) / 4);
        //            info.adr = adr[0];
        //            emit RS485WriteCorBd(info, (float *)CorBlock);
        break;
    }
    case Board::InterfaceType::USB:
    {
        //            if (Commands::WriteBd(7, CorBlock, sizeof(CorData)) == Error::Msg::NoError)
        //                QMessageBox::information(this, "INFO", "Записано успешно");
        //            else
        //                QMessageBox::information(this, "INFO", "Ошибка");

        //            if (Commands::GetBd(7, CorBlock, sizeof(CorData)) == Error::Msg::NoError)
        //                FillCor();
        break;
    }
    }
}

void StartupKIVDialog::WriteCor()
{
    if (WriteCheckPassword() != Error::Msg::NoError)
        return;
    switch (Board::GetInstance().interfaceType())
    {
    case Board::InterfaceType::Ethernet:
    {
        //            emit SendCom45(SETINITREG);
        //            QMessageBox::information(this, "INFO", "Задано успешно");
        //            emit CorReadRequest();
        break;
    }
    case Board::InterfaceType::RS485:
    {
        //            ModBus::Information info;
        //            info.size = 1;
        //            info.adr = SETINITREG;
        //            emit RS485WriteCorBd(info, nullptr);
        //            QMessageBox::information(this, "INFO", "Задано успешно");
        //            info.size = (sizeof(CorData) / 4);
        //            info.adr = 4000;
        //            emit RS485ReadCorBd(info);
        break;
    }
    case Board::InterfaceType::USB:
    {
        //            if (Commands::WriteCom(Commands::WriteInitValues) == Error::Msg::NoError) // задание общей
        //            коррекции
        //                                                                                      //{
        //                if (Commands::GetBd(7, CorBlock, sizeof(CorData)) == Error::Msg::NoError)
        //                {
        //                    FillCor();
        //                    QMessageBox::information(this, "INFO", "Задано и прочитано успешно");
        //                }
        //                // }
        //                else
        //                    QMessageBox::information(this, "INFO", "Ошибка");
        break;
    }
    }
}

void StartupKIVDialog::SetCor()
{
    switch (Board::GetInstance().interfaceType())
    {
    case Board::InterfaceType::Ethernet:
    {
        // emit SendCom45(903);
        break;
    }
    case Board::InterfaceType::USB:
    {
        //        if (Commands::WriteCom(Commands::WriteStartupValues) == Error::Msg::NoError)
        //            QMessageBox::information(this, "INFO", "Записано успешно");
        //        else
        //            QMessageBox::information(this, "INFO", "Ошибка");
        break;
    }
    }
}

void StartupKIVDialog::ResetCor()
{
    if (WriteCheckPassword() != Error::Msg::NoError)
        return;

    //        switch (Board::GetInstance().interfaceType())
    //        {
    //        case Board::InterfaceType::Ethernet:
    //        {
    //            emit SendCom45(CLEARREG);
    //            break;
    //        }
    //        case Board::InterfaceType::RS485:
    //        {
    //            ModBus::Information info;
    //            info.size = 1;
    //            info.adr = CLEARREG;
    //            emit RS485WriteCorBd(info, nullptr);
    //            break;
    //        }
    //        case Board::InterfaceType::USB:
    //        {
    //            if (Commands::WriteCom(Commands::ClearStartupValues) == Error::Msg::NoError)
    //                QMessageBox::information(this, "INFO", "Сброшено успешно");
    //            else
    //                QMessageBox::information(this, "INFO", "Ошибка");

    //            if (Commands::GetBd(7, CorBlock, sizeof(CorBlock)) == Error::Msg::NoError)
    //                FillCor();
    //            break;
    //        }
    //        }
}

// float StartupKIVDialog::ToFloat(QString text)
//{
//    bool ok;
//    float tmpf;
//    tmpf = text.toFloat(&ok);
//    if (!ok)
//    {
//        qCritical() << "Значение " << text << " не может быть переведено во float";
//        return 0;
//    }
//    return tmpf;
//}

void StartupKIVDialog::MessageOk()
{
    QMessageBox::information(this, "INFO", "Записано успешно");
}

// void CorDialog::UpdateFlCorData(IEC104Thread::FlSignals104 *Signal)
//{
//    if (((Signal)->fl.SigAdr >= 4000) && ((Signal)->fl.SigAdr <= 4010))
//    {
//        for (int i = 0; i < Signal->SigNumber; i++)
//        {
//            // if((Signal+i)->fl.SigAdr >= 1000 || (Signal+i)->fl.SigAdr <= 1009)
//            FillBd(
//                this, QString::number((Signal + i)->fl.SigAdr), WDFunc::StringValueWithCheck((Signal +
//                i)->fl.SigVal));
//        }

//        if (first)
//            QMessageBox::information(this, "INFO", "Прочитано успешно");
//        else
//            first = 1;
//    }
//}

// void CorDialog::FillBd(QWidget *parent, QString Name, QString Value)
//{
//    WDFunc::SetSPBData(parent, Name, Value.toDouble());
//}

// void CorDialog::ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal)
//{
//    int i = 0;

//    if (Signal.size() > 0)
//    {
//        if (Signal.at(0).SigAdr == 4000)
//        {
//            for (i = 0; i < Signal.size(); ++i)
//            {
//                FillBd(this, QString::number(Signal.at(i).SigAdr),
//                WDFunc::StringValueWithCheck(Signal.at(i).flVal));
//            }
//            QMessageBox::information(this, "INFO", "Прочитано успешно");
//        }
//    }
//}

void StartupKIVDialog::SaveToFile()
{
    QByteArray ba = QByteArray::fromRawData(reinterpret_cast<char *>(CorBlock), sizeof(CorBlock));
    FillBackCor();

    Error::Msg res = Files::SaveToFile(Files::ChooseFileForSave(this, "Tune files (*.cor)", "cor"), ba);
    switch (res)
    {
    case Error::Msg::NoError:
        QMessageBox::information(this, "Внимание", "Файл коэффициентов коррекции записан успешно!");
        break;
    case Error::Msg::FileWriteError:
        QMessageBox::critical(this, "Ошибка", "Ошибка при записи файла!");
        break;
    case Error::Msg::FileNameError:
        QMessageBox::critical(this, "Ошибка", "Пустое имя файла!");
        break;
    case Error::Msg::FileOpenError:
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия файла!");
        break;
    default:
        break;
    }
}

void StartupKIVDialog::ReadFromFile()
{
    QByteArray ba;
    ba.resize(sizeof(CorData));

    Error::Msg res = Files::LoadFromFile(Files::ChooseFileForOpen(this, "Tune files (*.cor)"), ba);
    if (res != Error::Msg::NoError)
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка при загрузке файла");
        ERMSG("Ошибка при загрузке файла");
        return;
    }
    // CorBlock = reinterpret_cast<CorData *>(ba.data());
    memcpy(CorBlock, &(ba.data()[0]), sizeof(CorData));

    FillCor();
    QMessageBox::information(this, "Внимание", "Загрузка прошла успешно!");
}

bool StartupKIVDialog::WriteCheckPassword()
{
    KeyPressDialog dlg; // = new KeyPressDialog;
    return dlg.CheckPassword("121941");
}

// void StartupKIVDialog::WritePasswordCheck(QString psw)
//{
//    if (psw == "121941")
//        ok = true;
//    else
//        ok = false;
//    emit WritePasswordChecked();
//}

// void StartupKIVDialog::TimerTimeout()
//{
//    MessageTimer->stop();
//    // first = 1;
//}

/*void CorDialog::WriteCorMessageOk()
{
   QMessageBox::information(this, "Успешно", "Записано успешно!");
} */

void StartupKIVDialog::ErrorRead()
{
    QMessageBox::information(this, "Ошибка", "Ошибка чтения");
}
