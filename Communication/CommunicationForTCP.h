#ifndef CCOMMUNICATIONFORTCP_H
#define CCOMMUNICATIONFORTCP_H

#include "AbstractCommunication.h"

#include <QObject>
#include <QTcpSocket>

class CCommunicationForTCP : public QObject, public CAbstractCommunication
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
    virtual nsCommunicationClient::eCommunicationResult Connect(const CAbstractCommunicationConfig& vConfig, quint64 vTimeout);

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

signals:
    void forConnected(const QString& vPeerAddress, quint32 vPeerPort);
    void forDisconnected(void);
    void forDataSent(const QByteArray& vDataArray);

private slots:
    void onConnected(const QString& vPeerAddress, quint32 vPeerPort);
    void onDisconnected(void);
    void onDataSent(const QByteArray& vDataArray);
    void onDataReceived(void);
    void onExceptionTriggered(QAbstractSocket::SocketError vErrorCode);

private:
    nsCommunicationClient::eCommunicationMedium m_CommunicationMedium = nsCommunicationClient::e_Medium_Unknown;

    QTcpSocket* m_SocketPTR = nullptr;
    bool        m_StopFlag  = true;

    bool        m_IsSendFinish  = false;
    qint64      m_SendDataLen   = 0;
};

#endif // CCOMMUNICATIONFORTCP_H
