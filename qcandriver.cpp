#include "qcandriver.h"
注：所有与飞轮协议有关指令代码都已经替换成F
QCan1680::QCan1680(QObject *parent):QObject(parent)
{
    qRegisterMetaType<canmsg_t>("canmsg_t");
}

QCanDriver::QCanDriver(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<canmsg_t>("canmsg_t");
    qRegisterMetaType<QVector<uchar>>("QVector<uchar>");
    connect(this,&QCanDriver::setCanFrame,&m_canA,&QCan1680::setCanFrame);
    connect(&m_canA,&QCan1680::getCanFrame,this,&QCanDriver::solveReturnFrame);
}

void QCanDriver::init()
{
    if(!m_canA.canInit(1))
        m_canA.canInit(2);
    m_canA.start();
}
//识别广播帧
void QCanDriver::sendRecoveryFrame(int id)
{
    canmsg_t msg;
    memset(&msg,0,sizeof(canmsg_t));
    msg.id = 0;
    msg.id = QString("FFFFFFFFFF").toULong(nullptr,16);

    msg.length  = 3;
    msg.cob = 0;
    msg.flags = MSG_EXT;
    msg.data[0] = 0xff;
    msg.data[1] = 0xff;
    msg.data[2] = 0xff;
    emit setCanFrame(msg);
}

//MODE = 1,2,3,4分别对应滑行、力矩、速度、电流模式
void QCanDriver::sendControlFrame(uint id, int mode, double data)
{
    canmsg_t msg;
    memset(&msg,0,sizeof(canmsg_t));

    ULONG tmp_id = 0b11111; //仲裁域

    msg.id = tmp_id | (id << 6);
    msg.length  = 4;
    msg.cob = 0;
    //msg.flags = MSG_EXT;

    int data_code;
    //滑行
    if (mode == 1){
        msg.data[0] = 0xFF;
        msg.data[1] = 0;
        msg.data[2] = 0;
        msg.data[3] = 0;
       // msg.data[4] = 0;
    }
    //力矩
    else if (mode ==2){
        data_code = static_cast<int>(data / tor_resolution_ratio);
        msg.data[0] = 0xFF;
        msg.data[1] = 0xFF;
     //   msg.data[2] = (data & 0xff0000) >> 16;
        msg.data[2] = (data_code & 0xff00) >> 8;
        msg.data[3] = (data_code & 0x00ff);
    }
    //速度
    else if (mode ==3){
        data_code = static_cast<int>(data / spd_resolution_ratio);
        msg.data[0] = 0xFF;
        msg.data[1] = (data_code & 0xff0000) >> 16;
        msg.data[2] = (data_code & 0xff00) >> 8;
        msg.data[3] = (data_code & 0x00ff);
    }
    else if (mode == 4){
        data_code = static_cast<int>(data / current_resolution_ratio);//这个电流指令的分辨率是电流分辨率吗
        msg.data[0] = 0xFF;
        msg.data[1] = 0xFF;
       // msg.data[2] = (data & 0xff0000) >> 16;
        msg.data[2] = (data_code & 0xff00) >> 8;
        msg.data[3] = (data_code & 0x00ff);
    }
//    int snp = 0;
//    for (int i = 0;i < 7;i++){
//        snp += msg.data[i];
//    }
//    msg.data[4] = (~snp + 1) & 0xff;
    else{

        logMsg(tr("CAN WRONG CONTROL MODE！"));
        return ;
    }
    emit setCanFrame(msg);
}

//数据请求帧
void QCanDriver::sendGetDataFrame(uint id)
{
    //首帧
    canmsg_t msg;
    //    memset(&msg,0,sizeof(canmsg_t));
    //    QString id_str;
    //    QString id_int;
    //    id_str.push_back("19");//
    ////    id_str.push_back(QString::number(id));
    //    if(id < 16)
    //    {
    //        id_str.push_back('0'+QString::number(id,16));
    //    }
    //    else
    //    {
    //         id_str.push_back(QString::number(id,16));
    //    }
    //    id_str.push_back("f0");
    //    id_str.push_back("00");
    memset(&msg,0,sizeof(canmsg_t));

    ULONG tmp_id = 0b11111;//

    msg.id = tmp_id | (id << 6);
   // msg.id = 0x198;
    msg.length  = 2;
    msg.cob = 0;
   // msg.flags = MSG_EXT;

    msg.data[0] = 0xFF;
    msg.data[1] = 0xFF;//43

    emit setCanFrame(msg);



}

