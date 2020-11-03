#include "eprotocom.h"

#include "../gen/board.h"
#include "../gen/datamanager.h"
#include "../gen/error.h"
#include "../gen/stdfunc.h"
#include "defines.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

//#define MAX_STR 255
#ifdef _WIN32
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif

#if _MSC_VER && !__INTEL_COMPILER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

const QMap<Queries::Commands, CN::Commands> EProtocom::m_dict {
    { Queries::Commands::QC_StartFirmwareUpgrade, CN::Commands::WriteUpgrade },
    { Queries::QC_SetStartupValues, CN::Commands::WriteStartupValues },
    { Queries::QC_ClearStartupValues, CN::Commands::EraseStartupValues },
    { Queries::QC_SetNewConfiguration, CN::Commands::WriteInitValues }

};

bool EProtocom::m_writeUSBLog;

// namespace Read
//{
//// чтение блока стартовой информации
// constexpr byte BlkStartInfo = 0x21;
//// чтение настроечных коэффициентов
// constexpr byte BlkAC = 0x22;
//// чтение текущих данных без настройки
// constexpr byte BlkDataA = 0x23;
//// чтение блока текущих данных
// constexpr byte BlkData = 0x24;
//// чтение технологического блока
// constexpr byte BlkTech = 0x26;
//// чтение файла
// constexpr byte File = 0x25;
//// чтение номера варианта использования
// constexpr byte Variant = 0x27;
//// чтение текущего режима работы
// constexpr byte Mode = 0x28;
//// чтение времени из МНК в формате UNIX
// constexpr byte Time = 0x29;

//} // namespace Read
// namespace Write
//{
//// запись настроечных коэффициентов
// constexpr byte BlkAC = 0x31;
//// посылка блока данных
// constexpr byte BlkData = 0x34;
//// посылка команды
// constexpr byte BlkCmd = 0x35;
//// запись технологического блока
// constexpr byte BlkTech = 0x2B;
//// запись файла
// constexpr byte File = 0x32;
//// задание варианта использования
// constexpr byte Variant = 0x44;
//// задание текущего режима работы
// constexpr byte Mode = 0x43;
//// запись времени в МНК в формате UNIX
// constexpr byte Time = 0x2A;
//// переход на новое ПО
// constexpr byte Upgrade = 0x40;
//// стирание технологического блока
// constexpr byte EraseTech = 0x45;
//// стирание счётчиков дискретных состояний
// constexpr byte EraseCnt = 0x47;
//// запись версии аппаратуры модуля/серийного номера/типа платы
// constexpr byte Hardware = 0x48;
//} // namespace Write

EProtocom::EProtocom(token, QWidget *parent) : BaseInterface(parent)
{
    QString tmps = "=== CLog started ===\n";
    Log = new LogClass;
    Log->Init("canal.log");
    Log->WriteRaw(tmps.toUtf8());
    RDLength = 0;
    SegEnd = 0;
    SegLeft = 0;
    OscNum = 0;
    OscTimer = new QTimer;
    OscTimer->setInterval(CN::TimeoutOscillogram);
    OscTimer->setSingleShot(false);
    m_waitTimer = new QTimer;
    m_waitTimer->setInterval(CN::Timeout);
    connect(OscTimer, &QTimer::timeout, this, &EProtocom::OscTimerTimeout);
    connect(m_waitTimer, &QTimer::timeout, &m_loop, &QEventLoop::quit);
    connect(this, &EProtocom::QueryFinished, &m_loop, &QEventLoop::quit);
    // QSharedPointer<QSettings> sets = QSharedPointer<QSettings>(new QSettings("EvelSoft", PROGNAME));
    // bool writeUSBLog = sets->value("WriteLog", "0").toBool();
    setWriteUSBLog(true);
}

Error::Msg EProtocom::result() const
{
    return m_result;
}

void EProtocom::setResult(const Error::Msg &result)
{
    m_result = result;
}

