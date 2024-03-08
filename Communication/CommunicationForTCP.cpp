#include "CommunicationForTCP.h"
#include "CommunicationConfig.h"

#include <QElapsedTimer>
#include <QCoreApplication>

CCommunicationForTCP::CCommunicationForTCP(QObject *parent)
    : QObject(parent)
{
    m_SocketPTR = new QTcpSocket(this);

    connect(this, &CCommunicationForTCP::forConnected, this, &CCommunicationForTCP::onConnected);
    connect(this, &CCommunicationForTCP::forDisconnected, this, &CCommunicationForTCP::onDisconnected);
    connect(this, &CCommunicationForTCP::forDataSent, this, &CCommunicationForTCP::onDataSent);

    connect(m_SocketPTR, &QTcpSocket::readyRead, this, &CCommunicationForTCP::onDataReceived);
    connect(m_SocketPTR,
            SIGNAL(error(QAbstractSocket::SocketError)),
            this,
            SLOT(onExceptionTriggered(QAbstractSocket::SocketError)));
}

CCommunicationForTCP::~CCommunicationForTCP()
{
    disconnect(this, &CCommunicationForTCP::forConnected, this, &CCommunicationForTCP::onConnected);
    disconnect(this, &CCommunicationForTCP::forDisconnected, this, &CCommunicationForTCP::onDisconnected);
    disconnect(this, &CCommunicationForTCP::forDataSent, this, &CCommunicationForTCP::onDataSent);

    disconnect(m_SocketPTR, &QTcpSocket::readyRead, this, &CCommunicationForTCP::onDataReceived);
    disconnect(m_SocketPTR,
            SIGNAL(error(QAbstractSocket::SocketError)),
            this,
            SLOT(onExceptionTriggered(QAbstractSocket::SocketError)));


    m_StopFlag = true;
    if(m_SocketPTR->isOpen())
        m_SocketPTR->abort();
}

nsCommunicationClient::eCommunicationResult CCommunicationForTCP::Connect(const CAbstractCommunicationConfig &vConfig, quint64 vTimeout)
{
    //不可重复连接
    if(IsConnected())
        return nsCommunicationClient::e_Result_ConnectionInUse;

    //检查通信配置参数
    CCommunicationConfigTCP* tCommunicationConfigPTR = dynamic_cast<CCommunicationConfigTCP*>((CAbstractCommunicationConfig*)&vConfig);
    if(tCommunicationConfigPTR == nullptr)
        return nsCommunicationClient::e_Result_ConfigError;

    if(tCommunicationConfigPTR->GetCommunicationMedium() != nsCommunicationClient::e_Medium_TCP)
        return nsCommunicationClient::e_Result_ConfigError;

    if(!tCommunicationConfigPTR->CheckDataAvailable())
        return nsCommunicationClient::e_Result_ConfigError;

    //连接对端
    emit forConnected(tCommunicationConfigPTR->m_PeerAddress, tCommunicationConfigPTR->m_PeerPort);

    //等待连接完成
    bool tIsTimeout = true;
    QElapsedTimer tElapsedTimer;
    tElapsedTimer.start();

    m_StopFlag = false;

    do
    {
        if(m_SocketPTR->state() == QAbstractSocket::ConnectedState)
        {
            tIsTimeout = false;
            break;
        }

        if(m_StopFlag)
            break;

        if(vTimeout <= 0)
            break;

        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    }while((quint64)tElapsedTimer.elapsed() < vTimeout);

    //超时了
    if(tIsTimeout)
    {
        m_StopFlag = true;
        m_SocketPTR->abort();
        return nsCommunicationClient::e_Result_ConnectTimeout;
    }

    m_CommunicationMedium = tCommunicationConfigPTR->GetCommunicationMedium();
    m_SocketPTR->setReadBufferSize(tCommunicationConfigPTR->m_ReceiveBufferSize);
    return nsCommunicationClient::e_Result_Success;
}