void QCanDriver::solveReturnFrame(canmsg_t msg)
{
    if (msg.length == 8){
        //这是数据传输帧
        //int id = ((msg.id >> 13) & 0xff );
        int id = ((msg.id >> 6) & 0xf);
        QVector<uchar> data;
      //  if ((msg.id & 0xff) == 0xff){
            //说明这是尾帧
            //            data.push_back(1);
            data.push_back(msg.data[0]);
            data.push_back(msg.data[1]);
            data.push_back(msg.data[2]);
            data.push_back(msg.data[3]);
            data.push_back(msg.data[4]);
            data.push_back(msg.data[5]);
            data.push_back(msg.data[6]);
            data.push_back(msg.data[7]);

            emit sendSolvedData(id,data);
      //  }
//        else{
//            //这是起始帧
//            data.push_back(0);
//        }
    }

    else{
        //说明出错了
        logMsg(tr("solve CAN frame error！length wrong!"));
    }
}

//识别允许帧 (用不着使用，直接在)
void QCanDriver::sendApplyFrame(quint64 oid,uchar nid)
{
    union id_union{
        quint64 id;
        uchar data[8];
    }id_conv;
    memset(&id_conv,0,sizeof(id_conv));
    id_conv.id = oid;


    canmsg_t msg;
    memset(&msg,0,sizeof(canmsg_t));
    msg.id = 0;

    QString id_str;
    id_str.append("02");
    id_str.append(id_conv.data[4]);
    id_str.append(id_conv.data[3]);
    id_str.append(id_conv.data[0]);

    msg.id = id_str.toULong(nullptr,16);

    msg.length  = 4;
    msg.cob = 0;
    msg.flags = MSG_EXT;

    msg.data[0] = id_conv.data[5];
    msg.data[1] = id_conv.data[2];
    msg.data[2] = id_conv.data[1];
    msg.data[3] = nid;
    emit setCanFrame(msg);
}



void QCanDriver::ctlMotorSpd(int id, double spd = 0)
{
    //MODE = 1,2,3,4分别对应滑行、力矩、速度、电流模式
    sendControlFrame(id,3,spd);
}

void QCanDriver::ctlMotorTor(int id, double tor)
{
    sendControlFrame(id,2,tor);
}

void QCanDriver::ctlMotorCur(int id, double cur)
{
    sendControlFrame(id, 4, cur);
}

void QCanDriver::getMotorData(int id)
{
    sendGetDataFrame(id);
}

void QCanDriver::setMotorHx(int id)
{
    sendControlFrame(id,1,0);
}

bool QCan1680::canInit(int channel = 0)
{
    int nRet = 0;
    if (channel == 1){
        m_canA = acCanOpen(1,false);
        m_can_name = "CAN1";
    }
    else if (channel == 2){
        m_canA = acCanOpen(2,false);
        m_can_name = "CAN2";
    }
    else{
        return false;
    }
    if( (unsigned int)m_canA == 0xffffffff )
    {
        qDebug()<<"can2 failed!";
        return false;
    }
    //--------------can2 init start------------------------

    nRet = acEnterResetMode( m_canA );                               //Enter reset mode
    if ( nRet < 0 )
    {
        acCanClose( m_canA );
        qDebug()<<"can2 failed!";
        m_canA = INVALID_HANDLE_VALUE;
        return false;
    }

    if( !acSetBaud(m_canA, CAN_TIMING_500K) )
    {
        acCanClose( m_canA );
        qDebug()<<"can2 failed!";
        m_canA = INVALID_HANDLE_VALUE;
        return false;
    }

    nRet = acSetTimeOut( m_canA, 10, 10 );               //Set time out
    if ( nRet < 0 )
    {
        acCanClose( m_canA );
        qDebug()<<"can2 failed!";
        m_canA = INVALID_HANDLE_VALUE;

        return false;
    }

    nRet = acEnterWorkMode( m_canA );                                //Enter work mode
    if( nRet < 0 )
    {
        qDebug()<<"Failed to restart operation!";
        acCanClose( m_canA );
        m_canA = INVALID_HANDLE_VALUE;

        return false;
    }

    //---------------can2 init end---------------

    qDebug()<<"can init success";

    is_init = true;

    //timer init
    if (m_timer_getFrame == nullptr){
        m_timer_getFrame = new QTimer;
    }
    connect(m_timer_getFrame,&QTimer::timeout,this,&QCan1680::readFrame);
    m_timer_getFrame->setInterval(20);
    return true;
}

