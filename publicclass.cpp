#include <QDateTime>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include "publicclass.h"
#include "log.h"

publicclass pc;

unsigned long dwPolynomial = 0xEDB88320;

unsigned long  _crc32_t[256]=
{
0x00000000,0x77073096,0xEE0E612C,0x990951BA,
0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,
0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,
0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,
0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,
0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,
0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,
0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,
0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,
0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,
0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,
0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,
0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,
0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,
0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,
0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,
0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,
0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,
0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,
0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,
0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,
0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,
0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,
0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,
0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,
0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,
0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,
0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,
0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,
0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,
0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,
0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,
0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,
0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,
0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,
0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,
0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,
0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,
0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,
0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,
0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,
0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,
0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,
0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,
0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,
0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,
0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D
};

publicclass::publicclass()
{
    Emul = false;
    ModuleBsi.MTypeB = ModuleBsi.MTypeM = 0xFFFFFFFF;
    log.Init(LOGFILE);

    QFile file;
    QString ermsgspath = pc.HomeDir;
    file.setFileName(ermsgspath+"ermsgs.dat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        result = ER_FILEOPENERROR;
        return;
    }
    else
    {
        QString tmpString;
        QTextStream streamfile(&file);
        streamfile.setCodec("WIN-1251");
        do
        {
            tmpString = streamfile.readLine();
            if (tmpString.isEmpty())
                errmsgs << "";
            else if (tmpString.at(0) != '#')
                errmsgs << tmpString;
        } while (!streamfile.atEnd());
    }
    result = NOERROR;
}

QString publicclass::VerToStr(quint32 num)
{
    int mv = (num&0xFF000000)>>24;
    int lv = (num&0x00FF0000)>>16;
    int sv = (num&0x0000FFFF);
    QString tmpString = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 16, QChar('0'));
    return tmpString;
}

int publicclass::StoreDataMem(void *mem, QVector<DataRec> *dr, quint16 fname) //0 - успешно, иначе код ошибки
{
    quint32 crc=0xFFFFFFFF;
    FileHeader D;
    DataRec R;
    quint32 i;
    char *m=static_cast<char *>(mem);
    m+=sizeof(FileHeader);
    D.size=0;
    for(QVector<DataRec>::iterator it=dr->begin(); it!=dr->end(); ++it)
    {
        R = *it;
        void *Rptr = static_cast<void *>(it);
        quint32 tmpi = sizeof(DataRec)-sizeof(void*);
        memcpy(m,&R,tmpi);
        D.size += tmpi;
        for(i=0;i<tmpi;i++)
            updCRC32((static_cast<unsigned char *>(Rptr))[i],&crc);
        if(R.id==0xFFFFFFFF)
            break;
        m+=tmpi;
        if(R.thedata)
        {
            tmpi = R.num_byte;
            for(i=0;i<tmpi;i++)
                updCRC32((static_cast<unsigned char *>(R.thedata))[i],&crc);
            D.size += tmpi;
            memcpy(m,R.thedata,tmpi);
            m+=tmpi;
        }
    }
    D.crc32=crc;
    D.thetime=getTime32();
    D.service=0xFFFF;
    D.fname=fname;
    memcpy(mem,&D,sizeof(D));
    return 0;
}

