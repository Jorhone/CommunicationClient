#ifndef CCOMMUNICATIONFORSERIALPORT_H
#define CCOMMUNICATIONFORSERIALPORT_H

#include "AbstractCommunication.h"
#include "DataHandleThread.h"
#include "AsyncSession.hpp"

#include <QObject>
#include <QSerialPort>

class CCommunicationForSerialPort : public QObject, public CAbstractCommunication, public CAbstractThreadFunc
{
    Q_OBJECT
public:
    CCommunicationForSerialPort(QObject *parent = nullptr);
    ~CCommunicationForSerialPort();

    /**
     * @brief Connect       建立连接
     * @param vConfig       [in]通信配置
     * @param vTimeout      [in]连接超时，单位：ms
     * @return
     */
    virtual nsCommunicationClient::eCommunicationResult Connect(const CAbstractCommunicationConf& vConfig, quint64 vTimeout);

    /**
     * @brief Disconnect    断开连接
     * @param vTimeout      [in]断开连接超时，单位：ms
     * @return
     */
    virtual nsCommunicationClient::eCommunicationResult Disconnect(quint64 vTimeout);

    /**
     * @brief SendData      发送数据
     * @param vDataArray    [in]数据内容
     * @param vTimeout      [in]发送超时，单位：ms
     * @return
     */
    virtual nsCommunicationClient::eCommunicationResult SendData(const QByteArray& vDataArray, quint64 vTimeout);

    /**
     * @brief IsConnected   是否已连接
     * @return
     */
    virtual bool IsConnected(void);

protected:
    virtual void ThreadFunction(const CDataHandleThread* vThreadPTR, const QVariant& vFuncData);

signals:
    /**
     * @brief forDataReceived       数据接收信号
     */
    virtual void forDataReceived(void);

    /**
     * @brief forExceptionTriggered 异常触发信号
     * @param vException            [out]异常信息
     */
    virtual void forExceptionTriggered(CCommunicationException vException);

private slots:
    void onDataReceived(void);
    void onExceptionTriggered(QSerialPort::SerialPortError vErrorCode);

private:
    void ReceiveALLData(void);

private:
    QSerialPort* m_SerialPortPTR = nullptr;
    CDataHandleThread* m_DataHandleThreadPTR = nullptr;
    AsyncSession<int> m_AsyncSession;
};

#endif // CCOMMUNICATIONFORSERIALPORT_H
