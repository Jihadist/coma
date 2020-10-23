#ifndef DATATYPES_H
#define DATATYPES_H

#include <QVariant>
#include <qmetatype.h>

namespace DataTypes
{
enum SignalTypes
{
    BitString,
    FloatWithTime,
    SinglePointWithTime,
    ByteArray,
    File,
    ConfParameter,
    GeneralResponse
};

enum GeneralResponseTypes
{
    Ok,
    DataSize,
    DataCount
};

struct BitStringStruct
{
    //    quint8 sigAdr[3];
    quint32 sigAdr;
    quint32 sigVal;
    quint8 sigQuality;
}; // первое - номера сигналов, второе - их значения ("" ~ недостоверное
   // значение), третье - метка времени

struct FloatWithTimeStruct
{
    quint32 sigAdr;
    float sigVal;
    quint8 sigQuality;
    quint64 CP56Time;
};

struct FloatStruct
{
    quint32 sigAdr;
    float sigVal;
};

struct SinglePointWithTimeStruct
{
    quint32 sigAdr;
    quint8 sigVal;
    quint64 CP56Time;
};

struct FileStruct
{
    quint32 filenum;
    QByteArray filedata;
};

struct ConfParameterStruct
{
    quint32 ID;
    QByteArray data;
};

struct SignalsStruct
{
    SignalTypes type;
    QVariant data;
};

struct GeneralResponseStruct
{
    GeneralResponseTypes type;
    quint64 data;
};
}

namespace Queries
{
enum Commands
{
    MBS_READHOLDINGREGISTERS,
    MBS_READINPUTREGISTER,
    MBS_WRITEMULTIPLEREGISTERS,
    CM104_REQGROUP,
    CM104_COM51WRITETIME,
    CM104_COM45,
    CM104_COM50,
    //    CM104_CORREADREQUEST,
    CM104_SELECTFILE,
    CM104_FILEREADY
};

struct Command
{
    Commands cmd;
    quint32 uintarg;
    float flarg;
    QByteArray ba;
};
}

namespace S2DataTypes
{
// S2: Определение типа заголовка
typedef struct
{
    quint16 fname;
    quint16 service;
    quint32 size;
    quint32 crc32;
    quint32 thetime;
} FileHeader;

// S2: Определение типа записи

typedef struct DataRec
{
    quint32 id;
    quint32 num_byte;
    void *thedata;
} DataRec;
}

Q_DECLARE_METATYPE(DataTypes::BitStringStruct)
Q_DECLARE_METATYPE(DataTypes::FloatWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::SinglePointWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FileStruct)
Q_DECLARE_METATYPE(DataTypes::ConfParameterStruct)
Q_DECLARE_METATYPE(DataTypes::GeneralResponseStruct)
Q_DECLARE_METATYPE(Queries::Command)
#endif // DATATYPES_H