bool EProtocom::isWriteUSBLog()
{
    return m_writeUSBLog;
}

void EProtocom::setWriteUSBLog(bool writeUSBLog)
{
    m_writeUSBLog = writeUSBLog;
}

void EProtocom::Send(char command, char parameter, QByteArray &ba, qint64 datasize)
{
    if (Board::GetInstance().connectionState() == Board::ConnectionState::Closed
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        qDebug() << Error::Msg::NoDeviceError;
        m_result = Error::Msg::NoDeviceError;
        return;
    }

    InData = ba;
    InDataSize = datasize; // размер области данных, в которую производить запись
    Command = command;
    BoardType = parameter; // in GBd command it is a block number
    InitiateSend();
    m_waitTimer->start();
    m_loop.exec(QEventLoop::ExcludeUserInputEvents);
}

void EProtocom::InitiateSend()
{
    WriteData.clear();
    switch (Command)
    {
    case CN::Read::BlkStartInfo: // запрос блока стартовой информации
    case CN::Read::Progress:     // запрос текущего прогресса
    case CN::Read::Variant:
    case CN::Read::Mode:
    case CN::Read::Time:
    case CN::Write::Upgrade:
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        AppendSize(WriteData, 0);
        WriteDataToPort(WriteData);
        break;
    }
    case CN::Read::BlkAC:    // чтение настроечных коэффициентов
    case CN::Read::BlkDataA: // чтение текущих данных без настройки
    case CN::Read::BlkData:  // запрос блока (подблока) текущих данных
    case CN::Write::Variant:
    case CN::Write::Mode:
    case CN::Read::BlkTech:    // чтение технологического блока
    case CN::Write::EraseTech: // команда стирания технологического блока
    case CN::Write::BlkCmd:
    case CN::Test:
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        AppendSize(WriteData, 1);
        WriteData.append(BoardType);
        WriteDataToPort(WriteData);
        break;
    }
    case CN::Read::File: // запрос файла
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        AppendSize(WriteData, 2);
        WriteData.append(static_cast<char>(FNum & 0x00FF));
        WriteData.append(static_cast<char>((FNum & 0xFF00) >> 8));
        WriteDataToPort(WriteData);
        break;
    }
    case CN::Write::Hardware:
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        int size = (BoardType == BoardTypes::BT_BSMZ) ? WHV_SIZE_TWOBOARDS : WHV_SIZE_ONEBOARD;
        AppendSize(WriteData, size); // BoardType(1), HiddenBlock(16)
        WriteData.append(BoardType);
        WriteData.append(InData);
        WriteDataToPort(WriteData);
        break;
    }
    case CN::Write::File: // запись файла
    {
        WriteData = InData;
        WRLength = WriteData.length();
        SetWRSegNum();
        WRCheckForNextSegment(true);
        break;
    }
    case CN::Write::BlkAC:
    case CN::Write::EraseCnt:
    case CN::Write::BlkTech:
    case CN::Write::BlkData:
    {
        WriteData.append(BoardType);
        WriteData.append(InData);
        WRLength = InDataSize + 1;
        SetWRSegNum();
        WRCheckForNextSegment(true);
        break;
    }
    case CN::Write::Time:
    {
        WriteData.append(CN::Message::Start);
        WriteData.append(Command);
        AppendSize(WriteData, InDataSize);
        WriteData.append(InData);
        WriteDataToPort(WriteData);
        break;
    }
    default:
    {
        ERMSG("Неизвестная команда");
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    }
    OutData.clear();
    ReadDataChunk.clear();
    // LastBlock = false;
    bStep = 0;
    RDLength = 0;
}

