#ifndef CONFDIALOG35_H
#define CONFDIALOG35_H

#include <QDialog>

#include "abstractconfdialog3x.h"
#include "config35.h"

class ConfDialog35 : public AbstractConfDialog3x
{
    Q_OBJECT
public:
    explicit ConfDialog35(QVector<publicclass::DataRec> &S2Config, bool BaseBoard=true, QWidget *parent = 0);

private:
    Config35 *C35;

    void Fill();
    void CheckConf();

signals:
    void BsiIsNeedToBeAcquiredAndChecked();

public slots:

private slots:
};

#endif // CONFDIALOG35_H
