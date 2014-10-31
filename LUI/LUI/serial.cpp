#include <QErrorMessage>
#include <QMessageBox>
#include <QTextStream>
#include <QtDebug>

#include "serial.h"

Serial::Serial(QWidget *parent) :
    QSerialPort(NULL),
    my_parent_widget(parent),
    waiting_data(QList<QByteArray>()),
    waiting_data_transmitted(false)
{
    this->port_descr = new QStringList();
    this->is_open = false;
    this->is_configured = false;
    //automatically close port after successful write operation
    QObject::connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(closePort()));
}

Serial::~Serial(){
    this->closePort();
    delete this->port_descr;
}

void Serial::configurePort(){
    if(!this->is_configured){
        this->setPort(*this->current_port_info);
        this->setBaudRate(QSerialPort::Baud115200);
        this->setDataBits(QSerialPort::Data8);
        this->setFlowControl(QSerialPort::NoFlowControl);
        this->setParity(QSerialPort::NoParity);
        this->setStopBits(QSerialPort::OneStop);
        this->is_configured = true;
    }
}

bool Serial::openAndWrite(const QByteArray &data){
    this->configurePort();
    if(this->is_open){
        this->waiting_data.append(data);
        //qDebug() << "waiting_data" << this->waiting_data.length();
        return true;
    }
    if(!this->open(QIODevice::ReadWrite)){
        /*QErrorMessage *err_msg = new QErrorMessage();
        err_msg->showMessage("Could not open serial Port.");
        delete err_msg;*/
        QMessageBox::critical(this->my_parent_widget, "Fehler", "Die Verbindung konnte nicht hergestellt werden.");
        qDebug() << "open failed: " << this->errorString();
        return false;
    }
    this->is_open = true;
    qDebug() << "Serial::openAndWrite: data:'" << data.toHex() << "'";
    this->write(data);
    return true;
}

bool Serial::isOpen()
{
    return this->is_open;
}

void Serial::closePort(){
    if(this->waiting_data_transmitted){
        this->waiting_data.removeFirst();
        //qDebug() << "sill waiting" << this->waiting_data.length();
    }
    if(!this->waiting_data.isEmpty()){
        //qDebug() << "write waiting_data" << this->waiting_data.first().toHex();
        this->write(this->waiting_data.first());
        this->waiting_data_transmitted = true;
    } else {
        this->waiting_data_transmitted = false;
        if(this->is_open){
            this->close();
            this->is_open = false;
        }
    }
}

void Serial::refreshPortData(){
    refreshPorts();
    updatePortDescription();
}

void Serial::refreshPorts(){
    this->is_configured = false;
    this->ports = QSerialPortInfo::availablePorts();
}

void Serial::setCurrentPort(int index){
    if((index < 0) or (index >= this->ports.size())){
        return;
    }
    if(this->current_port_info != &this->ports[index]){
        this->current_port_info = &this->ports[index];
        qDebug() << "Serial::setCurrentPort: current Port set to" << this->current_port_info->portName();
        this->is_configured = false;
        emit currentPortChanged(index);
    }
}

void Serial::updatePortDescription(){
    this->port_descr->clear();
    foreach(QSerialPortInfo port, this->ports){
        QString descr = port.portName();
        descr.append(" (");
        descr.append(port.description());
        descr.append(")");
        port_descr->append(descr);
    }
    emit updatedPortDescription(*this->port_descr);
}
