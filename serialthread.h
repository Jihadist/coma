#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QObject>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMutex>

#define SERIALER(a)       ERMSG(publicclass::ER_SERIAL,__LINE__,a)
#define SERIALDBG         DBGMSG(publicclass::ER_SERIAL,__LINE__)
#define SERIALWARN        WARNMSG(publicclass::ER_SERIAL,__LINE__)
#define SERIALINFO(a)     INFOMSG(publicclass::ER_SERIAL,__LINE__,a)

class SerialThread : public QObject
{
    Q_OBJECT

public:
    explicit SerialThread(QObject *parent = 0);
    QSerialPortInfo portinfo;
    int baud;
    QByteArray OutDataBuf;
    bool ClosePortAndFinishThread;

private:
    QSerialPort *port;
    QByteArray *DataToSend;
    quint16 RcvDataLength;
    QMutex OutDataBufMtx, ReadDataMtx;

signals:
    void canalisready();
    void newdataarrived(QByteArray);
    void datawritten(QByteArray);
    void receivecompleted();
    void finished();

public slots:
    void run();
    void stop();
    void WriteData();
    void InitiateWriteDataToPort(QByteArray);

private slots:
    void CheckForData();
    void Error(QSerialPort::SerialPortError);
};

#endif // SERIALTHREAD_H