int publicclass::RestoreDataMem(void *mem, quint32 memsize, QVector<DataRec> *dr)
{
  quint32 crc;
  quint32 sz=0;
  char *m=static_cast<char *>(mem);
  DataRec R;
  DataRec *r;
  FileHeader dh;
  quint32 i;
  bool NoIDs=true; // признак того, что не встретился ни один из ID в dr

  crc=0xFFFFFFFF;

  quint32 tmpi = sizeof(FileHeader);
  quint32 pos = tmpi;
  if (pos > memsize)
  {
      ERMSG("S2: выход за границу принятых байт"); // выход за границу принятых байт
      return S2_SIZEERROR;
  }
  memcpy(&dh,m,tmpi);
  m+=tmpi;
  for (i=0; i<(memsize-tmpi); i++)
      updCRC32(m[i], &crc);
  if (dh.crc32!=crc)
  {
      ERMSG("S2: Ошибка CRC"); // выход за границу принятых байт
      return S2_CRCERROR;
  }
  for(;;)
  {
      tmpi = sizeof(DataRec)-sizeof(void*);
      pos += tmpi;
      if (pos > memsize)
      {
          ERMSG("S2: выход за границу принятых байт"); // выход за границу принятых байт
          return S2_SIZEERROR;
      }
      memcpy(&R,m,tmpi);
      sz+=tmpi;
      m+=tmpi;
      if(R.id==0xFFFFFFFF)
          break;
      r=FindElem(dr,R.id);
      if(r == 0) //элемент не найден в описании, пропускаем
      {
          tmpi = R.num_byte;
          pos += tmpi;
          if (pos > memsize)
          {
              ERMSG("S2: выход за границу принятых байт"); // выход за границу принятых байт
              return S2_SIZEERROR;
          }
          m += tmpi;
          sz += tmpi;
          continue;
      }
      NoIDs = false;
      if (r->num_byte!=R.num_byte) //несовпадения описания прочитанного элемента с ожидаемым
      {
          ERMSG("Несовпадение описаний одного и того же блока"); // несовпадение описаний одного и того же блока
          return S2_DESCERROR;
      }
      tmpi = r->num_byte;
      pos += tmpi;
      if (pos > memsize)
      {
          ERMSG("S2: выход за границу принятых байт"); // выход за границу принятых байт
          return S2_SIZEERROR;
      }
      memcpy(r->thedata,m,tmpi);
      sz += tmpi;
      m += tmpi;
  }
  if(dh.size!=sz)
  {
      ERMSG("S2: ошибка длины"); // ошибка длины
      return S2_DHSZERROR;
  }
  if (NoIDs)
  {
      ERMSG("Не найдено ни одного элемента с заданным ID"); // не найдено ни одного ИД
      return S2_NOIDS;
  }
  return 0;
}

publicclass::DataRec *publicclass::FindElem(QVector<DataRec> *dr, quint16 id)
{
    for(QVector<DataRec>::iterator it=dr->begin(); it!=dr->end(); ++it)
    {
        DataRec R = *it;
        if(R.id==id)
            return it;
        if(R.id==static_cast<quint16>(0xFFFF))
            return 0;
    }
    return 0;
}

void inline publicclass::updCRC32(const quint8 byte, quint32 *dwCRC32)
{
    *dwCRC32 = (( *dwCRC32 ) >> 8 )^ _crc32_t[( byte )^(( *dwCRC32 ) & 0x000000FF )];
}

quint32 publicclass::GetCRC32(char *data, quint32 len)
{
  quint32 dwCRC32 = 0xFFFFFFFF;
  for(quint32 i=0;i<len;i++)
  {
      updCRC32(*data,&dwCRC32);
      data++;
  }
  return dwCRC32;
}

quint32 publicclass::getTime32()
{
    QDateTime dt;
    return dt.currentDateTime().toTime_t();
}

QString publicclass::NsTimeToString(quint64 nstime)
{
    quint32 unixtime = nstime / 1000000000L; // unix время
    QString tmps = QDateTime::fromTime_t(unixtime).toString("dd-MM-yyyy hh:mm:ss");
    quint32 nst = nstime % 1000000000L; // оставшееся время в нс до секунды
    tmps += "." + QString("%1").arg((nst/1000000L),3,10,QChar('0'));
    nst = nst % 1000000L;
    tmps += "." + QString("%1").arg((nst/1000L),3,10,QChar('0'));
    nst = nst % 1000L;
    tmps += "." + QString("%1").arg(nst,3,10,QChar('0'));
    return tmps;
}

