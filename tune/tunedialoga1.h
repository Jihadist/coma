#ifndef TUNEDIALOGA1_H
#define TUNEDIALOGA1_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "eabstracttunedialog.h"
#include "../config/configa1.h"
#include "../check/checka1.h"
#include "../gen/maindef.h"
#include "../widgets/waitwidget.h"

// определения для процедуры проверки CheckBdaValues
#define CHECK_VOLT  1
#define CHECK_PT100 2
#define CHECK_MA1   3
#define CHECK_MA2   4

#define TDA1_BDA    0x01
#define TDA1_BD1    0x02
#define TDA1_BD4    0x04

class TuneDialogA1 : public EAbstractTuneDialog
{
    Q_OBJECT
public:
    explicit TuneDialogA1(QWidget *parent = nullptr);

signals:
    void StartPercents(int Percent);
    void SetPercent(int Percent);
    void Degrees50Completed();
    void Degrees0Completed();

public slots:

private:
    ConfigA1 *CA1;
    CheckA1 *ChA1;

    struct Bac
    {
        float KmU[2];		// калибровочные коэффициенты по напряжению в 2 каналах
        float K_freq;		// коррекция частоты
        float DPhy;         // коррекция фазы в приборе (в градусах)
        float Art;          // коэффициент в канале Pt100, ед.АЦП/Ом
        float Brt;          // смещение в канале Pt100, ед.АЦП
        float Ama1; 		// коэффициент в канале A1, ед./мА
        float Bma1; 		// смещение в канале A1, ед.
        float Ama2; 		// коэффициент в канале A2, ед./мА
        float Bma2; 		// смещение в канале A2, ед.
        float TKUa[2];      // температурные коэффициенты линейной коррекции изменения KmU [1/градус]
        float TKUb[2];      // температурные коэффициенты квадратичной коррекции изменения KmU [1/градус²]
        float Tmk0;         // начальная температура в микроконтроллера при калибровке
        float ch_offset[2]; // смещения в каналах (в кодах АЦП)
    };

    Bac Bac_block, Bac_block_old;

    struct EMData
    {
        float u;
        float freq;
        float t;
    };

    EMData RealData;
    float RegData;
//    int result;
    struct TKUSourceDataStruct
    {
        CheckA1::A1_Bd1 Bda_in[3];
        CheckA1::A1_Bd4 Bda_out_an[3];
        float Tmk[3];
    };
    TKUSourceDataStruct TKUSourceData;
    int PovNumPoints;

    void SetupUI();

#if PROGSIZE != PROGSIZE_EMUL
    void SetLbls();
    void SetPf();
    int Start6_3_2();
    int Start6_3_3_1();
    int Start6_3_3_2();
    int Start6_3_3_3();
    int Start6_3_3_4();
    int Start6_3_3_5();
    int Start6_3_3_6();
    int Start6_3_3_7();
    int Start6_3_4_1();
    int Start6_3_4_3();
    int Start6_3_4_4();
    int Start6_3_5();
    int Start6_3_6_1();
    int Start6_3_6_2();
    int Start6_3_6_3();
    int Start6_3_7();
    int Start6_3_8_1();
    int Start6_3_8_2();
    int Start6_3_8_3();
    int Start6_3_9();
    int Start6_3_10_1();
    int Start6_3_10_2();
    int Start6_3_10_3();
    int Start6_3_10_60(int index); // вспомогательная функция для приёма 60 точек и усреднения
    int Start6_3_11();
    int Start6_3_12();

    int Start60PointsMeasurements(int whichtomeasure, void *dst1, void *dst2=nullptr, void *dst3=nullptr); // whichtomeasure - by mask, dst[i] - dest buffers
    int MeasureBd1(CheckA1::A1_Bd1 *result);
    int MeasureBd4(CheckA1::A1_Bd4 *result);
    int MeasureBda(CheckA1::Bda *result);
    void Bd1Mean(int count, CheckA1::A1_Bd1 *result);
    void Bd4Mean(int count, CheckA1::A1_Bd4 *result);
    void BdaMean(int count, CheckA1::Bda *result);
    int ShowScheme();
    int CheckBdaValues(int checktype);
    int CheckAnalogValues(bool isPrecise); // проверка Bda_in на корректность
    int GetExternalData(); // ввод данных в зависимости от выбранного режима и номера опыта
    int GetExternalTemp(); // ввод текущей температуры окр. среды для настройки на термокамеру
    void GetBdAndFillMTT();
    void LoadSettings();
#endif

private slots:
#if PROGSIZE != PROGSIZE_EMUL
    int ReadAnalogMeasurements();
    void SetExtData();
    void CancelExtData();
    void SetExtTemp();
    void CancelExtTemp();
    void CancelExt(const QString &dlgname);
#endif
    void FillBac();
    void FillBackBac();
    void SetDefCoefs();
};

#endif // TUNEDIALOGA1_H
