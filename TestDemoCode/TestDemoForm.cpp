#include "TestDemoForm.h"
#include "ui_TestDemoForm.h"

#include "CommunicationConfForTCP.h"
#include "CommunicationConfForSerialPort.h"

#include "QDateTime"
#include <QtSerialPort/QSerialPortInfo>

CTestDemoForm::CTestDemoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CTestDemoForm)
{
    ui->setupUi(this);

    InitSerialPortName();

    ui->pushButton_Connect->setEnabled(true);
    ui->pushButton_Disconnect->setEnabled(false);

    connect(&m_Client, &CCommunicationClient::forDataReceived, this, &CTestDemoForm::onDataReceived);
    connect(&m_Client, &CCommunicationClient::forExceptionTriggered, this, &CTestDemoForm::onExceptionTriggered);

}

CTestDemoForm::~CTestDemoForm()
{
    delete ui;
}

void CTestDemoForm::onDataReceived(QByteArray vDataArray, quint64 vDataLength)
{
    Q_UNUSED(vDataLength)

    QString tDataReceived = vDataArray.toHex(' ');
    QString tPrintInfo = QString("%1 [recv]：%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), tDataReceived);
    ui->plainTextEdit_Data->appendPlainText(tPrintInfo);
}

void CTestDemoForm::onExceptionTriggered(CCommunicationException vException)
{
    QString tPrintInfo = QString("%1 [error]：%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), vException.GetExceptionDescribe());
    ui->plainTextEdit_Data->appendPlainText(tPrintInfo);
}

void CTestDemoForm::on_comboBox_currentIndexChanged(int index)
{
    QString tCurrentText = ui->comboBox->itemText(index);

    if(tCurrentText.contains("TCP"))
    {
        ui->stackedWidget->setCurrentWidget(ui->page_TCP);
    }

    if(tCurrentText.contains("SerialPort"))
    {
        ui->stackedWidget->setCurrentWidget(ui->page_SerialPort);
    }
}

void CTestDemoForm::on_pushButton_Update_clicked()
{
    InitSerialPortName();
}

void CTestDemoForm::on_pushButton_Connect_clicked()
{
    if(m_Client.IsConnected())
        return;

    QString tCurrentText = ui->comboBox->currentText();
    bool tConnectResult = false;

    ui->pushButton_Connect->setEnabled(false);

    if(tCurrentText.contains("TCP"))
    {
        tConnectResult = ConnectTCP();
    }

    if(tCurrentText.contains("SerialPort"))
    {
        tConnectResult = ConnectSerialPort();
    }

    ui->pushButton_Connect->setEnabled(!tConnectResult);
    ui->pushButton_Disconnect->setEnabled(tConnectResult);

    ui->comboBox->setEnabled(!tConnectResult);
    ui->stackedWidget->setEnabled(!tConnectResult);
}


void CTestDemoForm::on_pushButton_Disconnect_clicked()
{
    ui->pushButton_Disconnect->setEnabled(false);

    bool tDisconnectResult = m_Client.Disconnect();
    if(!tDisconnectResult)
    {
        QString tLastErrorSTR = m_Client.GetLastErrorDescribe();
        QString tPrintInfo = QString("%1 [error]：%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), tLastErrorSTR);
        ui->plainTextEdit_Data->appendPlainText(tPrintInfo);
    }

    ui->pushButton_Connect->setEnabled(tDisconnectResult);
    ui->pushButton_Disconnect->setEnabled(!tDisconnectResult);

    ui->comboBox->setEnabled(tDisconnectResult);
    ui->stackedWidget->setEnabled(tDisconnectResult);
}

void CTestDemoForm::on_pushButton_Send_clicked()
{
    QString tSendData = ui->plainTextEdit_Send->toPlainText();
    QByteArray tSendDataHex = QByteArray::fromHex(tSendData.toUtf8());

    QString tPrintInfo = QString("%1 [send]：%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), tSendDataHex.toHex(' '));
    ui->plainTextEdit_Data->appendPlainText(tPrintInfo);

    bool tSendResult = m_Client.SendData(tSendDataHex);
    if(!tSendResult)
    {
        QString tLastErrorSTR = m_Client.GetLastErrorDescribe();
        QString tPrintInfo = QString("%1 [error]：%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), tLastErrorSTR);
        ui->plainTextEdit_Data->appendPlainText(tPrintInfo);
    }
}

void CTestDemoForm::InitSerialPortName()
{
    //先清空
    ui->comboBox_SerialPort->clear();

    QList<QSerialPortInfo> tInfoList = QSerialPortInfo::availablePorts();
    for(int i = 0; i < tInfoList.count(); i++)
    {
        QString tPortName = tInfoList.at(i).portName();
        ui->comboBox_SerialPort->addItem(tPortName);
    }
}

bool CTestDemoForm::ConnectTCP()
{
    CCommunicationConfForTCP tConfig;

    tConfig.m_PeerAddress = ui->lineEdit_PeerAddress->text();
    tConfig.m_PeerPort = ui->lineEdit_PeerPort->text().toUInt();

    bool tConnectResult = m_Client.Connect(tConfig);
    if(!tConnectResult)
    {
        QString tLastErrorSTR = m_Client.GetLastErrorDescribe();
        QString tPrintInfo = QString("%1 [error]：%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), tLastErrorSTR);
        ui->plainTextEdit_Data->appendPlainText(tPrintInfo);
    }

    return tConnectResult;
}

bool CTestDemoForm::ConnectSerialPort()
{
    CCommunicationConfForSerialPort tConfig;

    QString tSerialPort = ui->comboBox_SerialPort->currentText();
    int tSerialPortNum = tSerialPort.mid(QString("COM").length()).toInt();
    if(tSerialPortNum >= 10)
        tSerialPort.push_front("\\\\.\\");

    QSerialPort::DataBits tDataBits = QSerialPort::UnknownDataBits;
    switch(ui->comboBox_DataBit->currentText().toUInt())
    {
    case 5:
        tDataBits = QSerialPort::Data5;
        break;
    case 6:
        tDataBits = QSerialPort::Data6;
        break;
    case 7:
        tDataBits = QSerialPort::Data7;
        break;
    case 8:
        tDataBits = QSerialPort::Data8;
        break;
    default:
        break;
    }

    QSerialPort::Parity tParity = QSerialPort::UnknownParity;
    QString tParitySTR = ui->comboBox_Parity->currentText();
    if(tParitySTR.contains("奇校验"))
        tParity = QSerialPort::OddParity;
    else if(tParitySTR.contains("偶校验"))
        tParity = QSerialPort::EvenParity;
    else
        tParity = QSerialPort::NoParity;

    QSerialPort::StopBits tStopBits = QSerialPort::UnknownStopBits;
    double tStopBitValue = ui->comboBox_StopBit->currentText().toDouble();
    if(qFuzzyCompare(tStopBitValue, 1))
        tStopBits = QSerialPort::OneStop;
    else if(qFuzzyCompare(tStopBitValue, 2))
        tStopBits = QSerialPort::TwoStop;
    else
        tStopBits = QSerialPort::OneAndHalfStop;

    tConfig.m_PortName = tSerialPort;
    tConfig.m_BaudRate = ui->comboBox_BaudRate->currentText().toUInt();
    tConfig.m_DataBits = tDataBits;
    tConfig.m_Parity = tParity;
    tConfig.m_StopBits = tStopBits;
    tConfig.m_FlowControl = QSerialPort::NoFlowControl;

    bool tConnectResult = m_Client.Connect(tConfig);
    if(!tConnectResult)
    {
        QString tLastErrorSTR = m_Client.GetLastErrorDescribe();
        QString tPrintInfo = QString("%1 [error]：%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"), tLastErrorSTR);
        ui->plainTextEdit_Data->appendPlainText(tPrintInfo);
    }

    return tConnectResult;
}


