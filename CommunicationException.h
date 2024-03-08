#ifndef CCOMMUNICATIONEXCEPTION_H
#define CCOMMUNICATIONEXCEPTION_H

#include "CommunicationClientExport.h"

#include <QString>

class COMMUNICATIONCLIENT_EXPORT CCommunicationException
{
public:
    CCommunicationException(void);
    CCommunicationException(int vExceptionCode, QString vExceptionDescribe, bool vIsHealthy);

    /**
     * @brief SetException          设置异常信息
     * @param vExceptionCode        [in]异常状态码
     * @param vExceptionDescribe    [in]异常状态描述
     * @param vIsHealthy            [in]异常对象是否健康可运作
     */
    void SetException(int vExceptionCode, QString vExceptionDescribe, bool vIsHealthy);

    /**
     * @brief GetExceptionCode      获取异常状态码
     * @return
     */
    int GetExceptionCode(void);

    /**
     * @brief GetExceptionDescribe  获取异常状态描述
     * @return
     */
    QString GetExceptionDescribe(void);

    /**
     * @brief IsContinueToOperate   是否可继续运作
     * @return
     */
    bool IsContinueToOperate(void);

private:
    int     m_ExceptionCode = 999;
    QString m_ExceptionDescribe = "";
    bool    m_IsContinueToOperate = false;
};

#endif // CCOMMUNICATIONEXCEPTION_H