void publicclass::AddErrMsg(ermsgtype msgtype, QString file, int line, QString msg)
{
    if (ErMsgPool.size()>=ER_BUFMAX)
        ErMsgPool.removeFirst();
    ermsg tmpm;
    tmpm.type = msgtype;
    tmpm.file = file;
    tmpm.line = line;
    tmpm.DateTime = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");
    // Разбор кода ошибки
    QString prefix;
    if ((msg.isEmpty()) || (msg == " ")) // пробел выдаётся при пустом запросе в БД
    {
        switch (msgtype)
        {
        case ER_MSG:
            prefix = "Ошибка ";
            break;
        case WARN_MSG: prefix = "Проблема "; break;
        case INFO_MSG: prefix = "Инфо "; break;
        case DBG_MSG: prefix = "Отладка "; break;
        }
        msg = prefix+"в файле " + tmpm.file + " строка " + QString::number(tmpm.line);
    }
    if ((msgtype == ER_MSG) || (msgtype == DBG_MSG))
        log.error("file: "+tmpm.file+", line: "+QString::number(tmpm.line)+": "+msg);
    else if (msgtype == WARN_MSG)
        log.warning("file: "+tmpm.file+", line: "+QString::number(tmpm.line)+": "+msg);
    else
        log.info("file: "+tmpm.file+", line: "+QString::number(tmpm.line)+": "+msg);
    tmpm.msg = msg;
    ErMsgPool.append(tmpm);
}

int publicclass::LoadFile(QWidget *parent, QString mask, QByteArray &ba)
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getOpenFileName(parent, "Открыть файл", HomeDir, mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    dlg->close();
    if (filename.isEmpty())
    {
        ERMSG("Пустое имя файла");
        return ER_FILEOPEN; // Пустое имя файла
    }
    QFile *file = new QFile;
    file->setFileName(filename);
    if (!file->open(QIODevice::ReadOnly))
    {
        ERMSG("Ошибка открытия файла");
        return ER_FILEOPEN; // Ошибка открытия файла
    }
    ba = file->readAll();
    file->close();
/*    maxsize = (LoadBa.size() <= maxsize) ? LoadBa.size() : maxsize;
    memcpy(dst, &(LoadBa.data()[0]), maxsize); */
    return NOERROR;
}

int publicclass::SaveFile(QWidget *parent, const QString &mask, const QString &ext, QByteArray &src, unsigned int numbytes)
{
    QString MTypeM = (ModuleBsi.MTypeM == 0) ? "00" : QString::number(ModuleBsi.MTypeM, 16);
    QString tmps = HomeDir + QString::number(ModuleBsi.MTypeB, 16)+MTypeM+"-"+\
            QString("%1").arg(ModuleBsi.SerialNum, 8, 10, QChar('0'))+"."+ext;
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getSaveFileName(parent, "Сохранить файл", tmps, mask, Q_NULLPTR, QFileDialog::DontUseNativeDialog);
    dlg->close();
    if (filename.isEmpty())
        return ER_FILENAMEEMP; // Пустое имя файла
    QFile *file = new QFile;
    file->setFileName(filename);
    if (!file->open(QIODevice::WriteOnly))
        return ER_FILEOPEN; // Ошибка открытия файла
    int res = file->write(src, numbytes);
    file->close();
    delete file;
    if (res == GENERALERROR)
        return ER_FILEWRITE; // ошибка записи
    return NOERROR; // нет ошибок
}

bool publicclass::FloatInRange(float var, float value)
{
    if ((var > (value-TH01)) && (var < (value+TH01)))
        return true;
    else
        return false;
}

void publicclass::ErMsg(int ermsgnum)
{
    if (ermsgnum < pc.errmsgs.size())
        ERMSG(pc.errmsgs.at(ermsgnum));
    else
        ERMSG("Произошла неведомая фигня #"+QString::number(ermsgnum,10));
}
