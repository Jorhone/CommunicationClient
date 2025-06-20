#include "DataHandleThread.h"

void CDataHandleThread::SetThreadFunc(const CAbstractThreadFunc *vThreadFuncPTR, const QVariant &vFuncData)
{
    m_ThreadFuncPTR = (CAbstractThreadFunc*)vThreadFuncPTR;
    m_FuncData = vFuncData;
}

void CDataHandleThread::run()
{
    if(m_ThreadFuncPTR != nullptr)
        m_ThreadFuncPTR->ThreadFunction(this, m_FuncData);
}
