# QT通用模板

### 串口通信

- .pro工程文件中添加 ```QT += serialport```

##### File mainwindow.h:

```c++
#include <QSerialPort>
#include <QTimer>

public:
	void UART_Init();
	void UART_Task();

private:
    QTimer *ScanTimer = new QTimer(this);                   //实例化一个扫描串口的定时器
    QStringList SerialCom;                					//创建一个串口端口列表
    QSerialPort *PcSerialPort = new QSerialPort(this);  	//实例化一个串口类的对象
```

##### File mainwindow.cpp:

```c++
#include <QMessageBox>
#include <QSerialPortInfo>

//主函数中添加串口初始化
UART_Init();
UART_Task();

/*
 * @brief: 串口初始化
*/
void MainWindow::UART_Init()
{
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    SerialCom << info.portName();
    ui->SerialPortBox->addItems(SerialCom);

    connect(ScanTimer, &QTimer::timeout, this, [=]()mutable
    {
        QStringList SerialPortName;
        const auto infos = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : infos)
        {
           SerialPortName += info.portName();
        }
        //更新串口号
        if(SerialPortName.size() != SerialCom.size())
        {
           SerialCom = SerialPortName;
           ui->SerialPortBox->clear();
           ui->SerialPortBox->addItems(SerialCom);
        }

    });//更新端口号

    ScanTimer->start(500);

    if(PcSerialPort->isOpen())//如果串口已经打开了 先给他关闭了
    {
        PcSerialPort->clear();
        PcSerialPort->close();
    }
    
}

/*
 * @brief: 串口打开/关闭按键
*/
void MainWindow::on_SerialPortButton_clicked()
{
    if (ScanTimer->isActive()) {

        PcSerialPort->setPortName(ui->SerialPortBox->currentText());
        if(!PcSerialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
        {
            QMessageBox::information(this, tr("Warinig"), tr("打开失败"));
            return;
        }
        //打开成功
        //QMessageBox::information(this, tr("Warinig"), "打开成功");
        ScanTimer->stop();
        ui->SerialPortButton->setText(tr("关闭串口"));
        ui->SerialPortBox->setDisabled(1);

        PcSerialPort->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
        PcSerialPort->setDataBits(QSerialPort::Data8);		//数据位为8位
        PcSerialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
        PcSerialPort->setParity(QSerialPort::NoParity);	//无校验位
        PcSerialPort->setStopBits(QSerialPort::OneStop); //一位停止位

    } else {

        PcSerialPort->setPortName(ui->SerialPortBox->currentText());
        PcSerialPort->close();
        ui->SerialPortButton->setText(tr("打开串口"));
        ui->SerialPortBox->setDisabled(0);
        ScanTimer->start();
        QMessageBox::information(this, tr("Warinig"), tr("关闭成功"));
    }
}

/*
 * @brief: 串口任务处理函数
*/
void MainWindow::UART_Task()
{
    //连接信号槽 当下位机发送数据QSerialPortInfo 会发送个 readyRead 信号,我们定义个槽void receiveInfo()解析数据
    connect(PcSerialPort, &QSerialPort::readyRead, this, [=]()mutable
    {
        QByteArray hexData;
        hexData = PcSerialPort->readAll();
        //这里面的协议 你们自己定义就行  单片机发什么 代表什么 我们这里简单模拟一下
        if(hexData.startsWith(0x11) && hexData.endsWith(0x21)) {
            QByteArray info;
            info.resize(3);
            info[0] = 0xA0;
            info[1] = 0xA0;
            info[2] = 0xA1;
            info.data();
            PcSerialPort->write(info, 3);
        }
    });
}
```





