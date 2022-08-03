#include "motordriver.h"

MotorDriver::MotorDriver(QObject *parent) : QObject(parent)
  ,isInit(false)
{
    spd_array_.spd = 0;

    tor_array_.torque = 0;


    //acc_array_.acc = 0;
    recv_spd_.spd = 0;
    serial_port_ = nullptr;


}

bool MotorDriver::init()
{
    serial_port_ = new QSerialPort;
    connect(this->serial_port_,SIGNAL(readyRead()),this,SLOT(resolveDataFromSerialport()));

    if (!serial_port_->isOpen()){

        serial_port_->setPortName(port_name);
        serial_port_->setBaudRate(baud_rate.toInt());
        serial_port_->setParity(QSerialPort::OddParity);


        if(!serial_port_->open(QIODevice::ReadWrite)){

            emit sendErrText(serial_port_->errorString());
            return false;

        }

    }
    qDebug()<<port_name<<" has init";
    isInit = true;
    return true;
}

QByteArray MotorDriver::calSpdData(QString spd)
{
    // fw 20nm
    QByteArray spd_arr;
    spd_array_.spd = (spd.toDouble() /0.2)*256;//*256相当于左移8位
    spd_arr.resize(5);

    spd_arr[0] = 0x4D;
    spd_arr[1] = spd_array_.array[3];
    spd_arr[2] = spd_array_.array[2];
    spd_arr[3] = spd_array_.array[1];
    // cal total
    uchar t = 0x00;
    for (uint i = 1; i < 4;++i){
        t += spd_arr[i];
    }
    spd_arr[4] = t;
    return spd_arr;
}

QByteArray MotorDriver::calTorData(QString tor)
{
    //flywheel 20nm
    QByteArray tor_arr;

    tor_array_.torque = qint32((tor.toDouble()*1000 / resolution_ratio)*256);//力矩编码分辨率为0.853uNm
    tor_arr.resize(5);
    tor_arr[0] = 0xFF;
    tor_arr[1] = tor_array_.array[3];
    tor_arr[2] = tor_array_.array[2];
    tor_arr[3] = tor_array_.array[1];
    // cal total
    uchar t = 0xFF;
    for (uint i = 1; i < 4;++i){
        t += tor_arr[i];
    }
    tor_arr[4] = t;

    return tor_arr;
}

void MotorDriver::writeSerialPort(QByteArray data)
{
    QMutexLocker locker(&mutex_);
//    qDebug()<<"lock!";
    if(serial_port_->write(data) != data.size()){
        emit sendErrText(QString("driver control error"));
    }
    ctl_num_++;
}

void MotorDriver::ctlMotorSpd(double spd)
{
    qDebug()<<"stl spd:"<<spd;
    if (!isInit){
        emit sendErrText(QString("driver not init"));
    }
    else{
//        QByteArray spd_str = calSpdData(QString::number(spd));

//        emit sendErrText(QString::number(spd));
//        if(serial_port_->write(spd_str) != spd_str.size()){
//            emit sendErrText(QString("driver control error"));
//        }
//        getMotorData();
        writeSerialPort(calSpdData(QString::number(spd)));
    }
}

void MotorDriver::ctlMotorTor(double tor)
{
    if (!isInit){
        emit sendErrText(QString("driver not init"));
    }
    else{
//        QByteArray tor_str = calTorData(QString::number(tor));
//        if(serial_port_->write(tor_str) != tor_str.size()){
//            emit sendErrText(QString("driver control error"));
//        }
//        getMotorData();
        writeSerialPort(calTorData(QString::number(tor)));
    }
}
//查询指令

void MotorDriver::getMotorData()
{
    // flywheel 20nm
    QByteArray get_str;
    get_str.resize(1);
    get_str[0] = 0xFF;
//    serial_port_->write(get_str);
    writeSerialPort(get_str);
}
//滑行指令
void MotorDriver::setMotorHx()
{
    // flywheel 20nm
    QByteArray get_str;
    get_str.resize(1);
    get_str[0] = 0xFF;
//    serial_port_->write(get_str);
    writeSerialPort(get_str);

}
void MotorDriver::resolveDataFromSerialport()
{
    mutex_.lock();
    QByteArray tmp_data = serial_port_->readAll();
    mutex_.unlock();
//    sendErrText("recv num:"+ QString::number(++recv_num_) + " ctl num:" +QString::number(ctl_num_));
    for (int i = 0;i < tmp_data.size();++i){
        recv_data_buf.push_back(tmp_data.at(i));
        //flywheel 20nm
        //a full frame detected
        if (recv_data_buf.size() >= 6){
            //check frame
            uint8_t t = 0xFF;
            for (uint i = 0; i < 5;++i){
                t += recv_data_buf[i];
            }
            if (t != uint8_t(recv_data_buf[5])){
                recv_data_buf = recv_data_buf.mid(1);
                emit sendErrText("recv message error! chack communication!");
                continue;
            }

            recv_spd_.array[3] = recv_data_buf[0];
            recv_spd_.array[2] = recv_data_buf[1];
            recv_spd_.array[1] = recv_data_buf[2];

            emit sendMotorSpd((recv_spd_.spd>>8 )* spd_resolution_ratio);

            recv_cur_.array[0] = recv_data_buf[3];

            emit sendMotorCur(recv_cur_.cur * current_resolution_ratio);


            uchar status = recv_data_buf[4];
            emit sendMotorStatus(status);

            if (recv_data_buf.size() == 6){
                recv_data_buf.clear();
            }
            else{
                recv_data_buf = recv_data_buf.mid(6);
            }
        }
    }
}