//channel = 1表示打开CAN2 ，channel=2表示打开CAN3  synchronization= false表示异步通讯
HANDLE QCan1680::acCanOpen(int channel, WINBOOL synchronization)
{
    HANDLE hDevice;

    if (channel == 1){
        TCHAR CanName[20] = TEXT("\\\\.\\CAN1");
        hDevice = NULL;
        if(synchronization)
        {
            hDevice = CreateFile(CanName,
                                 GENERIC_READ | GENERIC_WRITE,
                                 0,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);
        }
        else{
            hDevice = CreateFile(CanName,
                                 GENERIC_READ | GENERIC_WRITE,
                                 0,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                                 NULL);
        }
    }
    else if (channel ==2){
        TCHAR CanName[20] = TEXT("\\\\.\\CAN2");
        hDevice = NULL;
        if(synchronization)
        {
            hDevice = CreateFile(CanName,
                                 GENERIC_READ | GENERIC_WRITE,
                                 0,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);
        }
        else{
            hDevice = CreateFile(CanName,
                                 GENERIC_READ | GENERIC_WRITE,
                                 0,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
                                 NULL);
        }
    }
    else{
        return hDevice;
    }


    return hDevice;
}

int QCan1680::acCanClose(HANDLE hDevice)
{
    if (hDevice != INVALID_HANDLE_VALUE)
    {
        if(!CloseHandle(hDevice))
            return OPERATION_ERROR;

    }
    return SUCCESS;
}

int QCan1680::acSetBaud(HANDLE hDevice, unsigned int nBaud)
{
    Config_par_t config;
    BOOL flag;
    ULONG  ulOutLen;

    config.target = CONF_TIMING;
    config.val1 = nBaud;
    flag = DeviceIoControl (hDevice,
                            (ULONG)CAN_IOCTL_CONFIG,
                            &config,
                            sizeof(Config_par_t),
                            NULL,
                            0,
                            &ulOutLen,
                            0
                            );
    if(!flag)
    {
        return false;
    }

    return true;
}

/*****************************************************************************
*
*    acSetTimeOut
*
*    Purpose:
*        Set Timeout for read and write
*
*
*    Arguments:
*        hDevice           - handle of device
*        ulReadTimeOut     - ms
*         ulWriteTimeOut   - ms
*    Returns:
*        =0 SUCCESS; or <0 failure
*
*****************************************************************************/
int QCan1680::acSetTimeOut(HANDLE hDevice, ULONG ulReadTimeOut, ULONG ulWriteTimeOut)
{
    Config_par_t config;
    BOOL flag;

    ULONG ulOutLen;
    config.target = CONF_TIMEOUT;
    config.val2 = ulReadTimeOut;
    config.val1 = ulWriteTimeOut;
    flag = DeviceIoControl (hDevice,
                            (ULONG)CAN_IOCTL_CONFIG,
                            &config,
                            sizeof(Config_par_t),
                            NULL,
                            0,
                            &ulOutLen,
                            0
                            );
    if(!flag)
    {
        return -1;
    }

    return 1;
}

int QCan1680::acCanRead(HANDLE hDevice, canmsg_t *pbyData, ULONG ulReadCount, ULONG *pulNumberofRead, OVERLAPPED *ov)
{
    ULONG ulErr = 0;

    *pulNumberofRead = 0;
    BOOL flag = ReadFile(hDevice, pbyData, ulReadCount, pulNumberofRead, ov);
    if (flag)
    {
        if(*pulNumberofRead == 0)
            return TIME_OUT;
        return SUCCESS;
    }
    else{
        ulErr = GetLastError();
        if ( ulErr == ERROR_IO_PENDING )
        {
            if ( GetOverlappedResult( hDevice, ov, pulNumberofRead, TRUE ) )    //retrieves the results of overlapped operation
            {
                if(*pulNumberofRead == 0)
                {
                    return TIME_OUT;
                }
                else
                {
                    return SUCCESS;
                }
            }
            else
                return OPERATION_ERROR;
        }
        else return OPERATION_ERROR;
    }
}

