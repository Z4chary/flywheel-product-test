#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <QObject>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <QMutexLocker>
#include <QMutex>

class MotorDriver : public QObject
{
    Q_OBJECT
public:
    explicit MotorDriver(QObject *parent = 0);

    ~MotorDriver(){
        disconnect(this->serial_port_,SIGNAL(readyRead()),this,SLOT(resolveDataFromSerialport()));
        if (serial_port_ != nullptr && serial_port_->isOpen()){
            serial_port_->close();
            delete serial_port_;
            qDebug()<<port_name<<" has close";
        }
        else if(serial_port_ != nullptr && !serial_port_->isOpen()){
            delete serial_port_;
        }
        else{
            serial_port_ = nullptr;
        }

    }


    void setResolution_ratio(const double resolution_ratio)
    {
        this->resolution_ratio = resolution_ratio;
    }

    void setCurrentResolutionRatio(double cur_res){
        this->current_resolution_ratio = cur_res;
    }

    void setSpdResolutionRatio(double spd_res){
        this->spd_resolution_ratio = spd_res;
    }

    //20mN
    QByteArray calSpdData(QString spd);
    QByteArray calTorData(QString tor);

    void writeSerialPort(QByteArray);

    void setPortName(QString str){
        port_name = str;
    }

    void setBaudRate(QString baud){
        baud_rate = baud;
    }



signals:
    void sendErrText(QString);

    void sendMotorSpd(double);
    void sendMotorCur(double);
    void sendMotorStatus(uchar);


public slots:
    //500mN
     void ctlMotorSpd(double spd = 0);
     void ctlMotorTor(double tor = 0);
     void getMotorData();
     void setMotorHx();
     void resolveDataFromSerialport();
     bool init();
private:

    QSerialPort*    serial_port_;
    bool            isInit;
    QString         port_name;
    QString         baud_rate;
    QByteArray      recv_data_buf;
    double          resolution_ratio = 0.0 ;          //力矩分辨率
    double          current_resolution_ratio = 0.0;   //电流分辨率
    double          spd_resolution_ratio = 0.0;         //转速分辨率

    int ctl_num_ = 0;
    int recv_num_ = 0;
    QMutex mutex_;
    //----------flywheel 500Nm start--------
    union{
        uchar   array[4];
        int32_t  spd;
    }spd_array_;

    union tor_array{
        uchar   array[4];
        int32_t  torque;
    }tor_array_;

    union recv_spd{
        qint32 spd;
        uchar   array[4];
    }recv_spd_;

    union recv_cur{
        quint8  cur;
        uchar   array[1];
    }recv_cur_;
    //----------flywheel 500Nm end----------
};

#endif // MOTORDRIVER_H
