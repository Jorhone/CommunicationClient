#include "CommunicationConfForSerialPort.h"

CCommunicationConfForSerialPort::CCommunicationConfForSerialPort()
{
    m_PortName = "";
    m_BaudRate = QSerialPort::Baud9600;
    m_DataBits = QSerialPort::Data8;
    m_Parity = QSerialPort::NoParity;
    m_StopBits = QSerialPort::OneStop;
    m_FlowControl = QSerialPort::NoFlowControl;
}

nsCommunicationClient::eCommunicationMedium CCommunicationConfForSerialPort::GetCommunicationMedium()
{
    return nsCommunicationClient::e_Medium_SerialPort;
}

bool CCommunicationConfForSerialPort::CheckDataAvailable()
{
    if(m_PortName.isEmpty())
        return false;

    return true;
}
