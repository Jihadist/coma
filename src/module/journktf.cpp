#include "journktf.h"

#include "../gen/timefunc.h"
#include "modules.h"
JournKTF::JournKTF(QMap<Modules::JournalType, DataTypes::JournalDesc> &jourMap, QObject *parent)
    : Journals(jourMap, parent)
{
}

QVector<QVariant> JournKTF::createMeasRecord(const char *file)
{
    AVM_KTF::MeasureStruct ktfdata;
    memcpy(&ktfdata, file, sizeof(AVM_KTF::MeasureStruct));
    if (ktfdata.Time == 0xFFFFFFFF)
        return QVector<QVariant>();
    QVector<QVariant> outvl { ktfdata.NUM, TimeFunc::UnixTime32ToInvString(ktfdata.Time, TimeFunc::userTimeZone()),
        ktfdata.Ueff[0], ktfdata.Ueff[1], ktfdata.Ueff[2], ktfdata.Ieff[0], ktfdata.Ieff[1], ktfdata.Ieff[2],
        ktfdata.Frequency, ktfdata.U0, ktfdata.U1, ktfdata.U2, ktfdata.I0, ktfdata.I1, ktfdata.I2, ktfdata.Pf[0],
        ktfdata.Pf[1], ktfdata.Pf[2], ktfdata.Pf[3], ktfdata.Qf[0], ktfdata.Qf[1], ktfdata.Qf[2], ktfdata.Qf[3],
        ktfdata.Sf[0], ktfdata.Sf[1], ktfdata.Sf[2], ktfdata.Sf[3], ktfdata.Cosphi, ktfdata.Tmk, ktfdata.Tamb,
        ktfdata.Twin };
    return outvl;
}

int JournKTF::measureSize()
{
    return sizeof(AVM_KTF::MeasureStruct);
}
