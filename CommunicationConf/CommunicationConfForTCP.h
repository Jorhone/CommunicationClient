#ifndef CCOMMUNICATIONCONFFORTCP_H
#define CCOMMUNICATIONCONFFORTCP_H

#include "AbstractCommunicationConf.h"
#include "CommunicationClientExport.h"

#include <QString>

class COMMUNICATIONCLIENT_EXPORT CCommunicationConfForTCP : public CAbstractCommunicationConf
{
public:
    QString     m_PeerAddress;          //对端IP地址
    uint32_t    m_PeerPort;             //对端服务端口

    uint64_t    m_SendBufferSize;       //发送缓冲区大小，单位：字节
    uint64_t    m_ReceiveBufferSize;    //接收缓冲区大小，单位：字节

public:
    CCommunicationConfForTCP();

    virtual nsCommunicationClient::eCommunicationMedium GetCommunicationMedium(void);
    virtual bool CheckDataAvailable(void);
};

#endif // CCOMMUNICATIONCONFFORTCP_H
