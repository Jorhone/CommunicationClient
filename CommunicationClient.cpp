#include "CommunicationClient.h"
#include "CommunicationClientDefine.h"

#include "AbstractCommunicationConfig.h"
#include "CommunicationForTCP.h"

CCommunicationClient::CCommunicationClient(QObject *parent)
    : QObject(parent)
{

}

CCommunicationClient::~CCommunicationClient()
{
    DestoryCommunication();
}

bool CCommunicationClient::Connect(const CAbstractCommunicationConfig &vConfig, quint64 vTimeout)
{
    bool tHasException = true;

    do
    {
        bool tCreateRet = CreateCommunication(vConfig);
        if(!tCreateRet)
        {
            m_LastErrorDescribe = "实例化通信模块失败";
            break;
        }

        nsCommunicationClient::eCommunicationResult tCommunicationResult = m_CommunicationPTR->Connect(vConfig, vTimeout);
        if(tCommunicationResult != nsCommunicationClient::e_Result_Success)
        {
            m_LastErrorDescribe = nsCommunicationClient::CCommunicationEnumConvert::EnumConvertToSTR(tCommunicationResult);
            break;
        }

        tHasException = false;

    }while(0);

    if(tHasException)
    {
        DestoryCommunication();
        return false;
    }

    return true;
}

bool CCommunicationClient::Disconnect(quint64 vTimeout)
{
    if(!IsConnected())
        return true;

    nsCommunicationClient::eCommunicationResult tCommunicationResult = m_CommunicationPTR->Disconnect(vTimeout);
    if(tCommunicationResult != nsCommunicationClient::e_Result_Success)
    {
        m_LastErrorDescribe = nsCommunicationClient::CCommunicationEnumConvert::EnumConvertToSTR(tCommunicationResult);
        return false;
    }

    DestoryCommunication();
    return true;
}

bool CCommunicationClient::SendData(const QByteArray &vDataArray, quint64 vTimeout)
{
    if(!IsConnected())
    {
        m_LastErrorDescribe = "通信模块未连接，发送禁止";
        return false;
    }

    nsCommunicationClient::eCommunicationResult tCommunicationResult = m_CommunicationPTR->SendData(vDataArray, vTimeout);
    if(tCommunicationResult != nsCommunicationClient::e_Result_Success)
    {
        m_LastErrorDescribe = nsCommunicationClient::CCommunicationEnumConvert::EnumConvertToSTR(tCommunicationResult);
        return false;
    }

    return true;
}

bool CCommunicationClient::IsConnected()
{
    if(m_CommunicationPTR == nullptr)
        return false;

    return m_CommunicationPTR->IsConnected();
}

QString CCommunicationClient::GetLastErrorDescribe()
{
    return m_LastErrorDescribe;
}

bool CCommunicationClient::CreateCommunication(const CAbstractCommunicationConfig &vConfig)
{
    CAbstractCommunicationConfig* tConfigPTR = const_cast<CAbstractCommunicationConfig*>(&vConfig);

    //如果已经实例化，直接返回
    if(m_CommunicationPTR != nullptr)
    {
        //不同类型，返回失败
        if(m_CommunicationPTR->GetCommunicationMedium() != tConfigPTR->GetCommunicationMedium())
        {
            return false;
        }

        return true;
    }

    switch(tConfigPTR->GetCommunicationMedium())
    {
    case nsCommunicationClient::e_Medium_TCP: {
        m_CommunicationPTR = new CCommunicationForTCP();
        break;
    }
    default:
        break;
    }

    //实例化失败
    if(m_CommunicationPTR == nullptr)
        return false;

    QObject* tObjectPTR = dynamic_cast<QObject*>(m_CommunicationPTR);
    if(tObjectPTR == nullptr)
    {
        DestoryCommunication();
        return false;
    }

    //信号连接
    connect(tObjectPTR, SIGNAL(forDataReceived(QByteArray, quint64)), this, SIGNAL(forDataReceived(QByteArray, quint64)), Qt::DirectConnection);
    connect(tObjectPTR, SIGNAL(forExceptionTriggered(CCommunicationException)), this, SIGNAL(forExceptionTriggered(CCommunicationException)), Qt::DirectConnection);

    //移入子线程
    m_WorkThreadPTR = new QThread(this);
    tObjectPTR->moveToThread(m_WorkThreadPTR);
    m_WorkThreadPTR->start();

    return true;
}

void CCommunicationClient::DestoryCommunication()
{
    if(m_CommunicationPTR == nullptr)
        return ;

    if(m_WorkThreadPTR != nullptr)
    {
        m_WorkThreadPTR->requestInterruption();
        m_WorkThreadPTR->quit();
        m_WorkThreadPTR->wait();

        delete m_WorkThreadPTR;
        m_WorkThreadPTR = nullptr;
    }

    delete m_CommunicationPTR;
    m_CommunicationPTR = nullptr;
}
