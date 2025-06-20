#ifndef CCOMMUNICATIONFORTCP_H
#define CCOMMUNICATIONFORTCP_H

#include "AbstractCommunication.h"
#include "DataHandleThread.h"
#include "AsyncSession.hpp"

#include <QObject>
#include <QTcpSocket>

class CCommunicationForTCP : public QObject, public CAbstractCommunication, public CAbstractThreadFunc
{
    Q_OBJECT
public:
    CCommunicationForTCP(QObject *parent = nullptr);
    ~CCommunicationForTCP();

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
     * @brief GetCommunicationMedium    获取通信媒介/通信类型
     * @return
     */
    virtual nsCommunicationClient::eCommunicationMedium GetCommunicationMedium(void);

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
     * @param vDataArray            [out]接收到的数据内容，统一使用uft8
     * @param vDataLength           [out]接收到的数据长度
     */
    virtual void forDataReceived(QByteArray vDataArray, quint64 vDataLength);

    /**
     * @brief forExceptionTriggered 异常触发信号
     * @param vException            [out]异常信息
     */
    virtual void forExceptionTriggered(CCommunicationException vException);

private slots:
    void onDataReceived(void);
    void onExceptionTriggered(QAbstractSocket::SocketError vErrorCode);

private:
    volatile bool m_StopFlag  = true;

    QTcpSocket* m_SocketPTR = nullptr;
    CDataHandleThread* m_DataHandleThreadPTR = nullptr;
    AsyncSession<int> m_AsyncSession;
};

#endif // CCOMMUNICATIONFORTCP_H
