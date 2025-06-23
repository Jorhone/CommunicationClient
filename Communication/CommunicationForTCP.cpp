#include "CommunicationForTCP.h"
#include "CommunicationConfForTCP.h"

#include <QElapsedTimer>
#include <QCoreApplication>

CCommunicationForTCP::CCommunicationForTCP(QObject *parent)
    : QObject(parent)
{
    m_SocketPTR = new QTcpSocket(this);

    connect(m_SocketPTR, &QTcpSocket::readyRead, this, &CCommunicationForTCP::onDataReceived);
    connect(m_SocketPTR, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onExceptionTriggered(QAbstractSocket::SocketError)));

    //创建线程对象
    m_DataHandleThreadPTR = new CDataHandleThread;
    //启动线程
    m_DataHandleThreadPTR->SetThreadFunc(this);
    m_DataHandleThreadPTR->start();
}

CCommunicationForTCP::~CCommunicationForTCP()
{
    //销毁线程对象
    if(m_DataHandleThreadPTR != nullptr)
    {
        //停止线程
        if(m_DataHandleThreadPTR->isRunning())
        {
            m_DataHandleThreadPTR->requestInterruption(); //请求中止
            m_AsyncSession.awaken(); //唤醒

            m_DataHandleThreadPTR->quit();
            m_DataHandleThreadPTR->wait();
        }

        delete m_DataHandleThreadPTR;
        m_DataHandleThreadPTR = nullptr;
    }

    disconnect(m_SocketPTR, &QTcpSocket::readyRead, this, &CCommunicationForTCP::onDataReceived);
    disconnect(m_SocketPTR, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onExceptionTriggered(QAbstractSocket::SocketError)));

    if(m_SocketPTR != nullptr)
    {
        if(m_SocketPTR->isOpen())
        {
            //关闭socket通道
            m_SocketPTR->abort();
        }
    }
}

nsCommunicationClient::eCommunicationResult CCommunicationForTCP::Connect(const CAbstractCommunicationConf &vConfig, quint64 vTimeout)
{
    //不可重复连接
    if(IsConnected())
        return nsCommunicationClient::e_Result_ConnectionInUse;

    //检查通信配置参数
    CCommunicationConfForTCP* tCommunicationConfigPTR = dynamic_cast<CCommunicationConfForTCP*>((CAbstractCommunicationConf*)&vConfig);
    if(tCommunicationConfigPTR == nullptr)
        return nsCommunicationClient::e_Result_ConfigError;

    if(tCommunicationConfigPTR->GetCommunicationMedium() != nsCommunicationClient::e_Medium_TCP)
        return nsCommunicationClient::e_Result_ConfigError;

    if(!tCommunicationConfigPTR->CheckDataAvailable())
        return nsCommunicationClient::e_Result_ConfigError;

    //连接对端
    m_SocketPTR->connectToHost(tCommunicationConfigPTR->m_PeerAddress, tCommunicationConfigPTR->m_PeerPort);

    //等待连接完成
    bool tIsTimeout = true;
    QElapsedTimer tElapsedTimer;
    tElapsedTimer.start();

    do
    {
        if(vTimeout <= 0)
            break;

        if(m_SocketPTR->state() == QAbstractSocket::ConnectedState)
        {
            tIsTimeout = false;
            break;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    }while((quint64)tElapsedTimer.elapsed() < vTimeout);

    //超时了
    if(tIsTimeout)
    {
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

    //断开连接
    m_SocketPTR->disconnectFromHost();

    //等待连接断开
    bool tIsTimeout = true;
    QElapsedTimer tElapsedTimer;
    tElapsedTimer.start();

    do
    {
        if(vTimeout <= 0)
            break;

        if(m_SocketPTR->state() == QAbstractSocket::UnconnectedState)
        {
            tIsTimeout = false;
            break;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    }while((quint64)tElapsedTimer.elapsed() < vTimeout);

    //超时了，强杀
    if(tIsTimeout)
    {
        m_SocketPTR->abort();
    }

    m_CommunicationMedium = nsCommunicationClient::e_Medium_Unknown;
    return nsCommunicationClient::e_Result_Success;
}

nsCommunicationClient::eCommunicationResult CCommunicationForTCP::SendData(const QByteArray &vDataArray, quint64 vTimeout)
{
    //未连接/不可写，缺少写权限
    if(!IsConnected() || !m_SocketPTR->isWritable())
        return nsCommunicationClient::e_Result_WriteInhibit;

    //发送数据
    qint64 tSendDataLen = m_SocketPTR->write(vDataArray);
    
    //检查发送是否成功启动
    if(tSendDataLen < 0)
        return nsCommunicationClient::e_Result_SendException;

    //检查发送是否完整
    if(tSendDataLen < vDataArray.size())
        return nsCommunicationClient::e_Result_WriteBufferFull;

    //等待所有数据发送完成，带超时检测和快速跳出机制
    bool tIsTimeout = true;
    QElapsedTimer tElapsedTimer;
    tElapsedTimer.start();

    do
    {
        //检查是否超时
        if(vTimeout <= 0)
            break;

        //检查发送缓冲区是否为空（数据已全部发送）
        if(m_SocketPTR->bytesToWrite() == 0)
        {
            tIsTimeout = false;
            break;
        }

        //处理事件，避免界面卡死
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    }while((quint64)tElapsedTimer.elapsed() < vTimeout);

    // 超时或被停止
    if(tIsTimeout)
        return nsCommunicationClient::e_Result_SendException;

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

void CCommunicationForTCP::ThreadFunction(const CDataHandleThread *vThreadPTR, const QVariant &vFuncData)
{
    Q_UNUSED(vFuncData)

    do
    {
        //休眠
        m_AsyncSession.wait();

        //线程请求中止，退出循环
        if(vThreadPTR->isInterruptionRequested())
            break;

        //接收数据
        QByteArray tDataArray = m_SocketPTR->readAll();
        emit forDataReceived(tDataArray, tDataArray.length());

    }while(1);
}

void CCommunicationForTCP::onDataReceived()
{
    //唤醒
    m_AsyncSession.awaken();
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
