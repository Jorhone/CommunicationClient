#ifndef CCOMMUNICATIONCONFIG_H
#define CCOMMUNICATIONCONFIG_H

#include "AbstractCommunicationConfig.h"
#include "CommunicationClientExport.h"

#include <QString>

class COMMUNICATIONCLIENT_EXPORT CCommunicationConfigTCP : public CAbstractCommunicationConfig
{
public:
    QString     m_PeerAddress;          //对端IP地址
    uint32_t    m_PeerPort;             //对端服务端口

//    uint64_t    m_SendBufferSize;       //发送缓冲区大小，单位：字节
    uint64_t    m_ReceiveBufferSize;    //接收缓冲区大小，单位：字节

public:
    CCommunicationConfigTCP();

    virtual nsCommunicationClient::eCommunicationMedium GetCommunicationMedium(void);
    virtual bool CheckDataAvailable(void);
};

#endif // CCOMMUNICATIONCONFIG_H
