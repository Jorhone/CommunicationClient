#include "CommunicationConfForTCP.h"

CCommunicationConfForTCP::CCommunicationConfForTCP()
{
    m_PeerAddress = "127.0.0.1";
    m_PeerPort = 0;
    m_SendBufferSize = 4*1024*1024;
    m_ReceiveBufferSize = 4*1024*1024;
}

nsCommunicationClient::eCommunicationMedium CCommunicationConfForTCP::GetCommunicationMedium()
{
    return nsCommunicationClient::e_Medium_TCP;
}

bool CCommunicationConfForTCP::CheckDataAvailable()
{
    if(m_PeerAddress.isEmpty())
        return false;

    if(m_PeerPort <= 0)
        return false;

    return true;
}
