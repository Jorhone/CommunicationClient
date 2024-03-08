#include "CommunicationException.h"

#include <QObject>

CCommunicationException::CCommunicationException()
{
    qRegisterMetaType<CCommunicationException>("CCommunicationException");
}

CCommunicationException::CCommunicationException(int vExceptionCode, QString vExceptionDescribe, bool vIsHealthy)
    : m_ExceptionCode(vExceptionCode), m_ExceptionDescribe(vExceptionDescribe), m_IsContinueToOperate(vIsHealthy)
{
    qRegisterMetaType<CCommunicationException>("CCommunicationException");
}

void CCommunicationException::SetException(int vExceptionCode, QString vExceptionDescribe, bool vIsHealthy)
{
    m_ExceptionCode = vExceptionCode;
    m_ExceptionDescribe = vExceptionDescribe;
    m_IsContinueToOperate = vIsHealthy;
}

int CCommunicationException::GetExceptionCode()
{
    return m_ExceptionCode;
}

QString CCommunicationException::GetExceptionDescribe()
{
    return m_ExceptionDescribe;
}

bool CCommunicationException::IsContinueToOperate()
{
    return m_IsContinueToOperate;
}
