#include "CommunicationForSerialPort.h"
#include "CommunicationConfForSerialPort.h"

#include <QElapsedTimer>
#include <QCoreApplication>

CCommunicationForSerialPort::CCommunicationForSerialPort(QObject *parent)
    : QObject(parent)
{
    m_SerialPortPTR = new QSerialPort(this);

    connect(m_SerialPortPTR, &QSerialPort::readyRead, this, &CCommunicationForSerialPort::onDataReceived);
    connect(m_SerialPortPTR, &QSerialPort::errorOccurred, this, &CCommunicationForSerialPort::onExceptionTriggered);

    // 创建线程对象
    m_DataHandleThreadPTR = new CDataHandleThread;
    m_DataHandleThreadPTR->SetThreadFunc(this);
    m_DataHandleThreadPTR->start();
}

CCommunicationForSerialPort::~CCommunicationForSerialPort()
{
    // 停止线程
    if(m_DataHandleThreadPTR->isRunning())
    {
        m_DataHandleThreadPTR->requestInterruption();
        m_AsyncSession.awaken();
        m_DataHandleThreadPTR->quit();
        m_DataHandleThreadPTR->wait();
    }
    delete m_DataHandleThreadPTR;
    m_DataHandleThreadPTR = nullptr;

    disconnect(m_SerialPortPTR, &QSerialPort::readyRead, this, &CCommunicationForSerialPort::onDataReceived);
    disconnect(m_SerialPortPTR, &QSerialPort::errorOccurred, this, &CCommunicationForSerialPort::onExceptionTriggered);

    m_StopFlag = true;
    if(m_SerialPortPTR->isOpen())
    {
        m_SerialPortPTR->close();
    }
}

nsCommunicationClient::eCommunicationResult CCommunicationForSerialPort::Connect(const CAbstractCommunicationConf &vConfig, quint64 vTimeout)
{
    if(IsConnected())
        return nsCommunicationClient::e_Result_ConnectionInUse;

    // 假设有串口配置类 CommunicationConfForSerialPort
    auto* tConfig = dynamic_cast<const CommunicationConfForSerialPort*>(&vConfig);
    if(!tConfig)
        return nsCommunicationClient::e_Result_ConfigError;

    m_StopFlag = false;

    m_SerialPortPTR->setPortName(tConfig->m_PortName);
    m_SerialPortPTR->setBaudRate(tConfig->m_BaudRate);
    m_SerialPortPTR->setDataBits(tConfig->m_DataBits);
    m_SerialPortPTR->setParity(tConfig->m_Parity);
    m_SerialPortPTR->setStopBits(tConfig->m_StopBits);
    m_SerialPortPTR->setFlowControl(tConfig->m_FlowControl);

    if(!m_SerialPortPTR->open(QIODevice::ReadWrite))
        return nsCommunicationClient::e_Result_ConnectTimeout;

    m_CommunicationMedium = nsCommunicationClient::e_Medium_SerialPort;
    return nsCommunicationClient::e_Result_Success;
}

nsCommunicationClient::eCommunicationResult CCommunicationForSerialPort::Disconnect(quint64 vTimeout)
{
    Q_UNUSED(vTimeout);
    if(!IsConnected())
        return nsCommunicationClient::e_Result_Success;

    m_StopFlag = true;
    m_SerialPortPTR->close();
    m_CommunicationMedium = nsCommunicationClient::e_Medium_Unknown;
    return nsCommunicationClient::e_Result_Success;
}

nsCommunicationClient::eCommunicationResult CCommunicationForSerialPort::SendData(const QByteArray &vDataArray, quint64 vTimeout)
{
    if(!IsConnected() || !m_SerialPortPTR->isWritable())
        return nsCommunicationClient::e_Result_WriteInhibit;

    qint64 tSendDataLen = m_SerialPortPTR->write(vDataArray);

    // 检查发送是否完整
    if(tSendDataLen < vDataArray.size())
        return nsCommunicationClient::e_Result_WriteBufferFull;

    // 超时检测和中断机制
    bool tIsTimeout = true;
    QElapsedTimer tElapsedTimer;
    tElapsedTimer.start();

    do
    {
        if(m_StopFlag)
            break;

        if(vTimeout <= 0)
            break;

        if(m_SerialPortPTR->bytesToWrite() == 0)
        {
            tIsTimeout = false;
            break;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    }while((quint64)tElapsedTimer.elapsed() < vTimeout);

    if(tIsTimeout)
        return nsCommunicationClient::e_Result_SendException;

    return nsCommunicationClient::e_Result_Success;
}

nsCommunicationClient::eCommunicationMedium CCommunicationForSerialPort::GetCommunicationMedium()
{
    return m_CommunicationMedium;
}

bool CCommunicationForSerialPort::IsConnected()
{
    return m_SerialPortPTR && m_SerialPortPTR->isOpen();
}

void CCommunicationForSerialPort::ThreadFunction(const CDataHandleThread *vThreadPTR, const QVariant &vFuncData)
{
    Q_UNUSED(vFuncData)

    do
    {
        m_AsyncSession.wait();

        if(vThreadPTR->isInterruptionRequested())
            break;

        QByteArray tDataArray = m_SerialPortPTR->readAll();
        emit forDataReceived(tDataArray, tDataArray.length());

    }while(1);
}

void CCommunicationForSerialPort::onDataReceived()
{
    m_AsyncSession.awaken();
}

void CCommunicationForSerialPort::onExceptionTriggered(QSerialPort::SerialPortError vErrorCode)
{
    bool tIsCritical = (vErrorCode != QSerialPort::NoError);
    QString tErrorSTR = m_SerialPortPTR->errorString();
    CCommunicationException tCommunicationException(vErrorCode, tErrorSTR, !tIsCritical);
    emit forExceptionTriggered(tCommunicationException);
}
