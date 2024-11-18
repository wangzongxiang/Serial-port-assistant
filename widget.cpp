#include "widget.h"
#include "ui_widget.h"
#include <QList>
#include <QTime>
#include <QMessageBox>
#include <QTimer>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->serialport=new QSerialPort(this);
    connect(ui->openSp,&QPushButton::clicked,[=](){
        this->openserialport();
    });
    connect(ui->checkSp,&QPushButton::clicked,[=](){
        this->checkserialport();
    });
    connect(ui->recievefile,&QPushButton::clicked,[=](){
        if(ui->openSp->text()=="关闭串口"){
                this->readdata();
        }else{
            QMessageBox* box=new QMessageBox(QMessageBox::Warning,"warn","你需要先打开串口");
            box->show();
        }
    });
    connect(ui->clearrecievefile,&QPushButton::clicked,[=](){
        if(ui->openSp->text()=="关闭串口"){
            ui->recievetext->clear();
        }else{
            QMessageBox* box=new QMessageBox(QMessageBox::Warning,"warn","你需要先打开串口");
            box->show();
        }
    });

    connect(ui->sendbutton,&QPushButton::clicked,[=](){
        if(ui->openSp->text()=="关闭串口"){
             this->senddata();
        }else{
            QMessageBox* box=new QMessageBox(QMessageBox::Warning,"warn","你需要先打开串口");
            box->show();
        }
    });
    connect(ui->delbutton,&QPushButton::clicked,[=](){
        ui->sendtext->clear();
    });
    QTimer* t=new QTimer(this);
    connect(ui->autosend,&QCheckBox::clicked,[=](){
        if(ui->openSp->text()=="关闭串口"){
            if(ui->autosend->checkState()==Qt::Checked){
                int ms=ui->timems->value();
                if(ui->choosetime->currentText()=="s") ms=ms*1000;
                if(ms==0) return;
                this->senddata();
                ui->timems->setEnabled(false);
                t->start(ms);
            }else{
                ui->timems->setEnabled(true);
                t->stop();
            }
        }
    });
    connect(t,&QTimer::timeout,[=](){
        this->senddata();
    });

}
void Widget::checkserialport() {
    QSerialPortInfo* info=new QSerialPortInfo();
    QList<QSerialPortInfo> list= info->availablePorts();
    for(int i=0;i<list.size();i++){
        ui->serialport->addItem(list[i].portName());
    }
}

void Widget::openserialport(){
    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits databits;
    QSerialPort::StopBits stopbits;
    QSerialPort::Parity parity;
    if(ui->baudrate->currentText()=="1200"){
        baudRate=QSerialPort::Baud1200;
    }else if(ui->baudrate->currentText()=="2400"){
        baudRate=QSerialPort::Baud2400;
    }else if(ui->baudrate->currentText()=="4800"){
        baudRate=QSerialPort::Baud4800;
    }else if(ui->baudrate->currentText()=="9600"){
        baudRate=QSerialPort::Baud9600;
    }else if(ui->baudrate->currentText()=="19200"){
        baudRate=QSerialPort::Baud19200;
    }else if(ui->baudrate->currentText()=="38400"){
        baudRate=QSerialPort::Baud38400;
    }else if(ui->baudrate->currentText()=="57600"){
        baudRate=QSerialPort::Baud57600;
    }else{
        baudRate=QSerialPort::Baud115200;
    }

    if(ui->datasite->currentText()=="5"){
        databits=QSerialPort::Data5;
    }else if(ui->datasite->currentText()=="6"){
        databits=QSerialPort::Data6;
    }else if(ui->datasite->currentText()=="7"){
        databits=QSerialPort::Data7;
    }else{
        databits=QSerialPort::Data8;
    }

    if(ui->stopsite->currentText()=="1"){
        stopbits=QSerialPort::OneStop;
    }else if(ui->stopsite->currentText()=="1.5(windows)"){
        stopbits=QSerialPort::OneAndHalfStop;
    }else{
        stopbits=QSerialPort::TwoStop;
    }

    if(ui->checksite->currentText()=="none"){
        parity=QSerialPort::NoParity;
    }else if(ui->checksite->currentText()=="奇校验"){
        parity=QSerialPort::OddParity;
    }else{
        parity=QSerialPort::EvenParity;
    }

    this->serialport->setPortName(ui->serialport->currentText());//名字
    this->serialport->setBaudRate(baudRate);//波特率
    this->serialport->setDataBits(databits);//数据位
    this->serialport->setStopBits(stopbits);//停止位
    this->serialport->setParity(parity);//校验位

    if(ui->openSp->text()=="打开串口"){
        if(this->serialport->open(QIODeviceBase::ReadWrite)){
            ui->openSp->setText("关闭串口");
            ui->serialport->setEnabled(false);//打开后不允许更改串口号
        }
    }else{
        this->serialport->close();
        ui->serialport->setEnabled(true);
        ui->openSp->setText("打开串口");
    }
}
void Widget::readdata(){
    QByteArray arr=this->serialport->readAll();
    QString str;
    if(ui->hexdata->checkState()==Qt::Checked){//没选中十六进制
        str=QString(arr)+"\n";
        if(ui->timec->checkState()==Qt::Checked){//选中时间戳
            QDateTime times= QDateTime::currentDateTime();
            str="["+times.toString("yyyy-MM-dd hh:mm:ss")+"]"+"   "+str;
        }
    }else{//选中十六进制
        str=arr.toHex().toUpper()+"\n";//转为十六进制大写
        if(ui->timec->checkState()==Qt::Checked){//选中时间戳
            QDateTime times= QDateTime::currentDateTime();
            str="["+times.toString("yyyy-MM-dd HH:mm:ss")+"]"+"   "+str;
        }
    }
    ui->recievetext->insertPlainText(str);
    ui->recievetext->moveCursor(QTextCursor::End);
}
void Widget::senddata(){
    QString str= ui->sendtext->toPlainText();
    QByteArray arr;
    if(ui->hexdata->checkState()==Qt::Checked){//十六进制数据
        arr=QByteArray::fromHex(str.toUtf8()).data();
    }else{//不是十六进制数据
        arr=str.toLocal8Bit().data();
    }
    this->serialport->write(arr);
}

Widget::~Widget()
{
    delete ui;
}
