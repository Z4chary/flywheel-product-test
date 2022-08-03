#ifndef QCANDRIVER_H
#define QCANDRIVER_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QTimer>
#include <QMap>
#include <QVector>
//#include <algorithm>

#include <windows.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <tchar.h>
//#include <string.h>
#include "AdvCan.h"
//#include "AdvCANIO.h"

typedef void *HANDLE;



#define SUCCESS                           0      //Status definition : success
#define OPERATION_ERROR                   -1     //Status definition : device error or parameter error
#define TIME_OUT                          -2


class QCan1680 : public QObject{
    Q_OBJECT
public:
    explicit QCan1680(QObject *parent = 0);
    ~QCan1680(){
        if (is_init){
            acCanClose(m_canA);
            qDebug()<<m_can_name<<" CAN CLOSED!";
        }
    }

    bool canInit(int channel);

    HANDLE acCanOpen(int channel, BOOL synchronization);
    int acCanClose(HANDLE hDevice);
    int acSetBaud(HANDLE hDevice, unsigned int nBaud);
    int acSetTimeOut(HANDLE hDevice, ULONG ulReadTimeOut, ULONG ulWriteTimeOut);
    int acCanRead(HANDLE hDevice, canmsg_t *pbyData, ULONG ulReadCount, ULONG *pulNumberofRead, OVERLAPPED *ov);
    int acCanWrite(HANDLE hDevice, canmsg_t *pbyData, ULONG ulWriteCount, ULONG *pulNumberofWritten, OVERLAPPED *ov);
    int acEnterResetMode(HANDLE hDevice);
    int acEnterWorkMode(HANDLE hDevice);
public slots:
    void setCanFrame(canmsg_t); //调用该函数，发送一帧数据
    void start();               //控制定时器开始读取
    void stop();                //控制定时器停止读取
private slots:
    void readFrame();           //从CAN中得到一帧，通过计时器触发,结果通过getCanFrame发送



signals:
    void logMsg(QString);

    void getCanFrame(canmsg_t); //通过该信号获取数据

private:
    //init
    bool is_init = false;       //初始化标志位
    bool is_start = false;      //开始读取标志位
    //CAN 对象句柄
    HANDLE m_canA;
//    HANDLE m_canB;
    QString m_can_name;
    QTimer *m_timer_getFrame = nullptr;

};

class QCanDriver : public QObject
{
    Q_OBJECT
public:
    explicit QCanDriver(QObject *parent = 0);

    void setResolution_ratio(const double val){
        this->tor_resolution_ratio = val;
    }

    void setCurrentResolutionRatio(double cur_res){
        this->current_resolution_ratio = cur_res;
    }

    void setSpdResolutionRatio(double spd_res){
        this->spd_resolution_ratio = spd_res;
    }

    double getResolution_ratio(){
         return this->tor_resolution_ratio;
    }

    double getCurrentResolutionRatio(){
        return this->current_resolution_ratio;
    }

    double getSpdResolutionRatio(){
        return this->spd_resolution_ratio;
    }


    //channel = 1~6 ,state = true & false
    void setFWState(uint channel,bool state,uchar id){
        if (channel == 1){
            if (state == false){
                fw1_init = false;
            }
            fw1_isOpen = state;
            fw1_id = id;
        }
        else if (channel == 2){
            if (state == false){
                fw2_init = false;
            }
            fw2_isOpen = state;
            fw2_id = id;
        }
        else if (channel == 3){
            if (state == false){
                fw3_init = false;
            }
            fw3_isOpen = state;
            fw3_id = id;
        }
        else if (channel == 4){
            if (state == false){
                fw4_init = false;
            }
            fw4_isOpen = state;
            fw4_id = id;
        }
        else if (channel == 5){
            if (state == false){
                fw5_init = false;
            }
            fw5_isOpen = state;
            fw5_id = id;
        }
        else if (channel == 6){
            if (state == false){
                fw6_init = false;
            }
            fw6_isOpen = state;
            fw6_id = id;
        }
    }

signals:
    void setCanFrame(canmsg_t); //通过该信号与CAN卡控制信号连接
    void sendSolvedData(int id,QVector<uchar>);

    void logMsg(QString);       //记录信息用

public slots:
    void init();
    void sendRecoveryFrame(int id);
    void sendControlFrame(uint id, int mode, double data);
    void sendGetDataFrame(uint id);
    void solveReturnFrame(canmsg_t);
    void sendApplyFrame(quint64 oid,uchar nid);
//    控制指令：0110是飞轮id 0 0110 0 1100 0000 0100
//    查询指令：0110是飞轮id 0 0110 0 1100 0000 0010
//    返回指令包：           1 0110 0 1100 0000 1000

    //匹配串口的接口
    void ctlMotorSpd(int id,double spd);
    void ctlMotorTor(int id,double tor);
    void ctlMotorCur(int id,double cur);//zzy
    void getMotorData(int id);
    void setMotorHx(int id);
private:
    //双路CAN 通道
    QCan1680 m_canA;
//    QCan1680 m_canB;
    double          tor_resolution_ratio = 0.0;       //力矩分辨率
    double          current_resolution_ratio = 0.0;   //电流分辨率
    double          spd_resolution_ratio = 0.0;         //转速分辨率

    //-------------为了应对500mNm的识别允许帧---------//
    bool fw1_init = false;
    bool fw2_init = false;
    bool fw3_init = false;
    bool fw4_init = false;
    bool fw5_init = false;
    bool fw6_init = false;

    bool fw1_isOpen = false;
    bool fw2_isOpen = false;
    bool fw3_isOpen = false;
    bool fw4_isOpen = false;
    bool fw5_isOpen = false;
    bool fw6_isOpen = false;

    uchar fw1_id;
    uchar fw2_id;
    uchar fw3_id;
    uchar fw4_id;
    uchar fw5_id;
    uchar fw6_id;

};



#endif // QCANDRIVER_H
