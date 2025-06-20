#ifndef CDATAHANDLETHREAD_H
#define CDATAHANDLETHREAD_H

#include <QThread>
#include <QVariant>

class CAbstractThreadFunc;

class CDataHandleThread : public QThread
{
    Q_OBJECT
public:
    void SetThreadFunc(const CAbstractThreadFunc* vThreadFuncPTR, const QVariant& vFuncData = QVariant());
    void run() override;

private:
    CAbstractThreadFunc* 	m_ThreadFuncPTR = nullptr; 	//线程任务
    QVariant                m_FuncData;                 //线程任务数据
};

class CAbstractThreadFunc
{
protected:
    virtual void ThreadFunction(const CDataHandleThread* vThreadPTR, const QVariant& vFuncData)
    {
        Q_UNUSED(vThreadPTR)
        Q_UNUSED(vFuncData)
    }

private:
    friend class CDataHandleThread;
};

#endif // CDATAHANDLETHREAD_H
