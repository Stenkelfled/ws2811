#ifndef SERIAL_H
#define SERIAL_H

#include <QByteArray>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>

class Serial : public QSerialPort
{
    Q_OBJECT
public:
    explicit Serial(QObject *parent = 0);
    ~Serial();
    bool openAndWrite(const QByteArray &data);
    bool isOpen();

signals:
    void updatedPortDescription(QStringList &description);
    void currentPortChanged(int index);

public slots:
    void refreshPortData(void);
    void setCurrentPort(int index);
    void closePort(void);

private:
    void configurePort(void);
    void refreshPorts(void);
    void updatePortDescription(void);
    QList<QSerialPortInfo> ports;
    QStringList *port_descr;
    QSerialPortInfo *current_port_info;
    bool is_open;
    bool is_configured;
    QList<QByteArray> waiting_data;
    bool waiting_data_transmitted;
};

#endif // SERIAL_H
