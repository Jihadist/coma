#pragma once
#define ID8084_START 1012
#define ID8084_COM 20
namespace AVM_KIV
{

const QStringList accidentEvents {
    "Авария по приращению тангенса дельта ввода фазы А", //
    "Авария по приращению тангенса дельта ввода фазы B", //
    "Авария по приращению тангенса дельта ввода фазы C", //
    "Авария по приращению C ввода фазы А              ", //
    "Авария по приращению C ввода фазы B              ", //
    "Авария по приращению C ввода фазы C              ", //
    "Авария по недопустимому небалансу токов          "  //
};
constexpr quint32 accidentFlags = 0x00000e39;
const quint32 accidentCount = accidentEvents.size();
const QStringList warnEvents {
    "Отсутствует сигнал напряжения фазы A                   ", //
    "Отсутствует сигнал напряжения фазы B                   ", //
    "Отсутствует сигнал напряжения фазы С                   ", //
    "Ток утечки ввода фазы A меньше 0,3 номинального",         //
    "Ток утечки ввода фазы B меньше 0,3 номинального",         //
    "Ток утечки ввода фазы C меньше 0,3 номинального",         //
    "Не заданы начальные значения                           ", //
    "Низкое напряжение фазы A                               ", //
    "Низкое напряжение фазы B                               ", //
    "Низкое напряжение фазы C                               ", //
    "Сигнализация по приращению тангенса дельта ввода фазы А", //
    "Сигнализация по приращению тангенса дельта ввода фазы B", //
    "Сигнализация по приращению тангенса дельта ввода фазы C", //
    "Сигнализация по приращению C ввода фазы А              ", //
    "Сигнализация по приращению C ввода фазы B              ", //
    "Сигнализация по приращению C ввода фазы C              ", //
    "Не заданы паспортные значения                          ", //
    "Сигнализация по повышенному небалансу токов            "  //
};
constexpr quint32 warnFlags = 0x1fff1ec6;
constexpr quint32 workJourId = 3010;
struct MeasureStruct
{
    quint32 NUM;
    quint32 Time;
    float Ueff[3];
    float Ieff[3];
    float Frequency;
    float U0;
    float U1;
    float U2;
    float I0;
    float I1;
    float I2;
    float Cbush[3];
    float Tg_d[3];
    float dCbush[3];
    float dTg_d[3];
    float Iunb;
    float Phy_unb;
    float Tmk;
    float Tamb;
    quint32 res;
};
const QStringList workJourDescription {
    "Отсутствует сигнал напряжения фазы A",                    //
    "Отсутствует сигнал напряжения фазы B",                    //
    "Отсутствует сигнал напряжения фазы С",                    //
    "Нет реактивного тока канала А (ток меньше 2мА)",          //
    "Нет реактивного тока канала B (ток меньше 2мА)",          //
    "Нет реактивного тока канала C (ток меньше 2мА)",          //
    "Не заданы начальные значения",                            //
    "Низкое напряжение фазы A",                                //
    "Низкое напряжение фазы B",                                //
    "Низкое напряжение фазы C",                                //
    "Сигнализация по приращению тангенса дельта ввода фазы А", //
    "Сигнализация по приращению тангенса дельта ввода фазы B", //
    "Сигнализация по приращению тангенса дельта ввода фазы C", //
    "Авария по приращению тангенса дельта ввода фазы А",       //
    "Авария по приращению тангенса дельта ввода фазы B",       //
    "Авария по приращению тангенса дельта ввода фазы C",       //
    "Сигнализация по приращению C ввода фазы А",               //
    "Сигнализация по приращению C ввода фазы B",               //
    "Сигнализация по приращению C ввода фазы C",               //
    "Авария по приращению C ввода фазы А",                     //
    "Авария по приращению C ввода фазы B",                     //
    "Авария по приращению C ввода фазы C",                     //
    "Не заданы паспортные значения",                           //
    "Сигнализация по изменению небаланса тока",                //
    "Авария по изменению небаланса тока"                       //
};
const QStringList measJourHeaders {
    " № ",            //
    "Дата/Время UTC", //
    "Ueff фA",        //
    "Ueff фB",        //
    "Ueff фC",        //
    "Ieff фA",        //
    "Ieff фB",        //
    "Ieff фC",        //
    "Freq",           //
    "U0",             //
    "U1",             //
    "U2",             //
    "I0",             //
    "I1",             //
    "I2",             //
    "Cbush фA",       //
    "Cbush фB",       //
    "Cbush фC",       //
    "Tg_d фA",        //
    "Tg_d фB",        //
    "Tg_d фC",        //
    "dCbush фA",      //
    "dCbush фB",      //
    "dCbush фC",      //
    "dTg_d фA",       //
    "dTg_d фB",       //
    "dTg_d фC",       //
    "Iunb",           //
    "Phy_unb",        //
    "Tmk, °С",        //
    "Tamb, °С"        //
};
struct Bci
{
    //Номинальное линейное первичное напряжение в кВ
    float Unom;
    //Уставка контроля минимума напряжения(в % от номинального)
    float Umin;
    //Уставка контроля минимума тока (в % от Imax)
    float Imin;
    //Начальные значени емкостей вводов
    float C_init[3];
    //Начальные значения tgδ вводов
    float Tg_init[3];
    // коррекция  tg δ вводов
    float corTg[3];
    // уставка предупредительной сигнализации по изменению емкости
    float dC_pred;
    // уставка аварийной сигнализации по изменению емкости
    float dC_alarm;
    // уставка предупредительной сигнализации по изменению tg δ
    float dTg_pred;
    // уставка аварийной сигнализации по изменению tg δ
    float dTg_alarm;
    // уставка предупредительной сигнализации по изменению небаланса токов
    float dIunb_pred;
    // уставка аварийной сигнализации по изменению небаланса токов
    float dIunb_alarm;
    // гистерезис на отключение сигнализации по dC, % от уставки
    float GdC;
    // гистерезис на отключение сигнализации по dTg, % от уставки
    float GdTg;
    // гистерезис на отключение сигнализации по небалансу токов
    float GdIunb;
    // Сопротивление термометра при 0°С, Ом (только для МНК3)
    float RTerm;
    // Температурный коэффициент термометра (только для МНК3)
    float W100;
    // Начальное действ. значение тока небаланса
    float Iunb_init;
    // Начальное значение угла тока небаланса относительно вектора тока фазы А
    float Phy_unb_init;
    // Интервал усреднения данных  (в периодах основной частоты)
    quint32 NFiltr;
    // Интервал записи данных в ПЗУ (тренд), в секундах
    quint32 T_Data_Rec;
    float LowU;
    quint8 IsU;
    quint8 IsIunb;
    quint32 Tevent_pred;
    quint32 Tevent_alarm;
    float Unom_1;
    float Tg_pasp[3];
    float C_pasp[3];
};
}
