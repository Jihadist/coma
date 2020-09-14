
#include "warnalarmkiv.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/modulebsi.h"
#include "../usb/commands.h"
#include "../widgets/wd_func.h"
#include "abstractalarm.h"

#include <QBoxLayout>

WarnAlarmKIV::WarnAlarmKIV(AlarmClass *alarm, QDialog *parent) : AbstractAlarm(parent)
{
    Alarm = alarm;
    WarnAlarmState();
}

void WarnAlarmKIV::WarnAlarmState()
{
    // QDialog *dlg = new QDialog;

    const QStringList events = QStringList() << "Отсутствует сигнал напряжения фазы A                   "
                                             << "Отсутствует сигнал напряжения фазы B                   "
                                             << "Отсутствует сигнал напряжения фазы С                   "
                                             << "Отсутствует ток ввода фазы А (ток меньше 2мА)          "
                                             << "Отсутствует ток ввода фазы B (ток меньше 2мА)          "
                                             << "Отсутствует ток ввода фазы C (ток меньше 2мА)          "
                                             << "Не заданы начальные значения                           "
                                             << "Низкое напряжение фазы A                               "
                                             << "Низкое напряжение фазы B                               "
                                             << "Низкое напряжение фазы C                               "
                                             << "Сигнализация по приращению тангенса дельта ввода фазы А"
                                             << "Сигнализация по приращению тангенса дельта ввода фазы B"
                                             << "Сигнализация по приращению тангенса дельта ввода фазы C"
                                             << "Сигнализация по приращению C ввода фазы А              "
                                             << "Сигнализация по приращению C ввода фазы B              "
                                             << "Сигнализация по приращению C ввода фазы C              "
                                             << "Не заданы паспортные значения                          "
                                             << "Сигнализация по повышенному небалансу токов            ";
    QWidget *w = new QWidget;
    w->setStyleSheet("QWidget {margin: 0; border-width: 0; padding: 0;};");

    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    for (int i = 0; i < Alarm->MapAlarm[MTYPE_KIV].warnCounts; ++i)
    {
        QHBoxLayout *hlyout = new QHBoxLayout;
        hlyout->addWidget(WDFunc::NewLBL(w, "", "transparent", QString::number(i)));
        hlyout->addWidget(WDFunc::NewLBLT(w, events.at(i), "", "", ""), 1);
        vlayout->addLayout(hlyout);
    }

    w->setLayout(vlayout);

    lyout->addWidget(w);
    QPushButton *pb = new QPushButton("Ok");
    connect(pb, SIGNAL(clicked()), this, SLOT(hide()));
    lyout->addWidget(pb, 0);
    setLayout(lyout);
}

void WarnAlarmKIV::AlarmState()
{
}
void WarnAlarmKIV::AvarState()
{
}

void WarnAlarmKIV::Update(QList<bool> states)
{
    int i = 0;
    // 25 оптимальная константа
    float circleRadius = CIRCLE_RADIUS;
    if (states.isEmpty())
        return;
    for (i = 0; i < Alarm->MapAlarm[MTYPE_KIV].warnCounts; i++)
    {
        quint32 alarm = states.at(i);
        if (alarm)
        {
            auto pixmap = WDFunc::NewCircle(Qt::red, circleRadius);
            WDFunc::SetLBLImage(this, (QString::number(i)), &pixmap);
        }
        else
        {
            auto pixmap = WDFunc::NewCircle(Qt::green, circleRadius);
            WDFunc::SetLBLImage(this, (QString::number(i)), &pixmap);
        }
    }
}