void EProtocom::WriteDataToPort(QByteArray &ba)
{
    QByteArray tmpba = ba;
    if (Command == CN::Unknown) // игнорируем вызовы процедуры без команды
    {
        ERMSG("Не пришла ещё шапка файла");
        qCritical() << QVariant::fromValue(Error::Msg::WrongCommandError).toString();
        return;
    }
    int byteswritten = 0;
    int basize = tmpba.size();
    while ((byteswritten < basize) && (!tmpba.isEmpty()))
    {
        if (isWriteUSBLog())
        {
            QByteArray tmps = "->" + tmpba.toHex() + "\n";
            Log->WriteRaw(tmps);
        }
        if (Board::GetInstance().connectionState() == Board::ConnectionState::Closed
            && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
        {
            ERMSG("Ошибка записи RawWrite");
            qCritical() << QVariant::fromValue(Error::Msg::WriteError).toString();
            Disconnect();
            return;
        }
        int tmpi = RawWrite(tmpba);
        byteswritten += tmpi;
        tmpba.remove(0, tmpi);
    }
}

void EProtocom::Finish(Error::Msg msg)
{
    // предотвращение вызова newdataarrived по приходу чего-то в канале, если
    // ничего не было послано
    Command = CN::Unknown;
    if (msg != Error::Msg::NoError)
    {

        qDebug(__PRETTY_FUNCTION__);
        Log->WriteRaw("### ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ ###\n");
        qWarning("ОШИБКА В ПЕРЕДАННЫХ ДАННЫХ!!!");
        qCritical() << QVariant::fromValue(msg).toString();
    }
    m_result = msg;
    emit QueryFinished();
}

void EProtocom::SetWRSegNum()
{
    if (WRLength > CN::Limits::MaxSegmenthLength)
        SegLeft = (WRLength / CN::Limits::MaxSegmenthLength) + 1;
    else
        SegLeft = 1;
    SegEnd = 0;
}

void EProtocom::WRCheckForNextSegment(int first)
{
    // quint8 ForMihalich = 0;
    QByteArray tmpba;
    if (SegLeft)
    {
        --SegLeft;
        if (first)
            tmpba.append(CN::Message::Start);
        else
            tmpba.append(CN::Message::Continue);
        tmpba.append(Command);
    }
    if (SegLeft)
    {
        AppendSize(tmpba, CN::Limits::MaxSegmenthLength);
        tmpba += WriteData.mid(SegEnd, CN::Limits::MaxSegmenthLength); // -4 = '<', cmd, L, L
        SegEnd += CN::Limits::MaxSegmenthLength;
    }
    else
    {
        AppendSize(tmpba, (WriteData.size() - SegEnd));
        tmpba += WriteData.right(WriteData.size() - SegEnd);
        SegEnd = WriteData.size();
        WriteData.clear();
    }
    WriteDataToPort(tmpba);
}

void EProtocom::AppendSize(QByteArray &ba, int size)
{
    char byte = static_cast<char>(size % 0x100);
    ba.append(byte);
    byte = static_cast<char>(size / 0x100);
    ba.append(byte);
}

void EProtocom::SendOk(bool cont)
{
    QByteArray tmpba;
    if (cont)
        tmpba.append(CN::Message::Continue);
    else
        tmpba.append(CN::Message::Start);
    tmpba.append(Command);
    AppendSize(tmpba, 0);
    // отправляем "ОК" и переходим к следующему сегменту
    WriteDataToPort(tmpba);
}

void EProtocom::SendErr()
{
    QByteArray tmpba;
    tmpba.append(CN::Message::Start);
    tmpba.append(CN::ResultError);
    AppendSize(tmpba, 1);
    // модулю не нужны коды ошибок
    tmpba.append(char(CN::NullByte));
    WriteDataToPort(tmpba);
}

bool EProtocom::GetLength()
{
    if (ReadDataChunk.at(1) != Command)
        return false;
    ReadDataChunkLength = static_cast<quint8>(ReadDataChunk.at(2));
    // посчитали длину посылки
    ReadDataChunkLength += static_cast<quint8>(ReadDataChunk.at(3)) * 256;
    return true;
}

void EProtocom::ParseIncomeData(QByteArray ba)
{
    if (isWriteUSBLog())
    {
        QByteArray tmps = "<-" + ba.toHex() + "\n";
        Log->WriteRaw(tmps);
    }
    if (Command == CN::Unknown) // игнорирование вызова процедуры, если не было
                                // послано никакой команды
    {
        ERMSG("Игнорирование вызова процедуры, если не было послано никакой команды");
        return;
    }
    ReadDataChunk.append(ba);
    qint64 rdsize = ReadDataChunk.size();
    if (rdsize < 4) // ждём, пока принятый буфер не будет хотя бы длиной 3 байта
                    // или не произойдёт таймаут
        return;
    if (ReadDataChunk.at(0) != CN::Message::Module)
    {
        ERMSG("Ошибка при приеме данных");
        Finish(Error::Msg::ReadError);
        return;
    }
    if (static_cast<unsigned char>(ReadDataChunk.at(1)) == CN::ResultError)
    {
        if (rdsize < 5) // некорректная посылка
            Finish(Error::Msg::ReadError);
        else
            // Finish(Error::Msg::USO_NOERR + ReadDataChunk.at(4));
            Finish(Error::Msg::NoError);
        return;
    }
    switch (bStep)
    {
    case 0: // первая порция
    {
        switch (Command)
        {
        // команды с ответом "ОК"
        case CN::Write::Hardware:
        case CN::Write::EraseTech:
        case CN::Write::EraseCnt:
        case CN::Write::Variant:
        case CN::Write::Mode:
        case CN::Write::Time:
        case CN::Write::BlkCmd:
        case CN::Write::Upgrade:
        case CN::Test:
        {
            if ((ReadDataChunk.at(1) != CN::ResultOk) || (ReadDataChunk.at(2) != 0x00) || (ReadDataChunk.at(3) != 0x00))
            {
                ERMSG("Ошибка при приеме данных");
                Finish(Error::Msg::ReadError);
                return;
            }
            if (Command == CN::Write::EraseTech)
                OscTimer->start(); // start timer to send ErPg command periodically
            Finish(Error::Msg::NoError);
            INFOMSG("Переход в тестовый режим без ошибок");
            return;
        }
        // команды с ответом "ОК" и с продолжением
        case CN::Write::File:
        case CN::Write::BlkAC:
        case CN::Write::BlkTech:
        case CN::Write::BlkData:
        {
            if ((ReadDataChunk.at(1) != CN::ResultOk) || (ReadDataChunk.at(2) != 0x00) || (ReadDataChunk.at(3) != 0x00))
            {
                ERMSG("Ошибка при приеме данных");
                Finish(Error::Msg::ReadError);
                return;
            }
            if (!SegLeft)
            {
                Finish(Error::Msg::NoError);
                return;
            }
            ReadDataChunk.clear();
            WRCheckForNextSegment(false);
            return;
        }
        // команды с ответом SS c L L ... и продолжением
        case CN::Read::BlkStartInfo:
        case CN::Read::BlkDataA:
        case CN::Read::BlkAC:
        case CN::Read::BlkData:
        case CN::Read::Progress:
        case CN::Read::BlkTech:
        case CN::Read::Variant:
        case CN::Read::Mode:
        case CN::Read::Time:
        {
            if (!GetLength())
            {
                ERMSG("Несовпадение длины");
                Finish(Error::Msg::ReadError);
                return;
            }
            if (ReadDataChunkLength == 0)
            {
                Finish(Error::Msg::NoError);
                return;
            }

            bStep++;
            break;
        }
        case CN::Read::File:
        {
            if (!GetLength())
            {
                ERMSG("Несовпадение длины");
                Finish(Error::Msg::ReadError);
                return;
            }
            if (ReadDataChunkLength == 0)
            {
                Finish(Error::Msg::NoError);
                return;
            }
            // надо проверить, тот ли номер файла принимаем
            if (rdsize < 20) // не пришла ещё шапка файла
            {
                ERMSG("Не пришла ещё шапка файла");
                return;
            }

            // шапка:
            // WORD fname;		// имя файла
            // WORD service;	// сервисное слово (по умолчанию 0xFF)
            // DWORD size;		// размер файла (без заголовка)
            // DWORD crc32;     // контрольная сумма
            // DWORD thetime;	// время создания файла

            quint8 tmpi = ReadDataChunk[5];
            quint16 filenum = quint16(tmpi * 256);
            tmpi = ReadDataChunk[4];
            filenum += tmpi;
            if (filenum != FNum)
            {
                Finish(Error::Msg::WrongFileError);
                return;
            }
            // вытаскиваем размер файла
            memcpy(&RDLength, &(ReadDataChunk.data()[8]), sizeof(quint32));
            RDLength += 16;
            // размер файла должен быть не более 16М
            if (RDLength > CN::Limits::MaxGetFileSize)
            {
                ERMSG("Некорректная длина блока");
                Finish(Error::Msg::SizeError);
                return;
            }
            //                OutData.resize(RDLength);
            bStep++;
            break;
        }

        default:
            Finish(Error::Msg::WrongCommandError);
            break;
        }
    }

        [[fallthrough]];
    case 1:
    {
        if (!GetLength())
        {
            Finish(Error::Msg::ReadError);
            return;
        }
        // пока не набрали целый буфер соответственно присланной длине или не
        // произошёл таймаут
        if (rdsize < ReadDataChunkLength)
            return;
        // убираем заголовок с < и длиной
        ReadDataChunk.remove(0, 4);
        switch (Command)
        {
        case CN::Read::BlkStartInfo:
        case CN::Read::BlkDataA:
        case CN::Read::BlkAC:
        case CN::Read::BlkData:
        case CN::Read::BlkTech:
        case CN::Read::Variant:
        case CN::Read::Mode:
        case CN::Read::Time:
        {
            // команды с чтением определённого InDataSize количества байт из
            // устройства
            ReadDataChunk.truncate(ReadDataChunkLength);
            OutData.append(ReadDataChunk);
            quint32 outdatasize = OutData.size();
            // сигнал для прогрессбара
            ReadDataChunk.clear();
            if ((outdatasize >= InDataSize) || (ReadDataChunkLength < CN::Limits::MaxSegmenthLength))
            {

                Finish(Error::Msg::NoError);
            }
            else
                SendOk(true);
            break;
        }
        case CN::Read::File:
        {
            // чтение файла количеством байт RDLength = (sizeof(FileHeader) + size)
            quint32 outdatasize = OutData.size();
            quint32 tmpi = outdatasize + ReadDataChunkLength;
            // проверка на выход за диапазон
            if (tmpi > RDLength)
                // копируем только требуемое количество байт
                ReadDataChunkLength = RDLength - outdatasize;
            ReadDataChunk.truncate(ReadDataChunkLength);
            OutData.append(ReadDataChunk);
            outdatasize += ReadDataChunkLength;

            ReadDataChunk.clear();
            if (outdatasize >= RDLength)
            {
                Finish(Error::Msg::NoError);
                return;
            }
            else
                SendOk(true);
            break;
        }
        case CN::Read::Progress:
        {
            quint16 OscNum = static_cast<quint8>(ReadDataChunk.at(0)) + static_cast<quint8>(ReadDataChunk.at(1)) * 256;
            if (OscNum == 100)
            {
                Finish(Error::Msg::NoError);
                OscTimer->stop();
                break;
            }
            break;
        }

        default:
        {
            Finish(Error::Msg::WrongCommandError);
            break;
        }
        }
    }
    }
}

void EProtocom::CheckForData()
{
    QByteArray ba = RawRead(1000);
    ParseIncomeData(ba);
}

void EProtocom::OscTimerTimeout()
{
    SendCmd(CN::Read::Progress);
}

void EProtocom::SendCmd(unsigned char command, int parameter)
{
    // only for these commands
    switch (command)
    {
    case CN::Read::Progress:
    case CN::Write::Upgrade:
    case CN::Write::BlkCmd:
    case CN::Test:
    case CN::Write::Variant:
    case CN::Write::Mode:
    case CN::Write::EraseTech:
        break;
    default:
        Finish(Error::Msg::WrongCommandError);
        ERMSG("Неизвестная команда");
        return;
    }
    QByteArray ba;
    Send(command, parameter, ba, 0);
}

void EProtocom::SendIn(unsigned char command, char parameter, QByteArray &ba, qint64 maxdatasize)
{
    // only for these commands
    switch (command)
    {
    case CN::Read::BlkStartInfo:
    case CN::Read::BlkAC:
    case CN::Read::BlkDataA:
    case CN::Read::BlkData:
    case CN::Read::BlkTech:
    case CN::Read::Time:
    case CN::Read::Variant:
    case CN::Read::Mode:
        break;
    default:
        ERMSG("Неизвестная команда");
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    Send(command, parameter, ba, maxdatasize);
    ba = OutData;
}

void EProtocom::SendOut(unsigned char command, char board_type, QByteArray &ba)
{
    // only for these commands
    switch (command)
    {
    case CN::Write::Hardware:
    case CN::Write::BlkAC:
    case CN::Write::Time:
    case CN::Write::BlkData:
    case CN::Write::BlkTech:
    case CN::Write::EraseCnt:
        break;
    default:
        ERMSG("Неизвестная команда");
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    Send(command, board_type, ba, 0);
}

void EProtocom::SendFile(unsigned char command, char board_type, int filenum, QByteArray &ba)
{
    // only for these commands
    switch (command)
    {
    case CN::Read::File:
    case CN::Write::File:
        break;
    default:
        ERMSG("Неизвестная команда");
        Finish(Error::Msg::WrongCommandError);
        return;
    }
    FNum = filenum;
    Send(command, board_type, ba, 0);
    ba = OutData;
}

void EProtocom::usbStateChanged(void *message)
{
#ifdef _WIN32
    MSG *msg = static_cast<MSG *>(message);
    int msgType = msg->message;
    if (msgType == WM_DEVICECHANGE)
    {
        msgType = msg->wParam;
        switch (msgType)
        {
        case DBT_CONFIGCHANGECANCELED:
            qDebug("DBT_CONFIGCHANGECANCELED");
            break;
        case DBT_CONFIGCHANGED:
            qDebug("DBT_CONFIGCHANGED");
            break;
        case DBT_CUSTOMEVENT:
            qDebug("DBT_CUSTOMEVENT");
            break;
        case DBT_DEVICEARRIVAL:
        {
            DevicesFound();
            if (Board::GetInstance().connectionState() == Board::ConnectionState::AboutToFinish)
            {
                if (m_devices.contains(m_usbWorker->deviceInfo()))
                {
                    m_devicePosition = m_devices.indexOf(m_usbWorker->deviceInfo());
                    m_usbWorker->setDeviceInfo(m_devices.at(m_devicePosition));
                    qDebug("Device arrived again");
                    if (!Reconnect())
                    {
                        qDebug("Reconnection failed");
                        Disconnect();
                    }
                }
            }
            break;
        }
        case DBT_DEVICEQUERYREMOVE:
            qDebug("DBT_DEVICEQUERYREMOVE");
            break;
        case DBT_DEVICEQUERYREMOVEFAILED:
            qDebug("DBT_DEVICEQUERYREMOVEFAILED");
            break;
        case DBT_DEVICEREMOVEPENDING:
            qDebug("DBT_DEVICEREMOVEPENDING");
            break;
        case DBT_DEVICEREMOVECOMPLETE:
        {
            qDebug("DBT_DEVICEREMOVECOMPLETE");
            qDebug() << DevicesFound();
            if (Board::GetInstance().connectionState() != Board::ConnectionState::Closed)
            {
                if (!m_devices.contains(m_usbWorker->deviceInfo()))
                {
                    qDebug() << "Device " << m_usbWorker->deviceInfo().serial << " removed completely";
                    WriteData.clear();
                    OutData.clear();
                    Finish(Error::Msg::NullDataError);
                    m_loop.exit();
                    QMessageBox::critical(nullptr, "Ошибка", "Связь с прибором была разорвана", QMessageBox::Ok);
                }
            }
            break;
        }
        case DBT_DEVICETYPESPECIFIC:
            qDebug("DBT_DEVICETYPESPECIFIC");
            break;
        case DBT_QUERYCHANGECONFIG:
            qDebug("DBT_QUERYCHANGECONFIG");
            break;
        case DBT_DEVNODES_CHANGED:
        {
            qDebug("DBT_DEVNODES_CHANGED");
            DevicesFound();

            // Ивенты должны происходить только если отключен подключенный раннее
            // прибор
            if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected)
            {
                if (!m_devices.contains(m_usbWorker->deviceInfo()))
                {
                    qDebug() << "Device " << m_usbWorker->deviceInfo().serial << " state changed";
                    Board::GetInstance().setConnectionState(Board::ConnectionState::AboutToFinish);
                }
            }
            break;
        }
        case DBT_USERDEFINED:
            qDebug("DBT_USERDEFINED");
            break;
        default:
            qDebug() << "Default";
            break;
        }
    }
#endif
}

EProtocom::~EProtocom()
{

    Log->deleteLater();
    OscTimer->deleteLater();
    m_waitTimer->deleteLater();
}

bool EProtocom::start(const ConnectStruct &st)
{
    if (Board::GetInstance().connectionState() == Board::ConnectionState::Connected
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
        Disconnect();
    m_usbWorker = new EUsbWorker(m_devices.at(st.usbDevicePosition), Log, isWriteUSBLog());
    m_devicePosition = st.usbDevicePosition;
    QThread *workerThread = new QThread;
    m_usbWorker->moveToThread(workerThread);

    connect(workerThread, &QThread::started, m_usbWorker, &EUsbWorker::interact);
    connect(workerThread, &QThread::started, [this] { m_workerStatus = true; });
    connect(workerThread, &QThread::finished, [this] { m_workerStatus = false; });
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(m_usbWorker, &EUsbWorker::finished, workerThread, &QThread::quit);
    connect(m_usbWorker, &EUsbWorker::finished, m_usbWorker, &EUsbWorker::deleteLater);
    connect(m_usbWorker, &EUsbWorker::NewDataReceived, this, &EProtocom::ParseIncomeData);

    //    QThread *parserThread = new QThread;

    //    connect(parserThread, &QThread::started, [this] { m_parserStatus = true; });
    //    connect(parserThread, &QThread::finished, [this] { m_parserStatus = false; });
    //    connect(parserThread, &QThread::finished, parserThread, &QThread::deleteLater);

    if (m_usbWorker->setupConnection() == Error::Msg::NoError
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
        workerThread->start();
    }
    else
        return false;
    return true;
}

void EProtocom::reqTime()
{
    CommandStruct inp { CN::Commands::ReadTime, 0, 0, {} };
    DataManager::addToInQueue(inp);
}

void EProtocom::reqFile(quint32 filenum)
{
    CommandStruct inp {
        CN::Commands::ReadFile, // Command
        filenum,                // File number
        0,                      // Empty float arg
        {}                      // Empty QByteArray
    };
    DataManager::addToInQueue(inp);
}

void EProtocom::reqStartup(quint32 sigAdr, quint32 sigCount)
{
}

void EProtocom::reqBSI()
{
    CommandStruct inp {
        CN::Commands::ReadBlkStartInfo,                       // Command
        BoardTypes::BT_NONE,                                  // Board type
        0,                                                    // Empty float arg
        QByteArray(sizeof(ModuleBSI::Bsi), Qt::Uninitialized) // Buffer for bsi
    };
    DataManager::addToInQueue(inp);
}

void EProtocom::writeFile(quint32 filenum, const QByteArray &file)
{
    CommandStruct inp {
        CN::Commands::WriteFile, // Command
        filenum,                 // File number
        0,                       // Empty float arg
        file                     // Buffer with file
    };
    DataManager::addToInQueue(inp);
}

void EProtocom::writeTime(quint32 time)
{
    CommandStruct inp { CN::Commands::WriteTime, time, 0, {} };
    DataManager::addToInQueue(inp);
}

void EProtocom::reqFloats(quint32 sigAdr, quint32 sigCount)
{
    // TODO CN::Commands::Unknown
    CommandStruct inp { CN::Commands::Unknown, sigAdr, sigCount, {} };
    DataManager::addToInQueue(inp);
}

void EProtocom::writeCommand(Queries::Commands cmd, QList<DataTypes::SignalsStruct> list)
{
    CommandStruct inp;
    switch (cmd)
    {
    default:
    {
        {
            inp = { translate(cmd), 0, 0, QByteArray {} };
        }
    }
    }
    DataManager::addToInQueue(inp);
}

bool EProtocom::Reconnect()
{
    m_usbWorker->closeConnection();
    if (m_usbWorker->setupConnection() == Error::Msg::NoError
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        Board::GetInstance().setConnectionState(Board::ConnectionState::Connected);
    }
    else
        return false;
    return true;
}

void EProtocom::Disconnect()
{
    qDebug(__PRETTY_FUNCTION__);
    RawClose();
    Log->WriteRaw("Disconnected!\n");
}

QByteArray EProtocom::RawRead(int bytes)
{
    Q_UNUSED(bytes)
    return QByteArray();
}

int EProtocom::RawWrite(QByteArray &ba)
{
    return m_usbWorker->WriteDataAttempt(ba);
}

void EProtocom::RawClose()
{
    if (Board::GetInstance().connectionState() != Board::ConnectionState::Closed
        && Board::GetInstance().interfaceType() == Board::InterfaceType::USB)
    {
        Board::GetInstance().setConnectionState(Board::ConnectionState::Closed);
    }
}

CN::Commands EProtocom::translate(const Queries::Commands cmd)
{
    if (m_dict.contains(cmd))
        return m_dict.value(cmd);
    else
        return CN::Commands::Unknown;
}

Queries::Commands EProtocom::translate(const CN::Commands cmd)
{
    // TODO  Реализовать проверку если нет такого value
    return m_dict.key(cmd);
}

QList<QStringList> EProtocom::DevicesFound()
{
    hid_device_info *devs, *cur_dev;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    QList<QStringList> sl;
    m_devices.clear();
    while (cur_dev)
    {

        if (cur_dev->vendor_id == 0xC251)
        {
            const DeviceConnectStruct buffer(cur_dev->vendor_id, cur_dev->product_id,
                QString::fromWCharArray(cur_dev->serial_number), QString(cur_dev->path));

            /*#ifdef __linux__
                        wstr[0] = 0x0000;
                        res = hid_get_serial_number_string(handle, wstr, MAX_STR);
                        if (res < 0)
                            printf("Unable to read serial number string\n");
            #endif
            #ifdef _WIN32

             sn = QString::fromWCharArray(cur_dev->serial_number);
            #endif*/
            QStringList tmps { QString::number(buffer.vendor_id, 16), QString::number(buffer.product_id, 16),
                buffer.serial, buffer.path };
            sl.append(tmps);
            m_devices.push_back(buffer);
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    return sl;
}

EUsbWorker *EProtocom::usbWorker() const
{
    return m_usbWorker;
}