int QCan1680::acCanWrite(HANDLE hDevice, canmsg_t *pbyData, ULONG ulWriteCount, ULONG *pulNumberofWritten, OVERLAPPED *ov)
{
    ULONG ulErr = 0;

    *pulNumberofWritten = 0;
    BOOL flag = WriteFile(hDevice, pbyData, ulWriteCount, pulNumberofWritten, ov);
    if (flag)
    {
        if(ulWriteCount > *pulNumberofWritten)
            return TIME_OUT;
        return SUCCESS;
    }
    else{
        ulErr = GetLastError();
        if ( ulErr == ERROR_IO_PENDING )
        {
            if( GetOverlappedResult( hDevice, ov, pulNumberofWritten, TRUE ) )         //retrieves the results of overlapped operation
            {
                if( ulWriteCount > *pulNumberofWritten)
                    return TIME_OUT;
                else
                    return SUCCESS;
            }
            else
                return OPERATION_ERROR;
        }
        else
            return OPERATION_ERROR;
    }
}

int QCan1680::acEnterResetMode(HANDLE hDevice)
{
    Command_par_t cmd;
    BOOL flag;
    ULONG ulOutLen;

    cmd.cmd = CMD_STOP;
    flag = DeviceIoControl (hDevice,
                            (ULONG)CAN_IOCTL_COMMAND,
                            &cmd,
                            sizeof(Command_par_t),
                            NULL,
                            0,
                            &ulOutLen,
                            0
                            );
    if(!flag)
    {
        return OPERATION_ERROR;
    }

    return SUCCESS;
}

int QCan1680::acEnterWorkMode(HANDLE hDevice)
{
    Command_par_t cmd;
    BOOL flag;
    ULONG ulOutLen;

    cmd.cmd = CMD_START;
    flag = DeviceIoControl (hDevice,
                            (ULONG)CAN_IOCTL_COMMAND,
                            &cmd,
                            sizeof(Command_par_t),
                            NULL,
                            0,
                            &ulOutLen,
                            0
                            );
    if(!flag)
    {
        return OPERATION_ERROR;
    }

    return SUCCESS;
}

void QCan1680::readFrame()
{
    if (is_init == false){
        return ;
    }
    canmsg_t msg;
    OVERLAPPED   ov;

    ZeroMemory( &ov, sizeof(OVERLAPPED) );
    ov.hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    ZeroMemory( &msg, sizeof( canmsg_t ) );
    ULONG ulNumberofRead = 0;

    int nRet = acCanRead( m_canA, &msg, (ULONG)1, &ulNumberofRead, &ov );
    if ( nRet == SUCCESS ){
        emit getCanFrame(msg);
    }
    else if( nRet == TIME_OUT ){
        //        logMsg(tr("read can time out!"));
    }
    else{

        logMsg(tr("read can wrong!"));
    }
}

void QCan1680::setCanFrame(canmsg_t msg)
{
    if (is_init == false){
        return;
    }
    OVERLAPPED    ov;
    OVERLAPPED    ovSelfReceive;
    ZeroMemory( &ov, sizeof(OVERLAPPED) );
    ZeroMemory( &ovSelfReceive, sizeof(OVERLAPPED) );
    ov.hEvent = CreateEvent( NULL,FALSE, FALSE, NULL );
    ovSelfReceive.hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    //ZeroMemory( &msg, sizeof( canmsg_t ));
    //    msg.flags = MSG_EXT;
    //    msg.id = 1;
    //    msg.cob = 0;
    //    msg.length = 1;
    //    msg.data[0] = 1;

    ULONG ulNumberofWritten = 0;

    int nRet = acCanWrite( m_canA,  &msg, 1, &ulNumberofWritten, &ov );
    if ( nRet == SUCCESS )
    {
        //Sending message ok
        //        qDebug()<< " sending ok!" ;
    }
    else if ( nRet == TIME_OUT)
    {
        //Sending message timeout
        logMsg( tr("can Sending timeout!"));
    }
    else
    {
        //Sending message error
        logMsg(tr("can Sending error!"));
    }
}

void QCan1680::start()
{
    if (is_init == false){
        return;
    }
    if (!is_start ){

        m_timer_getFrame->start();
        is_start = true;
    }
}

void QCan1680::stop()
{
    if (is_start){
        m_timer_getFrame->stop();
        is_start = false;
    }
}
