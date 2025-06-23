#ifndef CCOMMUNICATIONCONFFORSERIALPORT_H
#define CCOMMUNICATIONCONFFORSERIALPORT_H

#include "AbstractCommunicationConf.h"
#include "CommunicationClientExport.h"

#include <QString>
#include <QSerialPort>

class COMMUNICATIONCLIENT_EXPORT CCommunicationConfForSerialPort : public CAbstractCommunicationConf
{
public:
    QString m_PortName;                     // 串口号，如"COM1"、"/dev/ttyS0"
    qint32 m_BaudRate;                      // 波特率
    QSerialPort::DataBits m_DataBits;       // 数据位
    QSerialPort::Parity m_Parity;           // 校验位
    QSerialPort::StopBits m_StopBits;       // 停止位
    QSerialPort::FlowControl m_FlowControl; // 流控

public:
    CCommunicationConfForSerialPort();

    virtual nsCommunicationClient::eCommunicationMedium GetCommunicationMedium(void);
    virtual bool CheckDataAvailable(void);
};

#endif // CCOMMUNICATIONCONFFORSERIALPORT_H
