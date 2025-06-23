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

    virtual nsCommunicationClient::eCommunicationResult Connect(const CAbstractCommunicationConf& vConfig, quint64 vTimeout);
    virtual nsCommunicationClient::eCommunicationResult Disconnect(quint64 vTimeout);
    virtual nsCommunicationClient::eCommunicationResult SendData(const QByteArray& vDataArray, quint64 vTimeout);
    virtual nsCommunicationClient::eCommunicationMedium GetCommunicationMedium(void);
    virtual bool IsConnected(void);

protected:
    virtual void ThreadFunction(const CDataHandleThread* vThreadPTR, const QVariant& vFuncData);

signals:
    virtual void forDataReceived(QByteArray vDataArray, quint64 vDataLength);
    virtual void forExceptionTriggered(CCommunicationException vException);

private slots:
    void onDataReceived(void);
    void onExceptionTriggered(QSerialPort::SerialPortError vErrorCode);

private:
    QSerialPort* m_SerialPortPTR = nullptr;
    CDataHandleThread* m_DataHandleThreadPTR = nullptr;
    AsyncSession<int> m_AsyncSession;
};

#endif // CCOMMUNICATIONFORSERIALPORT_H
