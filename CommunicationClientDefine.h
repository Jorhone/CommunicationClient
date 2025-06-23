#ifndef COMMUNICATIONCLIENTDEFINE_H
#define COMMUNICATIONCLIENTDEFINE_H

#include <QHash>

namespace nsCommunicationClient {

/**
 * 数据类型定义
 */

typedef enum
{
    e_Result_Success = 0,       //成功
    e_Result_Unknown,           //未知异常
    e_Result_ConfigError,       //配置错误
    e_Result_ConnectionInUse,   //连接正在使用
    e_Result_ConnectTimeout,    //连接超时
    e_Result_WriteInhibit,      //禁止写操作
    e_Result_SendException,     //发送异常
    e_Result_WriteBufferFull,   //写通道已满
    e_Result_ReadBufferFull,    //读通道已满

    e_Result_SerialPortCannotBeOpened,  //串口无法打开

}eCommunicationResult;

typedef enum
{
    e_Medium_Unknown,
    e_Medium_TCP,
    e_Medium_UDP,
    e_Medium_SerialPort,

}eCommunicationMedium;

//---------------------------------------------------------------------------

/**
 * 数据结构/类定义
 */

class CCommunicationEnumConvert
{
public:
    static QString EnumConvertToSTR(eCommunicationResult vCommunicationResult)
    {
        QHash<eCommunicationResult, QString> tCommunicationResultHash{
            {e_Result_Success,          "成功"},
            {e_Result_Unknown,          "未知异常"},
            {e_Result_ConfigError,      "配置错误"},
            {e_Result_ConnectionInUse,  "连接正在使用"},
            {e_Result_ConnectTimeout,   "连接超时"},
            {e_Result_WriteInhibit,     "禁止写操作"},
            {e_Result_SendException,    "发送异常"},
            {e_Result_WriteBufferFull,  "写通道已满"},
            {e_Result_ReadBufferFull,   "读通道已满"},
            {e_Result_SerialPortCannotBeOpened, "串口无法打开"},
        };

        return tCommunicationResultHash.value(vCommunicationResult);
    }

    static QString EnumConvertToSTR(eCommunicationMedium vCommunicationMedium)
    {
        QHash<eCommunicationMedium, QString> tCommunicationMediumHash{
            {e_Medium_Unknown,          "未知"},
            {e_Medium_TCP,              "tcp"},
            {e_Medium_UDP,              "udp"},
            {e_Medium_SerialPort,       "串口"},
        };

        return tCommunicationMediumHash.value(vCommunicationMedium);
    }
};

} // nsCommunicationClient

#endif // COMMUNICATIONCLIENTDEFINE_H
