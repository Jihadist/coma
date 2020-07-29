#ifndef JOURNALDIALOG_H
#define JOURNALDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QThread>

#include "../gen/maindef.h"
#include "../widgets/etableview.h"
#include "../models/etablemodel.h"
#include "../gen/s2.h"
#include "../dialogs/keypressdialog.h"
#include "../gen/journals.h"
#include "../widgets/waitwidget.h"
#include "../iec104/iec104.h"

#define MAXSWJNUM   262144

#define WORKJOURIDKTF  5011
class JournalDialog : public QDialog
{
    Q_OBJECT
public:
    JournalDialog(IEC104 *iec, QWidget *parent = nullptr);
    ~JournalDialog();

    quint8 start;
    int SaveI;

private:
    WaitWidget *WW;
    QThread JourThread;

    const QStringList WorkJourDescriptionKTF = QStringList() << "Отсутствует сигнал напряжения фазы A"
                                            << "Отсутствует сигнал напряжения фазы B"
                                            << "Отсутствует сигнал напряжения фазы С"
                                            << "Отсутствует сигнал тока фазы A"
                                            << "Отсутствует сигнал тока фазы B"
                                            << "Отсутствует сигнал тока фазы С"
                                            << "Перегрузка по току фазы A"
                                            << "Перегрузка по току фазы B"
                                            << "Перегрузка по току фазы C"
                                            << "Машина включена"
                                            << "Сигнализация по опасному превышению температуры обмотки"
                                            << "Неисправны все датчики температуры об-мотки"
                                            << "Аварийное сообщение по недопустимому превышению температуры обмотки"
                                            << "Сигнализация по опасному уровню пускового тока"
                                            << "Аварийное сообщение по недопустимому уровню пускового тока";

    const QStringList TitleList = QStringList()
                                            << "Ueff фA"
                                            << "Ueff фB"
                                            << "Ueff фC"
                                            << "Ieff фA"
                                            << "Ieff фB"
                                            << "Ieff фC"
                                            << "Freq"
                                            << "U0"
                                            << "U1"
                                            << "U2"
                                            << "I0"
                                            << "I1"
                                            << "I2"
                                            << "Cbush фA"
                                            << "Cbush фB"
                                            << "Cbush фC"
                                            << "Tg_d фA"
                                            << "Tg_d фB"
                                            << "Tg_d фC"
                                            << "dCbush фA"
                                            << "dCbush фB"
                                            << "dCbush фC"
                                            << "dTg_d фA"
                                            << "dTg_d фB"
                                            << "dTg_d фC"
                                            << "Iunb"
                                            << "Phy_unb"
                                            << "Tmk, °С"
                                            << "Tamb, °С" ;

    const QStringList TitleListKTF = QStringList()

                                            << "Ueff фA"
                                            << "Ueff фB"
                                            << "Ueff фC"
                                            << "Ieff фA"
                                            << "Ieff фB"
                                            << "Ieff фC"
                                            << "Frequency"
                                            << "U0"
                                            << "U1"
                                            << "U2"
                                            << "I0"
                                            << "I1"
                                            << "I2"
                                            << "Pf фA"
                                            << "Pf фB"
                                            << "Pf фC"
                                            << "Pf сумм."
                                            << "Qf фA"
                                            << "Qf фB"
                                            << "Qf фC"
                                            << "Qf сумм."
                                            << "Sf фA"
                                            << "Sf фB"
                                            << "Sf фC"
                                            << "Sf сумм."
                                            << "CosPhi"
                                            << "Tmk,°С"
                                            << "Tamb,°С"
                                            << "Twin,°С";


    void SetupUI();
    QWidget *JourTab(int jourtype);
    int GetJourNum(const QString &objname);

signals:
    void WritePasswordChecked();
    void StartGetJour(); // start fet journal thread
    void StartReadFile();
    void StartSaveJour(int, QAbstractItemModel *, QString);

private slots:
    void TryGetJourByUSB();
    void GetJour();
    void JourFileChoosed(QString &file);
    void EraseJour();
    void SaveJour();
    void WritePasswordCheck(QString psw);
    void Done(QString msg);
    void Error(QString msg);
    void SetModel(ETableModel *mdl);
//    void SetProxyModel(QSortFilterProxyModel *mdl);

public slots:

private:
    QString JourFile;
    int JourType;
    bool ok;
    Journals *JourFuncs;
    QSortFilterProxyModel *ProxyWorkModel, *ProxySysModel, *ProxyMeasModel;

    int WriteCheckPassword();
    void StartReadJourFile();
};

#endif // JOURNALDIALOG_H