nsCommunicationClient::eCommunicationResult CCommunicationForTCP::Disconnect(quint64 vTimeout)
{
    //未连接，直接返回
    if(!IsConnected())
        return nsCommunicationClient::e_Result_Success;

    m_CommunicationMedium = nsCommunicationClient::e_Medium_Unknown;
    m_StopFlag = true;

    //断开连接
    emit forDisconnected();

    //等待连接断开
    bool tIsTimeout = true;
    QElapsedTimer tElapsedTimer;
    tElapsedTimer.start();

    do
    {
        if(m_SocketPTR->state() == QAbstractSocket::UnconnectedState)
        {
            tIsTimeout = false;
            break;
        }

        if(vTimeout <= 0)
            break;

        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    }while((quint64)tElapsedTimer.elapsed() < vTimeout);

    //超时了，强杀
    if(tIsTimeout)
        m_SocketPTR->abort();

    return nsCommunicationClient::e_Result_Success;
}

nsCommunicationClient::eCommunicationResult CCommunicationForTCP::SendData(const QByteArray &vDataArray, quint64 vTimeout)
{
    //未连接/不可写，缺少写权限
    if(!IsConnected() || !m_SocketPTR->isWritable())
        return nsCommunicationClient::e_Result_WriteInhibit;

    m_IsSendFinish = false;
    m_SendDataLen = 0;

    emit forDataSent(vDataArray);

    //等待发送完成
    bool tIsTimeout = true;
    QElapsedTimer tElapsedTimer;
    tElapsedTimer.start();

    do
    {
        if(m_IsSendFinish)
        {
            tIsTimeout = false;
            break;
        }

        if(m_StopFlag)
            break;

        if(vTimeout <= 0)
            break;

        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    }while((quint64)tElapsedTimer.elapsed() < vTimeout);

    //超时了
    if(tIsTimeout)
        return nsCommunicationClient::e_Result_SendException;

    //发送异常
    if(m_SendDataLen < 0)
        return nsCommunicationClient::e_Result_SendException;

    //发送不完整
    if(m_SendDataLen < vDataArray.size())
        return nsCommunicationClient::e_Result_WriteBufferFull;

    return nsCommunicationClient::e_Result_Success;
}

nsCommunicationClient::eCommunicationMedium CCommunicationForTCP::GetCommunicationMedium()
{
    return m_CommunicationMedium;
}

bool CCommunicationForTCP::IsConnected()
{
    if(m_SocketPTR == nullptr)
        return false;

    if(m_SocketPTR->isOpen())
        return true;

    return false;
}

void CCommunicationForTCP::onConnected(const QString &vPeerAddress, quint32 vPeerPort)
{
    //连接对端
    m_SocketPTR->connectToHost(vPeerAddress, vPeerPort);
}

void CCommunicationForTCP::onDisconnected()
{
    //断开连接
    m_SocketPTR->disconnectFromHost();
}

void CCommunicationForTCP::onDataSent(const QByteArray &vDataArray)
{
    //发送数据
    m_SendDataLen = m_SocketPTR->write(vDataArray);
    m_IsSendFinish = true;
}

void CCommunicationForTCP::onDataReceived()
{
    //接收数据
    QByteArray tDataArray = m_SocketPTR->readAll();
    emit forDataReceived(tDataArray, tDataArray.length());
}

void CCommunicationForTCP::onExceptionTriggered(QAbstractSocket::SocketError vErrorCode)
{
    //是否关键异常
    bool tIsCritical = false;
    switch(vErrorCode)
    {
    case QAbstractSocket::RemoteHostClosedError:    //对端断开连接
    case QAbstractSocket::NetworkError: {           //网络异常
        tIsCritical = true;
        break;
    }
    default:
        break;
    }

    QString tErrorSTR = m_SocketPTR->errorString();
    CCommunicationException tCommunicationException(vErrorCode, tErrorSTR, !tIsCritical);
    emit forExceptionTriggered(tCommunicationException);
}
