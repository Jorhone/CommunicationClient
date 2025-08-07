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
    //启动线程
    m_DataHandleThreadPTR->SetThreadFunc(this);
    m_DataHandleThreadPTR->start();
}

CCommunicationForSerialPort::~CCommunicationForSerialPort()
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

    disconnect(m_SerialPortPTR, &QSerialPort::readyRead, this, &CCommunicationForSerialPort::onDataReceived);
    disconnect(m_SerialPortPTR, &QSerialPort::errorOccurred, this, &CCommunicationForSerialPort::onExceptionTriggered);

    if(m_SerialPortPTR != nullptr)
    {
        if(m_SerialPortPTR->isOpen())
        {
            m_SerialPortPTR->close();
        }
    }
}

nsCommunicationClient::eCommunicationResult CCommunicationForSerialPort::Connect(const CAbstractCommunicationConf &vConfig, quint64 vTimeout)
{
    Q_UNUSED(vTimeout)

    if(IsConnected())
        return nsCommunicationClient::e_Result_ConnectionInUse;

    // 动态类型转换，确保配置类类型正确
    CCommunicationConfForSerialPort* tCommunicationConfigPTR = dynamic_cast<CCommunicationConfForSerialPort*>((CAbstractCommunicationConf*)&vConfig);
    if(tCommunicationConfigPTR == nullptr)
        return nsCommunicationClient::e_Result_ConfigError;

    if(tCommunicationConfigPTR->GetCommunicationMedium() != nsCommunicationClient::e_Medium_SerialPort)
        return nsCommunicationClient::e_Result_ConfigError;

    if(!tCommunicationConfigPTR->CheckDataAvailable())
        return nsCommunicationClient::e_Result_ConfigError;

    // 设置串口参数
    m_SerialPortPTR->setPortName(tCommunicationConfigPTR->m_PortName);
    m_SerialPortPTR->setBaudRate(tCommunicationConfigPTR->m_BaudRate);
    m_SerialPortPTR->setDataBits(tCommunicationConfigPTR->m_DataBits);
    m_SerialPortPTR->setParity(tCommunicationConfigPTR->m_Parity);
    m_SerialPortPTR->setStopBits(tCommunicationConfigPTR->m_StopBits);
    m_SerialPortPTR->setFlowControl(tCommunicationConfigPTR->m_FlowControl);

    // 打开串口
    bool tOpenResult = m_SerialPortPTR->open(QIODevice::ReadWrite);
    if(!tOpenResult)
    {
        m_SerialPortPTR->close();
        return nsCommunicationClient::e_Result_SerialPortCannotBeOpened;
    }

    m_CommunicationMedium = tCommunicationConfigPTR->GetCommunicationMedium();
    return nsCommunicationClient::e_Result_Success;
}

nsCommunicationClient::eCommunicationResult CCommunicationForSerialPort::Disconnect(quint64 vTimeout)
{
    Q_UNUSED(vTimeout);

    if(!IsConnected())
        return nsCommunicationClient::e_Result_Success;

    if(m_SerialPortPTR->isOpen())
    {
        m_SerialPortPTR->close();
    }

    m_CommunicationMedium = nsCommunicationClient::e_Medium_Unknown;
    return nsCommunicationClient::e_Result_Success;
}

nsCommunicationClient::eCommunicationResult CCommunicationForSerialPort::SendData(const QByteArray &vDataArray, quint64 vTimeout)
{
    if(!IsConnected() || !m_SerialPortPTR->isWritable())
        return nsCommunicationClient::e_Result_WriteInhibit;

    qint64 tSendDataLen = m_SerialPortPTR->write(vDataArray);

    //检查发送是否成功启动
    if(tSendDataLen < 0)
        return nsCommunicationClient::e_Result_SendException;

    // 检查发送是否完整
    if(tSendDataLen < vDataArray.size())
        return nsCommunicationClient::e_Result_WriteBufferFull;

    // 超时检测和中断机制
    bool tIsTimeout = true;
    QElapsedTimer tElapsedTimer;
    tElapsedTimer.start();

    do
    {
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

bool CCommunicationForSerialPort::IsConnected()
{
    if(m_SerialPortPTR == nullptr)
        return false;

    if(m_SerialPortPTR->isOpen())
        return true;

    return false;
}

void CCommunicationForSerialPort::ThreadFunction(const CDataHandleThread *vThreadPTR, const QVariant &vFuncData)
{
    Q_UNUSED(vFuncData)

    do
    {
        m_AsyncSession.wait();

        if(vThreadPTR->isInterruptionRequested())
            break;

        ReceiveALLData();
        emit forDataReceived();

    }while(1);
}

void CCommunicationForSerialPort::onDataReceived()
{
    m_AsyncSession.awaken();
}

void CCommunicationForSerialPort::onExceptionTriggered(QSerialPort::SerialPortError vErrorCode)
{
    //是否关键异常
    bool tIsCritical = false;
    switch(vErrorCode)
    {
    case QSerialPort::NoError: {
        return;
    }
    case QSerialPort::NotOpenError: {
        tIsCritical = false;
        break;
    }
    default:
        tIsCritical = true;
        break;
    }

    QString tErrorSTR = m_SerialPortPTR->errorString();
    CCommunicationException tCommunicationException(vErrorCode, tErrorSTR, !tIsCritical);
    emit forExceptionTriggered(tCommunicationException);
}

void CCommunicationForSerialPort::ReceiveALLData()
{
    QMutexLocker x(&m_DataMutex);
    QByteArray tDataArray = m_SerialPortPTR->readAll();
    m_ReceiveBuffer.push_back(tDataArray);
}
