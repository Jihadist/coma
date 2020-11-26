#ifndef WARN_H
#define WARN_H

#include "../widgets/uwidget.h"

#include <QBitArray>
#include <bitset>

#define CIRCLE_RADIUS 15

class Warn : public UWidget
{
    Q_OBJECT
public:
    int m_warnBdNum;
    int m_startWarnAddress;
    std::bitset<32> m_warnFlags; // '1' equals warning
    std::bitset<32> m_actualWarnFlags;

    explicit Warn(QWidget *parent = nullptr);
    int realWarnSize();
    void updatePixmap(bool isset, int position);

public slots:
    //    void Update(std::bitset<32> &states);
    void updateSpData(const DataTypes::SinglePointWithTimeStruct &sp);

signals:
    void updateWarn(bool value);

private:
    int m_realWarnSize;

protected:
    void showEvent(QShowEvent *e);
    void setupUI(const QStringList &events);
};

#endif // WARN_H
