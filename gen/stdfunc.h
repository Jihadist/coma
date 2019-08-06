#ifndef STDFUNC_H
#define STDFUNC_H

#include <QStringList>
#include <QPointer>
#include <QVector>
#include <QCoreApplication>
#include "maindef.h"

class StdFunc
{
public:
    StdFunc();

    static QString s_MIPIP;
    static QString s_OrganizationString;
    static bool Cancelled;

    static void Init();
    static QString VerToStr(quint32);
    static bool FloatInRange(float var, float value, float tolerance=TH01);
    static void SetHomeDir(const QString &dir);
    static QString GetHomeDir();
    static QString GetSystemHomeDir();
    static void SetMIPIP(const QString &ip);
    static QString MIPIP();
    static void SetOrganizationString(const QString &str);
    static QString OrganizationString();
    static void Cancel();
    static void ClearCancel();
    static bool IsCancelled();
    static bool IsInEmulateMode();
    static void SetEmulated(bool tb);
    static int IndexByBit(quint32 dword); // возвращение номера первого, начиная с младшего, установленного бита, нумерация с 1, dword=0 => return 0
    static quint32 BitByIndex(int idx); // возвращение битовой маски по индексу (0 => 0, 1 => 1, 2 => 2, 3 => 4, ...)
    static QString PrbMessage();
    static void SetPrbMessage(const QString &msg);

private:
    static QString HomeDir; // рабочий каталог программы
    static QString SystemHomeDir; // системный каталог программы
    static bool Emul;
    static QString PrbMsg;
};

#endif // STDFUNC_H
