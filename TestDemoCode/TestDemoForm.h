#ifndef TESTDEMOFORM_H
#define TESTDEMOFORM_H

#include "CommunicationClient.h"
#include <QWidget>

namespace Ui {
class CTestDemoForm;
}

class CTestDemoForm : public QWidget
{
    Q_OBJECT

public:
    explicit CTestDemoForm(QWidget *parent = nullptr);
    ~CTestDemoForm();

private slots:
    void onDataReceived(QByteArray vDataArray, quint64 vDataLength);
    void onExceptionTriggered(CCommunicationException vException);

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_Update_clicked();

    void on_pushButton_Connect_clicked();
    void on_pushButton_Disconnect_clicked();

    void on_pushButton_Send_clicked();

private:
    void InitSerialPortName(void);

    bool ConnectTCP(void);
    bool ConnectSerialPort(void);

private:
    Ui::CTestDemoForm *ui;
    CCommunicationClient m_Client;
};

#endif // TESTDEMOFORM_H
