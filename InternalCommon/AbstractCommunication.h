#ifndef CABSTRACTCOMMUNICATION_H
#define CABSTRACTCOMMUNICATION_H

#include "CommunicationClientDefine.h"
#include "CommunicationException.h"

#include <QString>

class CAbstractCommunicationConfig;

class CAbstractCommunication
{
public:
    virtual ~CAbstractCommunication(){};

    /**
     * @brief Connect       建立连接
     * @param vConfig       [in]通信配置
     * @param vTimeout      [in]连接超时，单位：ms
     * @return
     */
    virtual nsCommunicationClient::eCommunicationResult Connect(const CAbstractCommunicationConfig& vConfig, quint64 vTimeout) = 0;

    /**
     * @brief Disconnect    断开连接
     * @param vTimeout      [in]断开连接超时，单位：ms
     * @return
     */
    virtual nsCommunicationClient::eCommunicationResult Disconnect(quint64 vTimeout) = 0;

    /**
     * @brief SendData      发送数据
     * @param vDataArray    [in]数据内容
     * @param vTimeout      [in]发送超时，单位：ms
     * @return
     */
    virtual nsCommunicationClient::eCommunicationResult SendData(const QByteArray& vDataArray, quint64 vTimeout) = 0;

    /**
     * @brief GetCommunicationMedium    获取通信媒介/通信类型
     * @return
     */
    virtual nsCommunicationClient::eCommunicationMedium GetCommunicationMedium(void) = 0;

    /**
     * @brief IsConnected   是否已连接
     * @return
     */
    virtual bool IsConnected(void) = 0;

    //信号
    //-------------------------------------------------------------------------

    /**
     * @brief forDataReceived       数据接收信号
     * @param vDataArray            [out]接收到的数据内容，统一使用uft8
     * @param vDataLength           [out]接收到的数据长度
     */
    virtual void forDataReceived(QByteArray vDataArray, quint64 vDataLength) = 0;

    /**
     * @brief forExceptionTriggered 异常触发信号
     * @param vException            [out]异常信息
     */
    virtual void forExceptionTriggered(CCommunicationException vException) = 0;

    //槽
    //-------------------------------------------------------------------------

};

#endif // CABSTRACTCOMMUNICATION_H
