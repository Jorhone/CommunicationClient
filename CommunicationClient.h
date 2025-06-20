#ifndef CCOMMUNICATIONCLIENT_H
#define CCOMMUNICATIONCLIENT_H

#include "CommunicationClientExport.h"
#include "CommunicationException.h"

#include <QObject>
#include <QThread>

class CAbstractCommunication;
class CAbstractCommunicationConf;

class COMMUNICATIONCLIENT_EXPORT CCommunicationClient : public QObject
{
    Q_OBJECT
public:
    CCommunicationClient(QObject* parent = nullptr);
    ~CCommunicationClient(void);

    /**
     * @brief Connect       建立连接
     * @param vConfig       [in]通信配置
     * @param vTimeout      [in]连接超时，单位：ms
     * @return
     */
    bool Connect(const CAbstractCommunicationConf& vConfig, quint64 vTimeout = 5000);

    /**
     * @brief Disconnect    断开连接
     * @param vTimeout      [in]断开连接超时，单位：ms
     * @return
     */
    bool Disconnect(quint64 vTimeout = 3000);

    /**
     * @brief SendData      发送数据
     * @param vDataArray    [in]数据内容
     * @param vTimeout      [in]发送超时，单位：ms
     * @return
     */
    bool SendData(const QByteArray& vDataArray, quint64 vTimeout = 5000);

    /**
     * @brief IsConnected   是否已连接
     * @return
     */
    bool IsConnected(void);

    /**
     * @brief GetLastErrorDescribe  获取最新错误描述
     * @return
     */
    QString GetLastErrorDescribe(void);

signals:
    /**
     * @brief forDataReceived       数据接收信号
     * @param vDataArray            [out]接收到的数据内容，统一使用uft8
     * @param vDataLength           [out]接收到的数据长度
     */
    void forDataReceived(QByteArray vDataArray, quint64 vDataLength);

    /**
     * @brief forExceptionTriggered 异常触发信号
     * @param vException            [out]异常信息
     */
    void forExceptionTriggered(CCommunicationException vException);

private:
    bool CreateCommunication(const CAbstractCommunicationConf& vConfig);
    void DestoryCommunication(void);

private:
    CAbstractCommunication* m_CommunicationPTR = nullptr;
    QString m_LastErrorDescribe = "";
};

#endif // CCOMMUNICATIONCLIENT_H
