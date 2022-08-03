#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_sys_status_1_(false),
    m_sys_status_2_(false),
    m_sys_status_3_(false),
    m_sys_status_4_(false),
    m_sys_status_5_(false),
    m_sys_status_6_(false)
{
    ui->setupUi(this);
    qRegisterMetaType<QVector<QVector<QString> >>("QVector<QVector<QString> >");
    qRegisterMetaType<QVector<QString>>("QVector<QString>");
    qRegisterMetaType<QVector<double>>("QVector<double>");
    qDebug()<<"MAIN THREAD:"<<QThread::currentThreadId();

    m_timer_get_data_.setInterval(100); //get data timer 0.5s 这个定时器是用来控制调用模式函数的时间间隔，已经固定死了
    m_timer_get_data_2.setInterval(100); //get data timer 0.5s
    m_timer_get_data_3.setInterval(100); //get data timer 0.5s
    m_timer_get_data_4.setInterval(100); //get data timer 0.5s
    m_timer_get_data_5.setInterval(100); //get data timer 0.5s
    m_timer_get_data_6.setInterval(100); //get data timer 0.5s

    m_timer_update_.setInterval(500);   //update view every 0.5s 这个定时器是用来控制插入数据库的时间间隔等 它可以在界面上的控制间隔进行调整
    connect(&m_timer_update_,SIGNAL(timeout()),this,SLOT(updateMotor()));
    m_timer_update_.start();

    m_timer_update_2.setInterval(500);   //update view every 0.5s
    connect(&m_timer_update_2,SIGNAL(timeout()),this,SLOT(updateMotor2()));
    m_timer_update_2.start();

    m_timer_update_3.setInterval(500);   //update view every 0.5s
    connect(&m_timer_update_3,SIGNAL(timeout()),this,SLOT(updateMotor3()));
    m_timer_update_3.start();

    m_timer_update_4.setInterval(500);   //update view every 0.5s
    connect(&m_timer_update_4,SIGNAL(timeout()),this,SLOT(updateMotor4()));
    m_timer_update_4.start();

    m_timer_update_5.setInterval(500);   //update view every 0.5s
    connect(&m_timer_update_5,SIGNAL(timeout()),this,SLOT(updateMotor5()));
    m_timer_update_5.start();

    m_timer_update_6.setInterval(500);   //update view every 0.5s
    connect(&m_timer_update_6,SIGNAL(timeout()),this,SLOT(updateMotor6()));
    m_timer_update_6.start();

    initCombox();

    //从log文件中加载串口参数
    //    initSerialInfo();

    initDriver1();
    initDriver2();
    initDriver3();
    initDriver4();
    initDriver5();
    initDriver6();

    initQCustomPlot1();
    initQCustomPlot2();
    initQCustomPlot3();
    initQCustomPlot4();
    initQCustomPlot5();
    initQCustomPlot6();

    initSql();
    initReport();
    initDaqCard();


    connect(p_repoter_,&QMotorReport::processUpdate,this,&MainWindow::processDisplay);

    initSettingTab();
    ui->checkBox_xp_mode_1->setEnabled(false);
    ui->checkBox_xp_mode_2->setEnabled(false);
    ui->checkBox_xp_mode_3->setEnabled(false);
    ui->checkBox_xp_mode_4->setEnabled(false);
    ui->checkBox_xp_mode_5->setEnabled(false);
    ui->checkBox_xp_mode_6->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
    p_driver_thread1_->quit();
    p_driver_thread1_->wait();
    //    p_driver_thread2_->quit();
    //    p_driver_thread2_->wait();
    //    p_driver_thread3_->quit();
    //    p_driver_thread3_->wait();
    //    p_driver_thread4_->quit();
    //    p_driver_thread4_->wait();
    //    p_driver_thread5_->quit();
    //    p_driver_thread5_->wait();
    //    p_driver_thread6_->quit();
    //    p_driver_thread6_->wait();
}

bool MainWindow::initDriver1()
{
    p_motor_thread1_ = new QThread;
    p_motor1_ = new Motor;
    //    p_motor1_->setId(8);
    p_motor1_->setChannel("MOTOR1");
    //p_motor1_->setAccelerate(50);
    p_motor1_->moveToThread(p_motor_thread1_);
    p_motor_thread1_->start();

    //driver thread 1
    //    p_driver1_ = new MotorDriver;
    p_can_driver_ = new QCanDriver;
    p_driver_thread1_ = new QThread;
    p_can_driver_->moveToThread(p_driver_thread1_);

    connect(p_driver_thread1_,&QThread::finished,p_can_driver_,&QCanDriver::deleteLater);
    connect(p_driver_thread1_,&QThread::finished,p_driver_thread1_,&QThread::deleteLater);
    connect(p_can_driver_,&QCanDriver::logMsg,this,&MainWindow::logMsg);

    connect(p_driver_thread1_,&QThread::started,p_can_driver_,&QCanDriver::init);
    //seems that should connect driver obj later

    p_driver_thread1_->start();

    //get data timer init

    //    connect(&m_timer_get_data_,&QTimer::timeout,p_driver1_,&MotorDriver::getMotorData);

    //control init
    connect(p_motor1_,&Motor::sendMoTorSpd2,p_can_driver_,&QCanDriver::ctlMotorSpd);
    connect(p_motor1_,&Motor::sendMoTorTor2,p_can_driver_,&QCanDriver::ctlMotorTor);
    connect(p_motor1_,&Motor::sendMoTorHX2,p_can_driver_,&QCanDriver::setMotorHx);
    connect(p_motor1_, &Motor::sendMotorCur2, p_can_driver_, &QCanDriver::ctlMotorCur);

    connect(p_can_driver_,&QCanDriver::sendSolvedData,this,&MainWindow::solveCanData);
    connect(this,&MainWindow::canRecovery,p_can_driver_,&QCanDriver::sendRecoveryFrame);
    connect(this,&MainWindow::getCanFrame,p_can_driver_,&QCanDriver::getMotorData);

    connect(this,&MainWindow::startAutoTestModeNoAir,p_motor1_,&Motor::initTestModeNOAir);
    connect(this,&MainWindow::sendCurrentSpdForAutoTest,p_motor1_,&Motor::runWithAirMode);
    connect(this,&MainWindow::refreshAutoTestSpd,p_motor1_,&Motor::setSpdOfAutoTestSpdMode);
    connect(this,&MainWindow::refreshAutoTestTor,p_motor1_,&Motor::setTorOfAutoTestTorMode);
    connect(this, &MainWindow::refreshAutotestCur, p_motor1_, &Motor::setCurOfAutoTestCurMode);
    connect(p_motor1_,&Motor::autoTest,this,&MainWindow::displayStatus1);
    //connect(p_motor1_,&Motor::autoTorTest,this,&MainWindow::displayTorStatus);
    //connect(this,&MainWindow::refreshAutoTestHX,p_motor1_,&Motor::setSpdOfAutoTestHXMode);

    connect(this,&MainWindow::sendCurrentSpdForAutoTest,p_motor1_,&Motor::runWithAirMode);
    connect(p_motor1_,&Motor::sendHXEnd,this,&MainWindow::disp_hx_end);

    //采样间隔改了的话，记得改这个 不是界面上该的
    p_motor1_->setCurrentInterval(double(m_timer_update_.interval()) / 1000);
    //     p_motor1_->setCurrentInterval(0.1);
    return true;
}
bool MainWindow::initDriver2()
{
    p_motor_thread2_ = new QThread;
    p_motor2_ = new Motor;
    p_motor2_->setId(2);
    p_motor2_->setChannel("MOTOR2");
    //p_motor1_->setAccelerate(50);
    p_motor2_->moveToThread(p_motor_thread2_);
    p_motor_thread2_->start();

    //driver thread 1
    //    p_driver2_ = new MotorDriver;
    //    p_driver_thread2_ = new QThread;
    //    p_driver2_->moveToThread(p_driver_thread2_);

    //    connect(p_driver_thread2_,&QThread::finished,p_driver2_,&MotorDriver::deleteLater);
    //    connect(p_driver_thread2_,&QThread::finished,p_driver_thread2_,&QThread::deleteLater);
    //    connect(p_driver2_,&MotorDriver::sendErrText,this,&MainWindow::logMsg);
    //    connect(p_driver_thread2_,&QThread::started,p_driver2_,&MotorDriver::init);//这个可能要改
    //    //seems that should connect driver obj later
    //    p_driver2_->setPortName(serial_channel_2);
    //    p_driver2_->setBaudRate(serial_baudrate_2);
    //    p_driver_thread2_->start();
    //get data timer init

    //    connect(&m_timer_get_data_2,&QTimer::timeout,p_driver2_,&MotorDriver::getMotorData);

    //control init
    connect(p_motor2_,&Motor::sendMoTorSpd2,p_can_driver_,&QCanDriver::ctlMotorSpd);
    connect(p_motor2_,&Motor::sendMoTorTor2,p_can_driver_,&QCanDriver::ctlMotorTor);
    connect(p_motor2_,&Motor::sendMoTorHX2,p_can_driver_,&QCanDriver::setMotorHx);
    connect(p_motor2_, &Motor::sendMotorCur2, p_can_driver_, &QCanDriver::ctlMotorCur);

    //    connect(p_driver2_,&MotorDriver::sendMotorSpd,p_motor2_,&Motor::setSpeed);
    //    connect(p_driver2_,&MotorDriver::sendMotorCur,p_motor2_,&Motor::setCurrent);

    connect(this,&MainWindow::startAutoTestModeNoAir2,p_motor2_,&Motor::initTestModeNOAir);
    connect(this,&MainWindow::sendCurrentSpdForAutoTest2,p_motor2_,&Motor::runWithAirMode);
    connect(this,&MainWindow::refreshAutoTestSpd2,p_motor2_,&Motor::setSpdOfAutoTestSpdMode);
    connect(this,&MainWindow::refreshAutoTestTor2,p_motor2_,&Motor::setTorOfAutoTestTorMode);
    connect(this, &MainWindow::refreshAutotestCur2, p_motor2_, &Motor::setCurOfAutoTestCurMode);
    connect(p_motor2_,&Motor::autoTest,this,&MainWindow::displayStatus2);
    //    connect(p_motor2_,&Motor::autoSpdTest,this,&MainWindow::displaySpdStatus);
    //    connect(p_motor2_,&Motor::autoTorTest,this,&MainWindow::displayTorStatus);
    connect(p_motor2_,&Motor::sendHXEnd,this,&MainWindow::disp_hx_end);
    //采样间隔改了的话，记得改这个
    p_motor2_->setCurrentInterval(double(m_timer_update_2.interval()) / 1000);
    return true;
}
bool MainWindow::initDriver3()
{
    p_motor_thread3_ = new QThread;
    p_motor3_ = new Motor;
    p_motor3_->setId(3);
    p_motor3_->setChannel("MOTOR3");
    p_motor3_->moveToThread(p_motor_thread3_);
    p_motor_thread3_->start();

    //driver thread 1
    //    p_driver3_ = new MotorDriver;
    //    p_driver_thread3_ = new QThread;
    //    p_driver3_->moveToThread(p_driver_thread3_);

    //    connect(p_driver_thread3_,&QThread::finished,p_driver3_,&MotorDriver::deleteLater);
    //    connect(p_driver_thread3_,&QThread::finished,p_driver_thread3_,&QThread::deleteLater);
    //    connect(p_driver3_,&MotorDriver::sendErrText,this,&MainWindow::logMsg);
    //    connect(p_driver_thread3_,&QThread::started,p_driver3_,&MotorDriver::init);
    //    //seems that should connect driver obj later
    //    p_driver3_->setPortName(serial_channel_3);
    //    p_driver3_->setBaudRate(serial_baudrate_3);
    //    p_driver_thread3_->start();
    //get data timer init

    //    connect(&m_timer_get_data_3,&QTimer::timeout,p_driver3_,&MotorDriver::getMotorData);
    //control init
    connect(p_motor3_,&Motor::sendMoTorSpd2,p_can_driver_,&QCanDriver::ctlMotorSpd);
    connect(p_motor3_,&Motor::sendMoTorTor2,p_can_driver_,&QCanDriver::ctlMotorTor);
    connect(p_motor3_,&Motor::sendMoTorHX2,p_can_driver_,&QCanDriver::setMotorHx);
    connect(p_motor3_, &Motor::sendMotorCur2, p_can_driver_, &QCanDriver::ctlMotorCur);

    //    connect(p_driver3_,&MotorDriver::sendMotorSpd,p_motor3_,&Motor::setSpeed);
    //    connect(p_driver3_,&MotorDriver::sendMotorCur,p_motor3_,&Motor::setCurrent);

    connect(this,&MainWindow::startAutoTestModeNoAir3,p_motor3_,&Motor::initTestModeNOAir);
    connect(this,&MainWindow::sendCurrentSpdForAutoTest3,p_motor3_,&Motor::runWithAirMode);
    connect(this,&MainWindow::refreshAutoTestSpd3,p_motor3_,&Motor::setSpdOfAutoTestSpdMode);
    connect(this,&MainWindow::refreshAutoTestTor3,p_motor3_,&Motor::setTorOfAutoTestTorMode);
    connect(this, &MainWindow::refreshAutotestCur3, p_motor3_, &Motor::setCurOfAutoTestCurMode);

    connect(p_motor3_,&Motor::autoTest,this,&MainWindow::displayStatus3);
    connect(p_motor3_,&Motor::sendHXEnd,this,&MainWindow::disp_hx_end);
    //    connect(p_motor3_,&Motor::autoSpdTest,this,&MainWindow::displaySpdStatus);
    //    connect(p_motor3_,&Motor::autoTorTest,this,&MainWindow::displayTorStatus);

    //采样间隔改了的话，记得改这个
    p_motor3_->setCurrentInterval(double(m_timer_update_3.interval()) / 1000);
    return true;
}
bool MainWindow::initDriver4()
{
    p_motor_thread4_ = new QThread;
    p_motor4_ = new Motor;
    p_motor4_->setId(4);
    p_motor4_->setChannel("MOTOR4");
    p_motor4_->moveToThread(p_motor_thread4_);
    p_motor_thread4_->start();

    //driver thread 1
    //    p_driver4_ = new MotorDriver;
    //    p_driver_thread4_ = new QThread;
    //    p_driver4_->moveToThread(p_driver_thread4_);

    //    connect(p_driver_thread4_,&QThread::finished,p_driver4_,&MotorDriver::deleteLater);
    //    connect(p_driver_thread4_,&QThread::finished,p_driver_thread4_,&QThread::deleteLater);
    //    connect(p_driver4_,&MotorDriver::sendErrText,this,&MainWindow::logMsg);
    //    connect(p_driver_thread4_,&QThread::started,p_driver4_,&MotorDriver::init);
    //    //seems that should connect driver obj later
    //    p_driver4_->setPortName(serial_channel_4);
    //    p_driver4_->setBaudRate(serial_baudrate_4);
    //    p_driver_thread4_->start();
    //get data timer init

    //    connect(&m_timer_get_data_4,&QTimer::timeout,p_driver4_,&MotorDriver::getMotorData);
    //control init
    connect(p_motor4_,&Motor::sendMoTorSpd2,p_can_driver_,&QCanDriver::ctlMotorSpd);
    connect(p_motor4_,&Motor::sendMoTorTor2,p_can_driver_,&QCanDriver::ctlMotorTor);
    connect(p_motor4_,&Motor::sendMoTorHX2,p_can_driver_,&QCanDriver::setMotorHx);
    connect(p_motor4_, &Motor::sendMotorCur2, p_can_driver_, &QCanDriver::ctlMotorCur);

    //    connect(p_driver4_,&MotorDriver::sendMotorSpd,p_motor4_,&Motor::setSpeed);
    //    connect(p_driver4_,&MotorDriver::sendMotorCur,p_motor4_,&Motor::setCurrent);

    connect(this,&MainWindow::startAutoTestModeNoAir4,p_motor4_,&Motor::initTestModeNOAir);
    connect(this,&MainWindow::sendCurrentSpdForAutoTest4,p_motor4_,&Motor::runWithAirMode);
    connect(this,&MainWindow::refreshAutoTestSpd4,p_motor4_,&Motor::setSpdOfAutoTestSpdMode);
    connect(this,&MainWindow::refreshAutoTestTor4,p_motor4_,&Motor::setTorOfAutoTestTorMode);
    connect(this, &MainWindow::refreshAutotestCur4, p_motor4_, &Motor::setCurOfAutoTestCurMode);

    connect(p_motor4_,&Motor::autoTest,this,&MainWindow::displayStatus4);
    //    connect(p_motor4_,&Motor::autoSpdTest,this,&MainWindow::displaySpdStatus);
    //    connect(p_motor4_,&Motor::autoTorTest,this,&MainWindow::displayTorStatus);
    connect(p_motor4_,&Motor::sendHXEnd,this,&MainWindow::disp_hx_end);
    //采样间隔改了的话，记得改这个
    p_motor4_->setCurrentInterval(double(m_timer_update_4.interval()) / 1000);
    return true;
}
bool MainWindow::initDriver5()
{
    p_motor_thread5_ = new QThread;
    p_motor5_ = new Motor;
    p_motor5_->setId(5);
    p_motor5_->setChannel("MOTOR5");
    p_motor5_->moveToThread(p_motor_thread5_);
    p_motor_thread5_->start();

    //driver thread 1
    //    p_driver5_ = new MotorDriver;
    //    p_driver_thread5_ = new QThread;
    //    p_driver5_->moveToThread(p_driver_thread5_);

    //    connect(p_driver_thread5_,&QThread::finished,p_driver5_,&MotorDriver::deleteLater);
    //    connect(p_driver_thread5_,&QThread::finished,p_driver_thread5_,&QThread::deleteLater);
    //    connect(p_driver5_,&MotorDriver::sendErrText,this,&MainWindow::logMsg);
    //    connect(p_driver_thread5_,&QThread::started,p_driver5_,&MotorDriver::init);
    //    //seems that should connect driver obj later
    //    p_driver5_->setPortName(serial_channel_5);
    //    p_driver5_->setBaudRate(serial_baudrate_5);
    //    p_driver_thread5_->start();
    //    //get data timer init

    //    connect(&m_timer_get_data_5,&QTimer::timeout,p_driver5_,&MotorDriver::getMotorData);
    //control init
    connect(p_motor5_,&Motor::sendMoTorSpd2,p_can_driver_,&QCanDriver::ctlMotorSpd);
    connect(p_motor5_,&Motor::sendMoTorTor2,p_can_driver_,&QCanDriver::ctlMotorTor);
    connect(p_motor5_,&Motor::sendMoTorHX2,p_can_driver_,&QCanDriver::setMotorHx);
    connect(p_motor5_, &Motor::sendMotorCur2, p_can_driver_, &QCanDriver::ctlMotorCur);

    //    connect(p_driver5_,&MotorDriver::sendMotorSpd,p_motor5_,&Motor::setSpeed);
    //    connect(p_driver5_,&MotorDriver::sendMotorCur,p_motor5_,&Motor::setCurrent);

    connect(this,&MainWindow::startAutoTestModeNoAir5,p_motor5_,&Motor::initTestModeNOAir);
    connect(this,&MainWindow::sendCurrentSpdForAutoTest5,p_motor5_,&Motor::runWithAirMode);
    connect(this,&MainWindow::refreshAutoTestSpd5,p_motor5_,&Motor::setSpdOfAutoTestSpdMode);
    connect(this,&MainWindow::refreshAutoTestTor5,p_motor5_,&Motor::setTorOfAutoTestTorMode);
    connect(this, &MainWindow::refreshAutotestCur5, p_motor5_, &Motor::setCurOfAutoTestCurMode);

    connect(p_motor5_,&Motor::autoTest,this,&MainWindow::displayStatus5);
    //    connect(p_motor5_,&Motor::autoSpdTest,this,&MainWindow::displaySpdStatus);
    //    connect(p_motor5_,&Motor::autoTorTest,this,&MainWindow::displayTorStatus);
    connect(p_motor5_,&Motor::sendHXEnd,this,&MainWindow::disp_hx_end);
    //采样间隔改了的话，记得改这个
    p_motor5_->setCurrentInterval(double(m_timer_update_5.interval()) / 1000);
    return true;
}
bool MainWindow::initDriver6()
{
    p_motor_thread6_ = new QThread;
    p_motor6_ = new Motor;
    p_motor6_->setId(6);
    p_motor6_->setChannel("MOTOR6");
    p_motor6_->moveToThread(p_motor_thread6_);
    p_motor_thread6_->start();

    //driver thread 1
    //    p_driver6_ = new MotorDriver;
    //    p_driver_thread6_ = new QThread;
    //    p_driver6_->moveToThread(p_driver_thread6_);

    //    connect(p_driver_thread6_,&QThread::finished,p_driver6_,&MotorDriver::deleteLater);
    //    connect(p_driver_thread6_,&QThread::finished,p_driver_thread6_,&QThread::deleteLater);
    //    connect(p_driver6_,&MotorDriver::sendErrText,this,&MainWindow::logMsg);
    //    connect(p_driver_thread6_,&QThread::started,p_driver6_,&MotorDriver::init);
    //    //seems that should connect driver obj later
    //    p_driver6_->setPortName(serial_channel_6);
    //    p_driver6_->setBaudRate(serial_baudrate_6);
    //    p_driver_thread6_->start();
    //get data timer init

    //    connect(&m_timer_get_data_6,&QTimer::timeout,p_driver6_,&MotorDriver::getMotorData);
    //control init
    connect(p_motor6_,&Motor::sendMoTorSpd2,p_can_driver_,&QCanDriver::ctlMotorSpd);
    connect(p_motor6_,&Motor::sendMoTorTor2,p_can_driver_,&QCanDriver::ctlMotorTor);
    connect(p_motor6_,&Motor::sendMoTorHX2,p_can_driver_,&QCanDriver::setMotorHx);
    connect(p_motor6_, &Motor::sendMotorCur2, p_can_driver_, &QCanDriver::ctlMotorCur);

    //    connect(p_driver6_,&MotorDriver::sendMotorSpd,p_motor6_,&Motor::setSpeed);
    //    connect(p_driver6_,&MotorDriver::sendMotorCur,p_motor6_,&Motor::setCurrent);

    connect(this,&MainWindow::startAutoTestModeNoAir6,p_motor6_,&Motor::initTestModeNOAir);
    connect(this,&MainWindow::sendCurrentSpdForAutoTest6,p_motor6_,&Motor::runWithAirMode);
    connect(this,&MainWindow::refreshAutoTestSpd6,p_motor6_,&Motor::setSpdOfAutoTestSpdMode);
    connect(this,&MainWindow::refreshAutoTestTor6,p_motor6_,&Motor::setTorOfAutoTestTorMode);
    connect(this, &MainWindow::refreshAutotestCur6, p_motor6_, &Motor::setCurOfAutoTestCurMode);

    connect(p_motor6_,&Motor::autoTest,this,&MainWindow::displayStatus6);
    //    connect(p_motor6_,&Motor::autoSpdTest,this,&MainWindow::displaySpdStatus);
    //    connect(p_motor6_,&Motor::autoTorTest,this,&MainWindow::displayTorStatus);
    connect(p_motor6_,&Motor::sendHXEnd,this,&MainWindow::disp_hx_end);
    //采样间隔改了的话，记得改这个
    p_motor6_->setCurrentInterval(double(m_timer_update_6.interval()) / 1000);
    return true;
}
//初始化数据库
void MainWindow::initSql()
{
    p_sql_ = new SqlDataBase;
    p_sql_thread_ = new QThread;
    p_sql_->moveToThread(p_sql_thread_);
    p_sql_->sqlInit();

    connect(p_sql_thread_,&QThread::finished,p_sql_,&SqlDataBase::deleteLater);
    connect(p_sql_thread_,&QThread::finished,p_sql_thread_,&QThread::deleteLater);

    connect(this,&MainWindow::sendToSqlDB,p_sql_,&SqlDataBase::insertIntoDB);
    connect(this,&MainWindow::sendToSqlDB2,p_sql_,&SqlDataBase::insertIntoDB);
    connect(this,&MainWindow::sendToSqlDB3,p_sql_,&SqlDataBase::insertIntoDB);
    connect(this,&MainWindow::sendToSqlDB4,p_sql_,&SqlDataBase::insertIntoDB);
    connect(this,&MainWindow::sendToSqlDB5,p_sql_,&SqlDataBase::insertIntoDB);
    connect(this,&MainWindow::sendToSqlDB6,p_sql_,&SqlDataBase::insertIntoDB);

    connect(this,&MainWindow::sendQueryExcelToDB,p_sql_,&SqlDataBase::getExpDataFromSqlDB2);
    connect(this,&MainWindow::sendQueryWordToDB,p_sql_,&SqlDataBase::getExpDataFromSqlDB);

    connect(p_sql_,&SqlDataBase::sendErrorText,this,&MainWindow::logMsg);
    connect(p_sql_,&SqlDataBase::emitExpData,this,&MainWindow::updataSqlTableView);
    //connect(p_sql_,&SqlDataBase::emitExpDataforexcel,this,&MainWindow::exportExcel,Qt::QueuedConnection);
    p_sql_thread_->start();



}

void MainWindow::initReport()
{
    p_repoter_ = new QMotorReport;
    p_repoter_thread_ = new QThread;
    p_repoter_->moveToThread(p_repoter_thread_);

    connect(p_sql_thread_,&QThread::finished,p_repoter_,&QMotorReport::deleteLater);
    connect(p_sql_thread_,&QThread::finished,p_repoter_thread_,&QThread::deleteLater);

    //将接受数据与处理函数连接
    //    connect(p_sql_,&SqlDataBase::emitExpData,p_repoter_,&QMotorReport::getDataFromSql);
    connect(this,&MainWindow::getLastExpData,p_sql_,&SqlDataBase::getLastExpData);
    connect(p_sql_,&SqlDataBase::emitLastExpData,p_repoter_,&QMotorReport::getDataFromSql);
    connect(p_sql_,&SqlDataBase::emitExpDataforexcel,p_repoter_,&QMotorReport::createExcel);
    connect(this,&MainWindow::stopCreateExcel,p_repoter_,&QMotorReport::close);
    connect(p_repoter_,&QMotorReport::logMsg,this,&MainWindow::logMsg);
    connect(p_repoter_thread_,&QThread::started,p_repoter_,&QMotorReport::init);

    p_repoter_thread_->start();

    ui->dateTimeEdit_start_time->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_end_time->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::initDaqCard()
{
    p_daqcard_ = new QDaqcard;
    p_daqcard_thread_ = new QThread;
    p_daqcard_->moveToThread(p_daqcard_thread_);

    connect(p_daqcard_thread_,&QThread::finished,p_daqcard_,&QMotorReport::deleteLater);
    connect(p_daqcard_thread_,&QThread::finished,p_daqcard_thread_,&QThread::deleteLater);
    connect(p_daqcard_,&QDaqcard::logMsg,this,&MainWindow::logMsg);
    connect(p_daqcard_thread_,&QThread::started,p_daqcard_,&QDaqcard::init);
    connect(&m_timer_get_data_,&QTimer::timeout,p_daqcard_,&QDaqcard::readAllChannel);
    connect(&m_timer_get_data_2,&QTimer::timeout,p_daqcard_,&QDaqcard::readAllChannel);
    connect(&m_timer_get_data_3,&QTimer::timeout,p_daqcard_,&QDaqcard::readAllChannel);
    connect(&m_timer_get_data_4,&QTimer::timeout,p_daqcard_,&QDaqcard::readAllChannel);
    connect(&m_timer_get_data_5,&QTimer::timeout,p_daqcard_,&QDaqcard::readAllChannel);
    connect(&m_timer_get_data_6,&QTimer::timeout,p_daqcard_,&QDaqcard::readAllChannel);
    connect(p_daqcard_,&QDaqcard::sendAllData,this,&MainWindow::setMotorDataFromDAQCard);
    p_daqcard_thread_->start();
}

void MainWindow::initCombox()
{   //FW1
    ui->comboBox_motor_test_mode_1->addItem("速度模式");
    ui->comboBox_motor_test_mode_1->addItem("斜坡模式");
    ui->comboBox_motor_test_mode_1->addItem("正弦模式");
    ui->comboBox_motor_test_mode_1->addItem("力矩模式");
    ui->comboBox_motor_test_mode_1->addItem("时间常数计算");//速度时间常数计算
    ui->comboBox_motor_test_mode_1->addItem("滑行模式");
    ui->comboBox_motor_test_mode_1->addItem("寿命模式");
    ui->comboBox_motor_test_mode_1->addItem("电流模式");
    //FW2
    ui->comboBox_motor_test_mode_2->addItem("速度模式");
    ui->comboBox_motor_test_mode_2->addItem("斜坡模式");
    ui->comboBox_motor_test_mode_2->addItem("正弦模式");
    ui->comboBox_motor_test_mode_2->addItem("力矩模式");
    ui->comboBox_motor_test_mode_2->addItem("时间常数计算");//速度时间常数计算
    ui->comboBox_motor_test_mode_2->addItem("滑行模式");
    ui->comboBox_motor_test_mode_2->addItem("寿命模式");
    ui->comboBox_motor_test_mode_2->addItem("电流模式");

    //FW3
    ui->comboBox_motor_test_mode_3->addItem("速度模式");
    ui->comboBox_motor_test_mode_3->addItem("斜坡模式");
    ui->comboBox_motor_test_mode_3->addItem("正弦模式");
    ui->comboBox_motor_test_mode_3->addItem("力矩模式");
    ui->comboBox_motor_test_mode_3->addItem("时间常数计算");//速度时间常数计算
    ui->comboBox_motor_test_mode_3->addItem("滑行模式");
    ui->comboBox_motor_test_mode_3->addItem("寿命模式");
    ui->comboBox_motor_test_mode_3->addItem("电流模式");

    //FW4
    ui->comboBox_motor_test_mode_4->addItem("速度模式");
    ui->comboBox_motor_test_mode_4->addItem("斜坡模式");
    ui->comboBox_motor_test_mode_4->addItem("正弦模式");
    ui->comboBox_motor_test_mode_4->addItem("力矩模式");
    ui->comboBox_motor_test_mode_4->addItem("时间常数计算");//速度时间常数计算
    ui->comboBox_motor_test_mode_4->addItem("滑行模式");
    ui->comboBox_motor_test_mode_4->addItem("寿命模式");
    ui->comboBox_motor_test_mode_4->addItem("电流模式");

    //FW5
    ui->comboBox_motor_test_mode_5->addItem("速度模式");
    ui->comboBox_motor_test_mode_5->addItem("斜坡模式");
    ui->comboBox_motor_test_mode_5->addItem("正弦模式");
    ui->comboBox_motor_test_mode_5->addItem("力矩模式");
    ui->comboBox_motor_test_mode_5->addItem("时间常数计算");//速度时间常数计算
    ui->comboBox_motor_test_mode_5->addItem("滑行模式");
    ui->comboBox_motor_test_mode_5->addItem("寿命模式");
    ui->comboBox_motor_test_mode_5->addItem("电流模式");

    //FW6
    ui->comboBox_motor_test_mode_6->addItem("速度模式");
    ui->comboBox_motor_test_mode_6->addItem("斜坡模式");
    ui->comboBox_motor_test_mode_6->addItem("正弦模式");
    ui->comboBox_motor_test_mode_6->addItem("力矩模式");
    ui->comboBox_motor_test_mode_6->addItem("时间常数计算");//速度时间常数计算
    ui->comboBox_motor_test_mode_6->addItem("滑行模式");
    ui->comboBox_motor_test_mode_6->addItem("寿命模式");
    ui->comboBox_motor_test_mode_6->addItem("电流模式");

}

//更新电机1---波形显示
void MainWindow::initQCustomPlot1()
{

    ui->qcp_motor_cur_1->addGraph();
    ui->qcp_motor_cur_1->graph(0)->setPen(QPen(Qt::blue));
    ui->qcp_motor_cur_1->addGraph();
    ui->qcp_motor_cur_1->graph(1)->setPen(QPen(Qt::red));

    ui->qcp_motor_cur_1->graph(0)->setName("motorCurrent");

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s"); // %h代表小时
    ui->qcp_motor_cur_1->xAxis->setTicker(timeTicker);
    ui->qcp_motor_cur_1->axisRect()->setupFullAxesBox();
    ui->qcp_motor_cur_1->yAxis->setLabel("电流(mA)");

    // make left and bottom axes transfer their ranges to right and top axes:
    connect( ui->qcp_motor_cur_1->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_1->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_cur_1->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_1->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:

    //dataTimer.start(0); // Interval 0 means to refresh as fast as possibl

    ui->qcp_motor_spd_1->addGraph();
    ui->qcp_motor_spd_1->graph(0)->setPen(QPen(Qt::red));
    ui->qcp_motor_spd_1->addGraph();
    ui->qcp_motor_spd_1->graph(1)->setPen(QPen(Qt::blue));

    ui->qcp_motor_spd_1->yAxis->setLabel("转速(rpm)");

    ui->qcp_motor_spd_1->xAxis->setTicker(timeTicker);
    ui->qcp_motor_spd_1->axisRect()->setupFullAxesBox();
    ui->qcp_motor_spd_1->yAxis->setRange(0,1);//与graph(0)和graph(1)没有关系

    connect( ui->qcp_motor_spd_1->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_1->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_spd_1->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_1->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_1->addGraph();
    ui->qcp_motor_act_tor_1->graph(0)->setPen(QPen(Qt::red));

    ui->qcp_motor_act_tor_1->yAxis->setLabel("力矩(mNm)");

    ui->qcp_motor_act_tor_1->xAxis->setTicker(timeTicker);
    ui->qcp_motor_act_tor_1->axisRect()->setupFullAxesBox();
    //ui->widget->yAxis->setRange(-30,30);


    ui->qcp_motor_act_tor_1->xAxis->setTicker(timeTicker);

    connect( ui->qcp_motor_act_tor_1->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_1->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_act_tor_1->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_1->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_spd_1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_cur_1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}
void MainWindow::initQCustomPlot2()
{

    ui->qcp_motor_cur_2->addGraph();
    ui->qcp_motor_cur_2->graph(0)->setPen(QPen(Qt::blue));
    ui->qcp_motor_cur_2->addGraph();
    ui->qcp_motor_cur_2->graph(1)->setPen(QPen(Qt::red));

    ui->qcp_motor_cur_2->graph(0)->setName("motorCurrent");

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s"); // %h代表小时
    ui->qcp_motor_cur_2->xAxis->setTicker(timeTicker);
    ui->qcp_motor_cur_2->axisRect()->setupFullAxesBox();
    ui->qcp_motor_cur_2->yAxis->setLabel("电流(mA)");

    // make left and bottom axes transfer their ranges to right and top axes:
    connect( ui->qcp_motor_cur_2->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_2->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_cur_2->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_2->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:

    //dataTimer.start(0); // Interval 0 means to refresh as fast as possibl

    ui->qcp_motor_spd_2->addGraph();
    ui->qcp_motor_spd_2->graph(0)->setPen(QPen(Qt::red));
    ui->qcp_motor_spd_2->addGraph();
    ui->qcp_motor_spd_2->graph(1)->setPen(QPen(Qt::blue));

    ui->qcp_motor_spd_2->yAxis->setLabel("转速(rpm)");

    ui->qcp_motor_spd_2->xAxis->setTicker(timeTicker);
    ui->qcp_motor_spd_2->axisRect()->setupFullAxesBox();
    ui->qcp_motor_spd_2->yAxis->setRange(0,1);//与graph(0)和graph(1)没有关系

    connect( ui->qcp_motor_spd_2->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_2->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_spd_2->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_2->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_2->addGraph();
    ui->qcp_motor_act_tor_2->graph(0)->setPen(QPen(Qt::red));

    ui->qcp_motor_act_tor_2->yAxis->setLabel("力矩(mNm)");

    ui->qcp_motor_act_tor_2->xAxis->setTicker(timeTicker);
    ui->qcp_motor_act_tor_2->axisRect()->setupFullAxesBox();
    //ui->widget->yAxis->setRange(-30,30);


    ui->qcp_motor_act_tor_2->xAxis->setTicker(timeTicker);

    connect( ui->qcp_motor_act_tor_2->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_2->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_act_tor_2->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_2->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_spd_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_cur_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}
void MainWindow::initQCustomPlot3()
{

    ui->qcp_motor_cur_3->addGraph();
    ui->qcp_motor_cur_3->graph(0)->setPen(QPen(Qt::blue));
    ui->qcp_motor_cur_3->addGraph();
    ui->qcp_motor_cur_3->graph(1)->setPen(QPen(Qt::red));

    ui->qcp_motor_cur_3->graph(0)->setName("motorCurrent");

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s"); // %h代表小时
    ui->qcp_motor_cur_3->xAxis->setTicker(timeTicker);
    ui->qcp_motor_cur_3->axisRect()->setupFullAxesBox();
    ui->qcp_motor_cur_3->yAxis->setLabel("电流(mA)");

    // make left and bottom axes transfer their ranges to right and top axes:
    connect( ui->qcp_motor_cur_3->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_3->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_cur_3->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_3->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:

    //dataTimer.start(0); // Interval 0 means to refresh as fast as possibl

    ui->qcp_motor_spd_3->addGraph();
    ui->qcp_motor_spd_3->graph(0)->setPen(QPen(Qt::red));
    ui->qcp_motor_spd_3->addGraph();
    ui->qcp_motor_spd_3->graph(1)->setPen(QPen(Qt::blue));

    ui->qcp_motor_spd_3->yAxis->setLabel("转速(rpm)");

    ui->qcp_motor_spd_3->xAxis->setTicker(timeTicker);
    ui->qcp_motor_spd_3->axisRect()->setupFullAxesBox();
    ui->qcp_motor_spd_3->yAxis->setRange(0,1);//与graph(0)和graph(1)没有关系

    connect( ui->qcp_motor_spd_3->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_3->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_spd_3->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_3->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_3->addGraph();
    ui->qcp_motor_act_tor_3->graph(0)->setPen(QPen(Qt::red));

    ui->qcp_motor_act_tor_3->yAxis->setLabel("力矩(mNm)");

    ui->qcp_motor_act_tor_3->xAxis->setTicker(timeTicker);
    ui->qcp_motor_act_tor_3->axisRect()->setupFullAxesBox();
    //ui->widget->yAxis->setRange(-30,30);


    ui->qcp_motor_act_tor_3->xAxis->setTicker(timeTicker);

    connect( ui->qcp_motor_act_tor_3->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_3->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_act_tor_3->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_3->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_spd_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_cur_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}
void MainWindow::initQCustomPlot4()
{

    ui->qcp_motor_cur_4->addGraph();
    ui->qcp_motor_cur_4->graph(0)->setPen(QPen(Qt::blue));
    ui->qcp_motor_cur_4->addGraph();
    ui->qcp_motor_cur_4->graph(1)->setPen(QPen(Qt::red));

    ui->qcp_motor_cur_4->graph(0)->setName("motorCurrent");

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s"); // %h代表小时
    ui->qcp_motor_cur_4->xAxis->setTicker(timeTicker);
    ui->qcp_motor_cur_4->axisRect()->setupFullAxesBox();
    ui->qcp_motor_cur_4->yAxis->setLabel("电流(mA)");

    // make left and bottom axes transfer their ranges to right and top axes:
    connect( ui->qcp_motor_cur_4->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_4->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_cur_4->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_4->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:

    //dataTimer.start(0); // Interval 0 means to refresh as fast as possibl

    ui->qcp_motor_spd_4->addGraph();
    ui->qcp_motor_spd_4->graph(0)->setPen(QPen(Qt::red));
    ui->qcp_motor_spd_4->addGraph();
    ui->qcp_motor_spd_4->graph(1)->setPen(QPen(Qt::blue));

    ui->qcp_motor_spd_4->yAxis->setLabel("转速(rpm)");

    ui->qcp_motor_spd_4->xAxis->setTicker(timeTicker);
    ui->qcp_motor_spd_4->axisRect()->setupFullAxesBox();
    ui->qcp_motor_spd_4->yAxis->setRange(0,1);//与graph(0)和graph(1)没有关系

    connect( ui->qcp_motor_spd_4->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_4->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_spd_4->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_4->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_4->addGraph();
    ui->qcp_motor_act_tor_4->graph(0)->setPen(QPen(Qt::red));

    ui->qcp_motor_act_tor_4->yAxis->setLabel("力矩(mNm)");

    ui->qcp_motor_act_tor_4->xAxis->setTicker(timeTicker);
    ui->qcp_motor_act_tor_4->axisRect()->setupFullAxesBox();
    //ui->widget->yAxis->setRange(-30,30);


    ui->qcp_motor_act_tor_4->xAxis->setTicker(timeTicker);

    connect( ui->qcp_motor_act_tor_4->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_4->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_act_tor_4->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_4->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_4->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_spd_4->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_cur_4->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}
void MainWindow::initQCustomPlot5()
{

    ui->qcp_motor_cur_5->addGraph();
    ui->qcp_motor_cur_5->graph(0)->setPen(QPen(Qt::blue));
    ui->qcp_motor_cur_5->addGraph();
    ui->qcp_motor_cur_5->graph(1)->setPen(QPen(Qt::red));

    ui->qcp_motor_cur_5->graph(0)->setName("motorCurrent");

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s"); // %h代表小时
    ui->qcp_motor_cur_5->xAxis->setTicker(timeTicker);
    ui->qcp_motor_cur_5->axisRect()->setupFullAxesBox();
    ui->qcp_motor_cur_5->yAxis->setLabel("电流(mA)");

    // make left and bottom axes transfer their ranges to right and top axes:
    connect( ui->qcp_motor_cur_5->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_5->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_cur_5->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_5->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:

    //dataTimer.start(0); // Interval 0 means to refresh as fast as possibl

    ui->qcp_motor_spd_5->addGraph();
    ui->qcp_motor_spd_5->graph(0)->setPen(QPen(Qt::red));
    ui->qcp_motor_spd_5->addGraph();
    ui->qcp_motor_spd_5->graph(1)->setPen(QPen(Qt::blue));

    ui->qcp_motor_spd_5->yAxis->setLabel("转速(rpm)");

    ui->qcp_motor_spd_5->xAxis->setTicker(timeTicker);
    ui->qcp_motor_spd_5->axisRect()->setupFullAxesBox();
    ui->qcp_motor_spd_5->yAxis->setRange(0,1);//与graph(0)和graph(1)没有关系

    connect( ui->qcp_motor_spd_5->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_5->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_spd_5->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_5->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_5->addGraph();
    ui->qcp_motor_act_tor_5->graph(0)->setPen(QPen(Qt::red));

    ui->qcp_motor_act_tor_5->yAxis->setLabel("力矩(mNm)");

    ui->qcp_motor_act_tor_5->xAxis->setTicker(timeTicker);
    ui->qcp_motor_act_tor_5->axisRect()->setupFullAxesBox();
    //ui->widget->yAxis->setRange(-30,30);


    ui->qcp_motor_act_tor_5->xAxis->setTicker(timeTicker);

    connect( ui->qcp_motor_act_tor_5->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_5->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_act_tor_5->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_5->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_5->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_spd_5->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_cur_5->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}
void MainWindow::initQCustomPlot6()
{

    ui->qcp_motor_cur_6->addGraph();
    ui->qcp_motor_cur_6->graph(0)->setPen(QPen(Qt::blue));
    ui->qcp_motor_cur_6->addGraph();
    ui->qcp_motor_cur_6->graph(1)->setPen(QPen(Qt::red));

    ui->qcp_motor_cur_6->graph(0)->setName("motorCurrent");

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s"); // %h代表小时
    ui->qcp_motor_cur_6->xAxis->setTicker(timeTicker);
    ui->qcp_motor_cur_6->axisRect()->setupFullAxesBox();
    ui->qcp_motor_cur_6->yAxis->setLabel("电流(mA)");

    // make left and bottom axes transfer their ranges to right and top axes:
    connect( ui->qcp_motor_cur_6->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_6->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_cur_6->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_cur_6->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:

    //dataTimer.start(0); // Interval 0 means to refresh as fast as possibl

    ui->qcp_motor_spd_6->addGraph();
    ui->qcp_motor_spd_6->graph(0)->setPen(QPen(Qt::red));
    ui->qcp_motor_spd_6->addGraph();
    ui->qcp_motor_spd_6->graph(1)->setPen(QPen(Qt::blue));

    ui->qcp_motor_spd_6->yAxis->setLabel("转速(rpm)");

    ui->qcp_motor_spd_6->xAxis->setTicker(timeTicker);
    ui->qcp_motor_spd_6->axisRect()->setupFullAxesBox();
    ui->qcp_motor_spd_6->yAxis->setRange(0,1);//与graph(0)和graph(1)没有关系

    connect( ui->qcp_motor_spd_6->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_6->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_spd_6->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_spd_6->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_6->addGraph();
    ui->qcp_motor_act_tor_6->graph(0)->setPen(QPen(Qt::red));

    ui->qcp_motor_act_tor_6->yAxis->setLabel("力矩(mNm)");

    ui->qcp_motor_act_tor_6->xAxis->setTicker(timeTicker);
    ui->qcp_motor_act_tor_6->axisRect()->setupFullAxesBox();
    //ui->widget->yAxis->setRange(-30,30);


    ui->qcp_motor_act_tor_6->xAxis->setTicker(timeTicker);

    connect( ui->qcp_motor_act_tor_6->xAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_6->xAxis2, SLOT(setRange(QCPRange)));
    connect( ui->qcp_motor_act_tor_6->yAxis, SIGNAL(rangeChanged(QCPRange)),  ui->qcp_motor_act_tor_6->yAxis2, SLOT(setRange(QCPRange)));

    ui->qcp_motor_act_tor_6->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_spd_6->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->qcp_motor_cur_6->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}



//这个六路只要一个就行
void MainWindow::logMsg(QString text)
{
    qDebug() << text;
    ui->statusBar->showMessage(text,5000);
}

void MainWindow::on_pushButton_system_power_1_clicked()
{
    if (!m_sys_status_1_){
        //power not open
        //这里要加检测环节

        if (ui->lineEdit_fwid_1->text().isEmpty()){
            QMessageBox::warning(this,"警告","请输入飞轮ID");
            return;
        }
        if(!checkIsMultiId())
            return;
        p_daqcard_->setDOPort(1,true);//打开继电器，给飞轮上电
        //        double cur_vol = p_daqcard_->getVoltageOfChannel(7) * 10;//六个飞轮用同一个电源供电，数据采集卡的第6通道采集供电电源的电压值
        //        if (cur_vol >30 || cur_vol <= 25){
        //            p_daqcard_->setDOPort(1,false);//关闭继电器，给飞轮断电
        //            ui->pushButton_system_power_1->setText("打开");
        //            QMessageBox::warning(this,"警告","电源电压过高或电源电压过低");
        //            return;
        //        }

        //        qDebug() << "当前电源电压" << cur_vol;

        p_motor1_->setId(ui->lineEdit_fwid_1->text().toInt(nullptr,10));
        p_can_driver_->setFWState(1,true,p_motor1_->getID()); //允许发送识别允许帧
        m_sys_status_1_ = true;
        ui->pushButton_system_power_1->setText("关闭");
        m_timer_update_.setInterval(ui->doubleSpinBox_moto_test_time_1->text().toDouble()*1000);
        p_motor1_->setCurrentInterval(ui->doubleSpinBox_moto_test_time_1->text().toDouble());

        //    p_motor1_->setCurrentInterval(arg1);
        m_timer_get_data_.start();

        //emit canRecovery(p_motor1_->getID());
    }
    else{
        //power had open
        if (ui->pushButton_single_test_mode_1->text() == "停止")
        {
            on_pushButton_single_test_mode_1_clicked();
        }
        if(ui->pushButton_auto_test_noair_power_1->text() == "停止"){
            on_pushButton_auto_test_noair_power_1_clicked();
        }
        p_can_driver_->setFWState(1,false,p_motor1_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(1,false);
        if (p_motor1_->getIsRunning()){
            QMessageBox::warning(this,"警告","依然有任务运行中，但电源已关闭。");
        }
        m_sys_status_1_ = false;
        ui->pushButton_system_power_1->setText("启动");
        m_timer_get_data_.stop();

    }
}
void MainWindow::on_pushButton_system_power_2_clicked()
{
    if (!m_sys_status_2_){
        //power not open

        //这里要加检测环节
        //        double cur_vol = p_daqcard_->getVoltageOfChannel(8) * 10;//六个飞轮用同一个电源供电，数据采集卡的第6通道采集供电电源电压值
        //        if (cur_vol >30 || cur_vol <= 25){
        //            QMessageBox::warning(this,"警告","电源电压过高或电源电压过低");
        //            return;
        //        }
        if (ui->lineEdit_fwid_2->text().isEmpty()){
            QMessageBox::warning(this,"警告","请输入飞轮ID");
            return;
        }
        if(!checkIsMultiId())
            return;

        p_motor2_->setId(ui->lineEdit_fwid_2->text().toInt(nullptr,10));

        p_can_driver_->setFWState(2,true,p_motor2_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(2,true);//打开继电器，给飞轮2上电
        m_sys_status_2_ = true;
        ui->pushButton_system_power_2->setText("关闭");
        m_timer_get_data_2.start();
        m_timer_update_2.setInterval(ui->doubleSpinBox_moto_test_time_2->text().toDouble()*1000);
        p_motor2_->setCurrentInterval(ui->doubleSpinBox_moto_test_time_2->text().toDouble());

       // emit canRecovery(p_motor2_->getID());
    }
    else{
        //power had open

        if (ui->pushButton_single_test_mode_2->text() == "停止")
        {
            on_pushButton_single_test_mode_2_clicked();
        }
        if(ui->pushButton_auto_test_noair_power_2->text() == "停止"){
            on_pushButton_auto_test_noair_power_2_clicked();
        }
        p_can_driver_->setFWState(2,false,p_motor2_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(2,false);
        m_sys_status_2_ = false;
        ui->pushButton_system_power_2->setText("启动");
        m_timer_get_data_2.stop();
        if (p_motor2_->getIsRunning()){
            QMessageBox::warning(this,"警告","依然有任务运行中，但电源已关闭。");
        }
    }
}
void MainWindow::on_pushButton_system_power_3_clicked()
{
    if (!m_sys_status_3_){
        //power not open

        //这里要加检测环节
        //        double cur_vol = p_daqcard_->getVoltageOfChannel(9) * 10;//六个飞轮用同一个电源供电，数据采集卡的第6通道采集供电电源电压值
        //        if (cur_vol >30 || cur_vol <= 25){
        //            QMessageBox::warning(this,"警告","电源电压过高或电源电压过低");
        //            return;
        //        }
        if (ui->lineEdit_fwid_3->text().isEmpty()){
            QMessageBox::warning(this,"警告","请输入飞轮ID");
            return;
        }
        if(!checkIsMultiId())
            return;

        p_motor3_->setId(ui->lineEdit_fwid_3->text().toInt(nullptr,10));

        p_can_driver_->setFWState(3,true,p_motor3_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(3,true);//打开继电器，给飞轮2上电
        m_sys_status_3_ = true;
        ui->pushButton_system_power_3->setText("关闭");
        m_timer_get_data_3.start();
        m_timer_update_3.setInterval(ui->doubleSpinBox_moto_test_time_3->text().toDouble()*1000);
        p_motor3_->setCurrentInterval(ui->doubleSpinBox_moto_test_time_3->text().toDouble());

        //emit canRecovery(p_motor3_->getID());
    }
    else{
        //power had open
        if (ui->pushButton_single_test_mode_3->text() == "停止")
        {
            on_pushButton_single_test_mode_3_clicked();
        }
        if(ui->pushButton_auto_test_noair_power_3->text() == "停止"){
            on_pushButton_auto_test_noair_power_3_clicked();
        }
        p_can_driver_->setFWState(3,false,p_motor3_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(3,false);
        m_sys_status_3_ = false;
        ui->pushButton_system_power_3->setText("启动");
        m_timer_get_data_3.stop();
        if (p_motor3_->getIsRunning()){
            QMessageBox::warning(this,"警告","依然有任务运行中，但电源已关闭。");
        }
    }
}
void MainWindow::on_pushButton_system_power_4_clicked()
{
    if (!m_sys_status_4_){
        //power not open
        //这里要加检测环节
        //        double cur_vol = p_daqcard_->getVoltageOfChannel(10) * 10;//六个飞轮用同一个电源供电，数据采集卡的第6通道采集供电电源电压值
        //        if (cur_vol >30 || cur_vol <= 25){
        //            QMessageBox::warning(this,"警告","电源电压过高或电源电压过低");
        //            return;
        //        }
        if (ui->lineEdit_fwid_4->text().isEmpty()){
            QMessageBox::warning(this,"警告","请输入飞轮ID");
            return;
        }
        if(!checkIsMultiId())
            return;

        p_motor4_->setId(ui->lineEdit_fwid_4->text().toInt(nullptr,10));

        p_can_driver_->setFWState(4,true,p_motor4_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(4,true);//打开继电器，给飞轮4上电
        m_sys_status_4_ = true;
        ui->pushButton_system_power_4->setText("关闭");
        m_timer_get_data_4.start();
        m_timer_update_4.setInterval(ui->doubleSpinBox_moto_test_time_4->text().toDouble()*1000);
        p_motor4_->setCurrentInterval(ui->doubleSpinBox_moto_test_time_4->text().toDouble());

        //emit canRecovery(p_motor4_->getID());
    }
    else{
        //power had open
        if (ui->pushButton_single_test_mode_4->text() == "停止")
        {
            on_pushButton_single_test_mode_4_clicked();
        }
        if(ui->pushButton_auto_test_noair_power_4->text() == "停止"){
            on_pushButton_auto_test_noair_power_4_clicked();
        }
        p_can_driver_->setFWState(4,false,p_motor4_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(4,false);
        m_sys_status_4_ = false;
        ui->pushButton_system_power_4->setText("启动");
        m_timer_get_data_4.stop();
        if (p_motor4_->getIsRunning()){
            QMessageBox::warning(this,"警告","依然有任务运行中，但电源已关闭。");
        }
    }
}
void MainWindow::on_pushButton_system_power_5_clicked()
{
    if (!m_sys_status_5_){
        //power not open
        //这里要加检测环节
        //        double cur_vol = p_daqcard_->getVoltageOfChannel(11) * 10;//六个飞轮用同一个电源供电，数据采集卡的第6通道采集供电电源电压值
        //        if (cur_vol >30 || cur_vol <= 25){
        //            QMessageBox::warning(this,"警告","电源电压过高或电源电压过低");
        //            return;
        //        }
        if (ui->lineEdit_fwid_5->text().isEmpty()){
            QMessageBox::warning(this,"警告","请输入飞轮ID");
            return;
        }
        if(!checkIsMultiId())
            return;

        p_motor5_->setId(ui->lineEdit_fwid_5->text().toInt(nullptr,10));

        p_can_driver_->setFWState(5,true,p_motor5_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(5,true);//打开继电器，给飞轮4上电
        m_sys_status_5_ = true;
        ui->pushButton_system_power_5->setText("关闭");
        m_timer_get_data_5.start();
        m_timer_update_5.setInterval(ui->doubleSpinBox_moto_test_time_5->text().toDouble()*1000);
        p_motor5_->setCurrentInterval(ui->doubleSpinBox_moto_test_time_5->text().toDouble());

        //emit canRecovery(p_motor5_->getID());
    }
    else{
        //power had open
        if (ui->pushButton_single_test_mode_5->text() == "停止")
        {
            on_pushButton_single_test_mode_5_clicked();
        }
        if(ui->pushButton_auto_test_noair_power_5->text() == "停止"){
            on_pushButton_auto_test_noair_power_5_clicked();
        }
        p_can_driver_->setFWState(5,false,p_motor5_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(5,false);
        m_sys_status_5_ = false;
        ui->pushButton_system_power_5->setText("启动");
        m_timer_get_data_5.stop();
        if (p_motor5_->getIsRunning()){
            QMessageBox::warning(this,"警告","依然有任务运行中，但电源已关闭。");
        }
    }
}
void MainWindow::on_pushButton_system_power_6_clicked()
{
    if (!m_sys_status_6_){
        //power not open
        //这里要加检测环节
        //        double cur_vol = p_daqcard_->getVoltageOfChannel(12) * 10;//六个飞轮用同一个电源供电，数据采集卡的第6通道采集供电电源电压值
        //        if (cur_vol >30 || cur_vol <= 25){
        //            QMessageBox::warning(this,"警告","电源电压过高或电源电压过低");
        //            return;
        //        }
        if (ui->lineEdit_fwid_6->text().isEmpty()){
            QMessageBox::warning(this,"警告","请输入飞轮ID");
            return;
        }
        if(!checkIsMultiId())
            return;

        p_motor6_->setId(ui->lineEdit_fwid_6->text().toInt(nullptr,10));

        p_can_driver_->setFWState(6,true,p_motor6_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(6,true);//打开继电器，给飞轮6上电
        m_sys_status_6_ = true;
        ui->pushButton_system_power_6->setText("关闭");
        m_timer_get_data_6.start();
        m_timer_update_6.setInterval(ui->doubleSpinBox_moto_test_time_6->text().toDouble()*1000);
        p_motor6_->setCurrentInterval(ui->doubleSpinBox_moto_test_time_6->text().toDouble());

        //emit canRecovery(p_motor6_->getID());
    }
    else{
        //power had open
        if (ui->pushButton_single_test_mode_6->text() == "停止")
        {
            on_pushButton_single_test_mode_6_clicked();
        }
        if(ui->pushButton_auto_test_noair_power_6->text() == "停止"){
            on_pushButton_auto_test_noair_power_6_clicked();
        }
        p_can_driver_->setFWState(6,false,p_motor6_->getID()); //允许发送识别允许帧
        p_daqcard_->setDOPort(6,false);
        m_sys_status_6_ = false;
        ui->pushButton_system_power_6->setText("启动");
        m_timer_get_data_6.stop();
        if (p_motor6_->getIsRunning()){
            QMessageBox::warning(this,"警告","依然有任务运行中，但电源已关闭。");
        }
    }
}

void MainWindow::on_pushButton_single_test_mode_1_clicked()
{
    //mdoea = true;
    static bool this_mode_running = false;
    static bool xp_mode_running = false;
    static bool spd_sin_running = false;
    static bool time_constant_running = false;
    static bool hx_mode_running = false;
    //static bool ljxp_mode_running = false;
    static bool sm_mode_running = false;
    if (!m_sys_status_1_){
        QMessageBox::warning(this,"电源未打开","电源未打开,请开启总电源！");
    }

    else{
        if(!p_motor1_->getIsRunning() && !this_mode_running){
            int t = 0;
            //p_motor1_->setId(ui->lineEdit_flywheel_id->text().toInt());
            switch(ui->comboBox_motor_test_mode_1->currentIndex()){
            case 0://速度模式
                p_motor1_->setFlywheelMode(FLYWHEEL_MODE_SPD);//这个可能要与一键测试区分开
                p_motor1_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_1->text().toDouble());
                break;
            case 1://斜坡模式
                //p_motor1_->setSetSpeed(0);
                p_motor1_->initXpMode(ui->doubleSpinBox_motor_test_spd_1->text().toDouble()
                                      ,ui->doubleSpinBox_motor_test_acc_1->text().toDouble());
                connect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(calXpMode()));
                xp_mode_running = true;
                p_motor1_->setXpStatus(true);
                break;
            case 2://正弦模式
                p_motor1_->setSetSpeed(0);
                //p_motor1_->setAccelerate(ui->doubleSpinBox_motor_test_acc_1->text().toDouble());
                p_motor1_->initsdsin(ui->doubleSpinBox_sin_K_1->text().toDouble()
                                     ,ui->spinBox_sin_A_1->text().toDouble()
                                     ,ui->spinBox_sin_T_1->text().toDouble()
                                     ,0);
                connect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(calSpdCosMode()));
                spd_sin_running = true;
                p_motor1_->setsdsinStatus(true);
                break;
            case 3://力矩模式
                p_motor1_->setFlywheelMode(FLYWHEEL_MODE_TOR);
                p_motor1_->setSetTorque(ui->doubleSpinBox_motor_test_spd_1->text().toDouble());

                break;
            case 4://时间常数计算

                //p_motor1_->setSetSpeed(100);
                connect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(caltimeconstant()));
                time_constant_running = true;
                p_motor1_->settimeconstant(true);
                break;
            case 5://滑行模式
                p_motor1_->initHXMode(ui->doubleSpinBox_motor_test_spd_1->text().toDouble());//这个可能要从界面上传参数进来
                connect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(setSpdOfAutoTestHXMode()));
                //可能还需要设置滑行的状态
                hx_mode_running = true;
                break;
            case 6://寿命模式
                sm_mode_running = true;
                p_motor1_->setFlywheelMode(FLYWHEEL_MODE_SM);
                p_motor1_->initSMMode(ui->doubleSpinBox_motor_test_spd_1->text().toDouble(),ui->doubleSpinBox_motor_test_acc_1->text().toDouble());
                connect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(calSMModeSpd()));
                break;
            case 7:
                p_motor1_->setFlywheelMode(FLYWHEEL_MODE_DL);
                p_motor1_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_1->text().toDouble());
                break;
            default:
                break;
            }
            //            m_timer_get_data_.setInterval();

            keyContainer.resize(0);
            //tmpContainer.resize(0);
            torContainer.resize(0);
            curContainer.resize(0);
            spdContainer.resize(0);
            setSpdContainer.resize(0);

            p_motor1_->setIsRunning(true);
            //            m_timer_get_data_.start();
            this_mode_running = true;
            ui->pushButton_single_test_mode_1->setText("停止");
            ui->comboBox_motor_test_mode_1->setEnabled(false);
        }
        else{
            //停止的处理函数
            //这里斜坡计数要清零
            if (this_mode_running){
                if (xp_mode_running){
                    disconnect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(calXpMode()));
                    xp_mode_running = false;
                    p_motor1_->setXpStatus(false);
                }
                if(spd_sin_running){
                    disconnect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(calSpdCosMode()));
                    spd_sin_running = false;
                    p_motor1_->setsdsinStatus(false);
                }
                if(time_constant_running){
                    disconnect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(caltimeconstant()));
                    spd_sin_running = false;
                    p_motor1_->setsdsinStatus(false);
                }
                if(hx_mode_running){
                    disconnect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(setSpdOfAutoTestHXMode()));
                    hx_mode_running = false;
                }
                if (sm_mode_running){
                    disconnect(&m_timer_update_,SIGNAL(timeout()),p_motor1_,SLOT(calSMModeSpd()));
                    sm_mode_running = false;
                }
                p_motor1_->setIsRunning(false);
                p_motor1_->setSetSpeed(0);
                p_motor1_->setXpCount(0);
                //p_motor1_->setAccelerate(0);
                //m_timer_get_data_.stop();

                this_mode_running = false;

                ui->pushButton_single_test_mode_1->setText("启动");
                ui->comboBox_motor_test_mode_1->setEnabled(true);
            }
            else{
                QMessageBox::warning(this,"警告","其它模式运行中。");
            }
        }
    }
}
void MainWindow::on_pushButton_single_test_mode_2_clicked()
{
    static bool this_mode_running = false;
    static bool xp_mode_running = false;
    static bool spd_sin_running = false;
    static bool time_constant_running = false;
    static bool hx_mode_running = false;
    //static bool ljxp_mode_running = false;
    static bool sm_mode_running = false;
    if (!m_sys_status_2_){
        QMessageBox::warning(this,"电源未打开","电源未打开,请开启总电源！");
    }

    else{
        if(!p_motor2_->getIsRunning() && !this_mode_running){
            int t = 0;
            //p_motor1_->setId(ui->lineEdit_flywheel_id->text().toInt());
            switch(ui->comboBox_motor_test_mode_2->currentIndex()){
            case 0://速度模式
                p_motor2_->setFlywheelMode(FLYWHEEL_MODE_SPD);
                p_motor2_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_2->text().toDouble());
                break;
            case 1://斜坡模式
                //p_motor1_->setSetSpeed(0);
                p_motor2_->initXpMode(ui->doubleSpinBox_motor_test_spd_2->text().toDouble()
                                      ,ui->doubleSpinBox_motor_test_acc_2->text().toDouble());
                connect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(calXpMode()));
                xp_mode_running = true;
                p_motor2_->setXpStatus(true);
                break;
            case 2://正弦模式
                p_motor2_->setSetSpeed(0);
                //p_motor1_->setAccelerate(ui->doubleSpinBox_motor_test_acc_1->text().toDouble());
                p_motor2_->initsdsin(ui->doubleSpinBox_sin_K_2->text().toDouble()
                                     ,ui->spinBox_sin_A_2->text().toDouble()
                                     ,ui->spinBox_sin_T_2->text().toDouble()
                                     ,t);
                connect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(calSpdCosMode()));
                spd_sin_running = true;
                p_motor2_->setsdsinStatus(true);
                break;
            case 3://力矩模式
                p_motor2_->setFlywheelMode(FLYWHEEL_MODE_TOR);
                p_motor2_->setSetTorque(ui->doubleSpinBox_motor_test_spd_2->text().toDouble());
                break;
            case 4://时间常数计算
                //p_motor1_->setSetSpeed(100);
                connect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(caltimeconstant()));
                time_constant_running = true;
                p_motor2_->settimeconstant(true);
                break;
            case 5://滑行模式
                p_motor2_->initHXMode(ui->doubleSpinBox_motor_test_spd_2->text().toDouble());//这个可能要从界面上传参数进来
                connect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(setSpdOfAutoTestHXMode()));
                //可能还需要设置滑行的状态
                hx_mode_running = true;
                break;
            case 6://寿命模式
                sm_mode_running = true;
                p_motor2_->setFlywheelMode(FLYWHEEL_MODE_SM);
                p_motor2_->initSMMode(ui->doubleSpinBox_motor_test_spd_2->text().toDouble(),ui->doubleSpinBox_motor_test_acc_2->text().toDouble());
                connect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(calSMModeSpd()));
                break;
            case 7:
                p_motor2_->setFlywheelMode(FLYWHEEL_MODE_DL);
                p_motor2_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_2->text().toDouble());
                break;
            default:
                break;
            }
            //            m_timer_get_data_.setInterval();

            keyContainer_2.resize(0);
            //tmpContainer_2.resize(0);
            torContainer_2.resize(0);
            curContainer_2.resize(0);
            spdContainer_2.resize(0);
            setSpdContainer_2.resize(0);

            p_motor2_->setIsRunning(true);
            //            m_timer_get_data_2.start();
            this_mode_running = true;
            ui->pushButton_single_test_mode_2->setText("停止");
            ui->comboBox_motor_test_mode_2->setEnabled(false);
        }
        else{//这里斜坡计数要清零
            if (this_mode_running){
                if (xp_mode_running){
                    disconnect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(calXpMode()));
                    xp_mode_running = false;
                    p_motor2_->setXpStatus(false);
                }
                if(spd_sin_running){
                    disconnect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(calSpdCosMode()));
                    spd_sin_running = false;
                    p_motor2_->setsdsinStatus(false);
                }
                if(time_constant_running){
                    disconnect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(caltimeconstant()));
                    spd_sin_running = false;
                    p_motor2_->setsdsinStatus(false);
                }
                if(hx_mode_running){
                    disconnect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(setSpdOfAutoTestHXMode()));
                    hx_mode_running = false;
                }
                if (sm_mode_running){
                    disconnect(&m_timer_update_2,SIGNAL(timeout()),p_motor2_,SLOT(calSMModeSpd()));
                    sm_mode_running = false;
                }
                p_motor2_->setIsRunning(false);
                p_motor2_->setSetSpeed(0);
                p_motor2_->setXpCount(0);
                //p_motor1_->setAccelerate(0);
                //                m_timer_get_data_2.stop();
                this_mode_running = false;

                ui->pushButton_single_test_mode_2->setText("启动");
                ui->comboBox_motor_test_mode_2->setEnabled(true);
            }
            else{
                QMessageBox::warning(this,"警告","其它模式运行中。");
            }
        }
    }
}
void MainWindow::on_pushButton_single_test_mode_3_clicked()
{
    static bool this_mode_running = false;
    static bool xp_mode_running = false;
    static bool spd_sin_running = false;
    static bool time_constant_running = false;
    static bool hx_mode_running = false;
    //static bool ljxp_mode_running = false;
    static bool sm_mode_running = false;
    if (!m_sys_status_3_){
        QMessageBox::warning(this,"电源未打开","电源未打开,请开启总电源！");
    }

    else{
        if(!p_motor3_->getIsRunning() && !this_mode_running){
            int t = 0;
            //p_motor1_->setId(ui->lineEdit_flywheel_id->text().toInt());
            switch(ui->comboBox_motor_test_mode_3->currentIndex()){
            case 0://速度模式
                p_motor3_->setFlywheelMode(FLYWHEEL_MODE_SPD);
                p_motor3_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_3->text().toDouble());
                break;
            case 1://斜坡模式
                //p_motor1_->setSetSpeed(0);
                p_motor3_->initXpMode(ui->doubleSpinBox_motor_test_spd_3->text().toDouble()
                                      ,ui->doubleSpinBox_motor_test_acc_3->text().toDouble());
                connect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(calXpMode()));
                xp_mode_running = true;
                p_motor3_->setXpStatus(true);
                break;
            case 2://正弦模式
                p_motor3_->setSetSpeed(0);
                //p_motor1_->setAccelerate(ui->doubleSpinBox_motor_test_acc_1->text().toDouble());
                p_motor3_->initsdsin(ui->doubleSpinBox_sin_K_3->text().toDouble()
                                     ,ui->spinBox_sin_A_3->text().toDouble()
                                     ,ui->spinBox_sin_T_3->text().toDouble()
                                     ,t);
                connect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(calSpdCosMode()));
                spd_sin_running = true;
                p_motor3_->setsdsinStatus(true);
                break;
            case 3://力矩模式
                p_motor3_->setFlywheelMode(FLYWHEEL_MODE_TOR);
                p_motor3_->setSetTorque(ui->doubleSpinBox_motor_test_spd_3->text().toDouble());
                break;
            case 4://时间常数计算
                //p_motor1_->setSetSpeed(100);
                connect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(caltimeconstant()));
                time_constant_running = true;
                p_motor3_->settimeconstant(true);
                break;
            case 5://滑行模式
                p_motor3_->initHXMode(ui->doubleSpinBox_motor_test_spd_3->text().toDouble());//这个可能要从界面上传参数进来
                connect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(setSpdOfAutoTestHXMode()));
                //可能还需要设置滑行的状态
                hx_mode_running = true;
                break;
            case 6://寿命模式
                sm_mode_running = true;
                p_motor3_->setFlywheelMode(FLYWHEEL_MODE_SM);
                p_motor3_->initSMMode(ui->doubleSpinBox_motor_test_spd_3->text().toDouble(),ui->doubleSpinBox_motor_test_acc_3->text().toDouble());
                connect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(calSMModeSpd()));
                break;
            case 7:
                p_motor3_->setFlywheelMode(FLYWHEEL_MODE_DL);
                p_motor3_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_3->text().toDouble());
                break;
            default:
                break;
            }
            //            m_timer_get_data_.setInterval();

            keyContainer_3.resize(0);
            //tmpContainer_2.resize(0);
            torContainer_3.resize(0);
            curContainer_3.resize(0);
            spdContainer_3.resize(0);
            setSpdContainer_3.resize(0);

            p_motor3_->setIsRunning(true);
            //            m_timer_get_data_3.start();
            this_mode_running = true;
            ui->pushButton_single_test_mode_3->setText("停止");
            ui->comboBox_motor_test_mode_3->setEnabled(false);
        }
        else{//这里斜坡计数要清零
            if (this_mode_running){
                if (xp_mode_running){
                    disconnect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(calXpMode()));
                    xp_mode_running = false;
                    p_motor3_->setXpStatus(false);
                }
                if(spd_sin_running){
                    disconnect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(calSpdCosMode()));
                    spd_sin_running = false;
                    p_motor3_->setsdsinStatus(false);
                }
                if(time_constant_running){
                    disconnect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(caltimeconstant()));
                    spd_sin_running = false;
                    p_motor3_->setsdsinStatus(false);
                }
                if(hx_mode_running){
                    disconnect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(setSpdOfAutoTestHXMode()));
                    hx_mode_running = false;
                }
                if (sm_mode_running){
                    disconnect(&m_timer_update_3,SIGNAL(timeout()),p_motor3_,SLOT(calSMModeSpd()));
                    sm_mode_running = false;
                }
                p_motor3_->setIsRunning(false);
                p_motor3_->setSetSpeed(0);
                p_motor3_->setXpCount(0);
                //p_motor1_->setAccelerate(0);
                //                m_timer_get_data_3.stop();
                this_mode_running = false;

                ui->pushButton_single_test_mode_3->setText("启动");
                ui->comboBox_motor_test_mode_3->setEnabled(true);
            }
            else{
                QMessageBox::warning(this,"警告","其它模式运行中。");
            }
        }
    }
}
void MainWindow::on_pushButton_single_test_mode_4_clicked()
{
    static bool this_mode_running = false;
    static bool xp_mode_running = false;
    static bool spd_sin_running = false;
    static bool time_constant_running = false;
    static bool hx_mode_running = false;
    //static bool ljxp_mode_running = false;
    static bool sm_mode_running = false;
    if (!m_sys_status_4_){
        QMessageBox::warning(this,"电源未打开","电源未打开,请开启总电源！");
    }

    else{
        if(!p_motor4_->getIsRunning() && !this_mode_running){
            int t = 0;
            //p_motor1_->setId(ui->lineEdit_flywheel_id->text().toInt());
            switch(ui->comboBox_motor_test_mode_4->currentIndex()){
            case 0://速度模式
                p_motor4_->setFlywheelMode(FLYWHEEL_MODE_SPD);
                p_motor4_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_4->text().toDouble());
                break;
            case 1://斜坡模式
                //p_motor1_->setSetSpeed(0);
                p_motor4_->initXpMode(ui->doubleSpinBox_motor_test_spd_4->text().toDouble()
                                      ,ui->doubleSpinBox_motor_test_acc_4->text().toDouble());
                connect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(calXpMode()));
                xp_mode_running = true;
                p_motor4_->setXpStatus(true);
                break;
            case 2://正弦模式
                p_motor4_->setSetSpeed(0);
                //p_motor1_->setAccelerate(ui->doubleSpinBox_motor_test_acc_1->text().toDouble());
                p_motor4_->initsdsin(ui->doubleSpinBox_sin_K_4->text().toDouble()
                                     ,ui->spinBox_sin_A_4->text().toDouble()
                                     ,ui->spinBox_sin_T_4->text().toDouble()
                                     ,t);
                connect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(calSpdCosMode()));
                spd_sin_running = true;
                p_motor4_->setsdsinStatus(true);
                break;
            case 3://力矩模式
                p_motor4_->setFlywheelMode(FLYWHEEL_MODE_TOR);
                p_motor4_->setSetTorque(ui->doubleSpinBox_motor_test_spd_4->text().toDouble());
                break;
            case 4://时间常数计算
                //p_motor1_->setSetSpeed(100);
                connect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(caltimeconstant()));
                time_constant_running = true;
                p_motor4_->settimeconstant(true);
                break;
            case 5://滑行模式
                p_motor4_->initHXMode(ui->doubleSpinBox_motor_test_spd_4->text().toDouble());//这个可能要从界面上传参数进来
                connect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(setSpdOfAutoTestHXMode()));
                //可能还需要设置滑行的状态
                hx_mode_running = true;
                break;
            case 6://寿命模式
                sm_mode_running = true;
                p_motor4_->setFlywheelMode(FLYWHEEL_MODE_SM);
                p_motor4_->initSMMode(ui->doubleSpinBox_motor_test_spd_4->text().toDouble(),ui->doubleSpinBox_motor_test_acc_4->text().toDouble());
                connect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(calSMModeSpd()));
                break;
            case 7:
                p_motor4_->setFlywheelMode(FLYWHEEL_MODE_DL);
                p_motor4_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_4->text().toDouble());
                break;
            default:
                break;
            }
            //            m_timer_get_data_.setInterval();

            keyContainer_4.resize(0);
            //tmpContainer_2.resize(0);
            torContainer_4.resize(0);
            curContainer_4.resize(0);
            spdContainer_4.resize(0);
            setSpdContainer_4.resize(0);

            p_motor4_->setIsRunning(true);
            //            m_timer_get_data_4.start();
            this_mode_running = true;
            ui->pushButton_single_test_mode_4->setText("停止");
            ui->comboBox_motor_test_mode_4->setEnabled(false);
        }
        else{//这里斜坡计数要清零
            if (this_mode_running){
                if (xp_mode_running){
                    disconnect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(calXpMode()));
                    xp_mode_running = false;
                    p_motor4_->setXpStatus(false);
                }
                if(spd_sin_running){
                    disconnect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(calSpdCosMode()));
                    spd_sin_running = false;
                    p_motor4_->setsdsinStatus(false);
                }
                if(time_constant_running){
                    disconnect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(caltimeconstant()));
                    spd_sin_running = false;
                    p_motor4_->setsdsinStatus(false);
                }
                if(hx_mode_running){
                    disconnect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(setSpdOfAutoTestHXMode()));
                    hx_mode_running = false;
                }
                if (sm_mode_running){
                    disconnect(&m_timer_update_4,SIGNAL(timeout()),p_motor4_,SLOT(calSMModeSpd()));
                    sm_mode_running = false;
                }
                p_motor4_->setIsRunning(false);
                p_motor4_->setSetSpeed(0);
                p_motor4_->setXpCount(0);
                //p_motor1_->setAccelerate(0);
                //                m_timer_get_data_4.stop();
                this_mode_running = false;

                ui->pushButton_single_test_mode_4->setText("启动");
                ui->comboBox_motor_test_mode_4->setEnabled(true);
            }
            else{
                QMessageBox::warning(this,"警告","其它模式运行中。");
            }
        }
    }
}
void MainWindow::on_pushButton_single_test_mode_5_clicked()
{
    static bool this_mode_running = false;
    static bool xp_mode_running = false;
    static bool spd_sin_running = false;
    static bool time_constant_running = false;
    static bool hx_mode_running = false;
    //static bool ljxp_mode_running = false;
    static bool sm_mode_running = false;
    if (!m_sys_status_5_){
        QMessageBox::warning(this,"电源未打开","电源未打开,请开启总电源！");
    }

    else{
        if(!p_motor5_->getIsRunning() && !this_mode_running){
            int t = 0;
            //p_motor1_->setId(ui->lineEdit_flywheel_id->text().toInt());
            switch(ui->comboBox_motor_test_mode_5->currentIndex()){
            case 0://速度模式
                p_motor5_->setFlywheelMode(FLYWHEEL_MODE_SPD);
                p_motor5_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_5->text().toDouble());
                break;
            case 1://斜坡模式
                //p_motor1_->setSetSpeed(0);
                p_motor5_->initXpMode(ui->doubleSpinBox_motor_test_spd_5->text().toDouble()
                                      ,ui->doubleSpinBox_motor_test_acc_5->text().toDouble());
                connect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(calXpMode()));
                xp_mode_running = true;
                p_motor5_->setXpStatus(true);
                break;
            case 2://正弦模式
                p_motor5_->setSetSpeed(0);
                //p_motor1_->setAccelerate(ui->doubleSpinBox_motor_test_acc_1->text().toDouble());
                p_motor5_->initsdsin(ui->doubleSpinBox_sin_K_5->text().toDouble()
                                     ,ui->spinBox_sin_A_5->text().toDouble()
                                     ,ui->spinBox_sin_T_5->text().toDouble()
                                     ,t);
                connect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(calSpdCosMode()));
                spd_sin_running = true;
                p_motor5_->setsdsinStatus(true);
                break;
            case 3://力矩模式
                p_motor5_->setFlywheelMode(FLYWHEEL_MODE_TOR);
                p_motor5_->setSetTorque(ui->doubleSpinBox_motor_test_spd_5->text().toDouble());
                break;
            case 4://时间常数计算
                //p_motor1_->setSetSpeed(100);
                connect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(caltimeconstant()));
                time_constant_running = true;
                p_motor5_->settimeconstant(true);
                break;
            case 5://滑行模式
                p_motor5_->initHXMode(ui->doubleSpinBox_motor_test_spd_5->text().toDouble());//这个可能要从界面上传参数进来
                connect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(setSpdOfAutoTestHXMode()));
                //可能还需要设置滑行的状态
                hx_mode_running = true;
                break;
            case 6://寿命模式
                sm_mode_running = true;
                p_motor5_->setFlywheelMode(FLYWHEEL_MODE_SM);
                p_motor5_->initSMMode(ui->doubleSpinBox_motor_test_spd_5->text().toDouble(),ui->doubleSpinBox_motor_test_acc_5->text().toDouble());
                connect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(calSMModeSpd()));
                break;
            case 7:
                p_motor5_->setFlywheelMode(FLYWHEEL_MODE_DL);
                p_motor5_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_5->text().toDouble());
                break;
            default:
                break;
            }
            //            m_timer_get_data_.setInterval();

            keyContainer_5.resize(0);
            //tmpContainer_2.resize(0);
            torContainer_5.resize(0);
            curContainer_5.resize(0);
            spdContainer_5.resize(0);
            setSpdContainer_5.resize(0);

            p_motor5_->setIsRunning(true);
            //            m_timer_get_data_5.start();
            this_mode_running = true;
            ui->pushButton_single_test_mode_5->setText("停止");
            ui->comboBox_motor_test_mode_5->setEnabled(false);
        }
        else{//这里斜坡计数要清零
            if (this_mode_running){
                if (xp_mode_running){
                    disconnect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(calXpMode()));
                    xp_mode_running = false;
                    p_motor5_->setXpStatus(false);
                }
                if(spd_sin_running){
                    disconnect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(calSpdCosMode()));
                    spd_sin_running = false;
                    p_motor5_->setsdsinStatus(false);
                }
                if(time_constant_running){
                    disconnect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(caltimeconstant()));
                    spd_sin_running = false;
                    p_motor5_->setsdsinStatus(false);
                }
                if(hx_mode_running){
                    disconnect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(setSpdOfAutoTestHXMode()));
                    hx_mode_running = false;
                }
                if (sm_mode_running){
                    disconnect(&m_timer_update_5,SIGNAL(timeout()),p_motor5_,SLOT(calSMModeSpd()));
                    sm_mode_running = false;
                }
                p_motor5_->setIsRunning(false);
                p_motor5_->setSetSpeed(0);
                p_motor5_->setXpCount(0);
                //p_motor1_->setAccelerate(0);
                //                m_timer_get_data_5.stop();
                this_mode_running = false;

                ui->pushButton_single_test_mode_5->setText("启动");
                ui->comboBox_motor_test_mode_5->setEnabled(true);
            }
            else{
                QMessageBox::warning(this,"警告","其它模式运行中。");
            }
        }
    }
}
void MainWindow::on_pushButton_single_test_mode_6_clicked()
{
    static bool this_mode_running = false;
    static bool xp_mode_running = false;
    static bool spd_sin_running = false;
    static bool time_constant_running = false;
    static bool hx_mode_running = false;
    //static bool ljxp_mode_running = false;
    static bool sm_mode_running = false;
    if (!m_sys_status_6_){
        QMessageBox::warning(this,"电源未打开","电源未打开,请开启总电源！");
    }

    else{
        if(!p_motor6_->getIsRunning() && !this_mode_running){
            int t = 0;
            //p_motor1_->setId(ui->lineEdit_flywheel_id->text().toInt());
            switch(ui->comboBox_motor_test_mode_6->currentIndex()){
            case 0://速度模式
                p_motor6_->setFlywheelMode(FLYWHEEL_MODE_SPD);
                p_motor6_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_6->text().toDouble());
                break;
            case 1://斜坡模式
                //p_motor1_->setSetSpeed(0);
                p_motor6_->initXpMode(ui->doubleSpinBox_motor_test_spd_6->text().toDouble()
                                      ,ui->doubleSpinBox_motor_test_acc_6->text().toDouble());
                connect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(calXpMode()));
                xp_mode_running = true;
                p_motor6_->setXpStatus(true);
                break;
            case 2://正弦模式
                p_motor6_->setSetSpeed(0);
                //p_motor1_->setAccelerate(ui->doubleSpinBox_motor_test_acc_1->text().toDouble());
                p_motor6_->initsdsin(ui->doubleSpinBox_sin_K_6->text().toDouble()
                                     ,ui->spinBox_sin_A_6->text().toDouble()
                                     ,ui->spinBox_sin_T_6->text().toDouble()
                                     ,t);
                connect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(calSpdCosMode()));
                spd_sin_running = true;
                p_motor6_->setsdsinStatus(true);
                break;
            case 3://力矩模式
                p_motor6_->setFlywheelMode(FLYWHEEL_MODE_TOR);
                p_motor6_->setSetTorque(ui->doubleSpinBox_motor_test_spd_6->text().toDouble());
                break;
            case 4://时间常数计算
                //p_motor1_->setSetSpeed(100);
                connect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(caltimeconstant()));
                time_constant_running = true;
                p_motor6_->settimeconstant(true);
                break;
            case 5://滑行模式
                p_motor6_->initHXMode(ui->doubleSpinBox_motor_test_spd_6->text().toDouble());//这个可能要从界面上传参数进来
                connect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(setSpdOfAutoTestHXMode()));
                //可能还需要设置滑行的状态
                hx_mode_running = true;
                break;
            case 6://寿命模式
                sm_mode_running = true;
                p_motor6_->setFlywheelMode(FLYWHEEL_MODE_SM);
                p_motor6_->initSMMode(ui->doubleSpinBox_motor_test_spd_6->text().toDouble(),ui->doubleSpinBox_motor_test_acc_6->text().toDouble());
                connect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(calSMModeSpd()));
                break;
            case 7:
                p_motor6_->setFlywheelMode(FLYWHEEL_MODE_DL);
                p_motor6_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_6->text().toDouble());
                break;
            default:
                break;
            }
            //            m_timer_get_data_.setInterval();

            keyContainer_6.resize(0);
            //tmpContainer_2.resize(0);
            torContainer_6.resize(0);
            curContainer_6.resize(0);
            spdContainer_6.resize(0);
            setSpdContainer_6.resize(0);

            p_motor6_->setIsRunning(true);

            this_mode_running = true;
            ui->pushButton_single_test_mode_6->setText("停止");
            ui->comboBox_motor_test_mode_6->setEnabled(false);
        }
        else{//这里斜坡计数要清零
            if (this_mode_running){
                if (xp_mode_running){
                    disconnect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(calXpMode()));
                    xp_mode_running = false;
                    p_motor6_->setXpStatus(false);
                }
                if(spd_sin_running){
                    disconnect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(calSpdCosMode()));
                    spd_sin_running = false;
                    p_motor6_->setsdsinStatus(false);
                }
                if(time_constant_running){
                    disconnect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(caltimeconstant()));
                    spd_sin_running = false;
                    p_motor6_->setsdsinStatus(false);
                }
                if(hx_mode_running){
                    disconnect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(setSpdOfAutoTestHXMode()));
                    hx_mode_running = false;
                }
                if (sm_mode_running){
                    disconnect(&m_timer_update_6,SIGNAL(timeout()),p_motor6_,SLOT(calSMModeSpd()));
                    sm_mode_running = false;
                }
                p_motor6_->setIsRunning(false);
                p_motor6_->setSetSpeed(0);
                p_motor6_->setXpCount(0);
                //p_motor1_->setAccelerate(0);
                //                m_timer_get_data_6.stop();
                this_mode_running = false;

                ui->pushButton_single_test_mode_6->setText("启动");
                ui->comboBox_motor_test_mode_6->setEnabled(true);
            }
            else{
                QMessageBox::warning(this,"警告","其它模式运行中。");
            }
        }
    }
}



void MainWindow::on_doubleSpinBox_J_1_valueChanged(double arg)
{
    p_motor1_->setJ(arg);

}
void MainWindow::on_doubleSpinBox_J_2_valueChanged(double arg)
{
    p_motor2_->setJ(arg);

}
void MainWindow::on_doubleSpinBox_J_3_valueChanged(double arg)
{
    p_motor3_->setJ(arg);

}
void MainWindow::on_doubleSpinBox_J_4_valueChanged(double arg)
{
    p_motor4_->setJ(arg);

}
void MainWindow::on_doubleSpinBox_J_5_valueChanged(double arg)
{
    p_motor5_->setJ(arg);

}
void MainWindow::on_doubleSpinBox_J_6_valueChanged(double arg)
{
    p_motor6_->setJ(arg);//如果这个错误，可以试试这个p_motor6_->setJ(ui->doubleSpinBox_J_6->text().toDouble());
}

void MainWindow::on_doubleSpinBox_test_interval_1_valueChanged(double arg)
{
    m_timer_get_data_.setInterval(arg*1000);
    //    p_motor1_->setCurrentInterval(arg);

}
void MainWindow::on_doubleSpinBox_test_interval_2_valueChanged(double arg)
{
    m_timer_get_data_2.setInterval(arg*1000);
    //    p_motor2_->setCurrentInterval(arg);
}
void MainWindow::on_doubleSpinBox_test_interval_3_valueChanged(double arg)
{
    m_timer_get_data_3.setInterval(arg*1000);
    //    p_motor3_->setCurrentInterval(arg);
}
void MainWindow::on_doubleSpinBox_test_interval_4_valueChanged(double arg)
{
    m_timer_get_data_4.setInterval(arg*1000);
    //    p_motor4_->setCurrentInterval(arg);
}
void MainWindow::on_doubleSpinBox_test_interval_5_valueChanged(double arg)
{
    m_timer_get_data_5.setInterval(arg*1000);
    //    p_motor5_->setCurrentInterval(arg);
}
void MainWindow::on_doubleSpinBox_test_interval_6_valueChanged(double arg)
{
    m_timer_get_data_6.setInterval(arg*1000);
    //    p_motor6_->setCurrentInterval(arg);
}
//为sql查询生成一个vector，为了方便信号槽数据的传递。
QVector<QString> MainWindow::makeSqlVector(Motor &motor)
{

    static QVector<QString> res;
    res.clear();
    res.append(motor.getChannel());//表名
    res.append(motor.getExpId());
    res.append(QString::number(motor.getVoltage(),'f',2));
    res.append(QString::number(motor.getCurrent(),'f',3));
    res.append(QString::number(motor.getSetSpeed(),'f',2));
    res.append(QString::number(motor.getSpeed(),'f',2));
    res.append(QString::number(motor.getSetTorque(),'f',2));
    res.append(QString::number(motor.getAveTorque(),'f',2));//实际力矩
    res.append(QString::number(motor.getHopeAveTorque(),'f',2));//期望力矩
    res.append(QString::number(motor.getWate(),'f',2));
    res.append(QString::number(motor.getAngularMomentum500(),'f',4));//
    res.append(QString::number(motor.getAngularMomentumConst500(),'f',4));//
    res.append(QString::number(motor.getAngularMomentumDynamic500(),'f',4));//
    res.append(QString::number(motor.getFlywheelMode()));
    res.append(QString::number(motor.getTimeConstant(),'f',3));
    res.append(QString::number(motor.getActCur(),'f',3));
    res.append(QString::number(motor.getSetCurrent(), 'f', 3));//电流模式的设置电流 zzy
    return res;//共16个数据
}
//更新电机1 总控制函数 调试时要多多注意这里
void MainWindow::updateMotor()
{
    //计算从数据采集卡的十个平均电压和电流
    if(m_sys_status_1_)
        p_motor1_->setAveVoltageAndCurrent();

    if (p_motor1_->getIsRunning()){
        emit getCanFrame(p_motor1_->getID());
        //if (ui->pushButton_system_power_1->text() == "关闭"){
        //斜坡模式不用发

        //        if (!p_motor1_->getXpStatus() && !p_motor1_->getsdsinStatus()&&!p_motor1_->gettimeconstant()&&!p_motor1_->getAutoTestSpdMode()&&
        //                !p_motor1_->getHXMode()&&!p_motor1_->getAutoTestTorMode()){//这个判断在多路测试的时候可能会有问题
        //          if(ui->pushButton_auto_test_noair_power_1->text() =="启动"){
        //            switch(ui->comboBox_motor_test_mode_1->currentIndex()){
        //            case 0://速度模式
        //                p_motor1_->setFlywheelMode(FLYWHEEL_MODE_SPD);
        //                p_motor1_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_1->text().toDouble());
        //                break;
        //            case 3://力矩模式
        //                p_motor1_->setFlywheelMode(FLYWHEEL_MODE_TOR);
        //                p_motor1_->setSetTorque(ui->doubleSpinBox_motor_test_spd_1->text().toDouble());
        //                break;
        //            default:
        //                break;
        //            }
        //        }
        //        p_motor1_->setCurrentInterval(ui->doubleSpinBox_test_interval_1->text().toDouble());  //new ADD
        //更新显示界面
        updateMotor1Display();
        refreshCustomPlotData1();
        //非真空速度测试
        if (p_motor1_->getAirMode()){
            emit sendCurrentSpdForAutoTest(p_motor1_->getSpeed());
        }
        //一键测试，速度模式
        else if (p_motor1_->getAutoTestSpdMode()){
            emit refreshAutoTestSpd();
        }
        else if(p_motor1_->getAutoTestTorMode()){
            emit refreshAutoTestTor();
        }
        else if(p_motor1_->getAutoTestCurMode()){
            emit refreshAutotestCur();
        }
        
        /*
        //这是非真空一键测试的
        else if (p_motor1_->getNoAirMode()){
            emit sendCurrentSpdForAutoTest(p_motor1_->getSpeed());
        }*/
        //更新数据库,为了防止插入太快，每隔0.5S插入一次

        static QTime time(QTime::currentTime());
        // calculate two new data points:
        double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
        static double lastPointKey = 0;
        if (key-lastPointKey >0.5){//0.5
            emit sendToSqlDB(ui->lineEdit_product_no_1->text(),ui->lineEdit_exp_fw_id_1->text(),
                             ui->lineEdit_exp_aim_1->text(),ui->lineEdit_exp_time_1->text(),
                             ui->lineEdit_exp_environment_1->text(),ui->lineEdit_exp_address_1->text(),makeSqlVector(*p_motor1_));
            lastPointKey = key;
            if (key > 43200){
                //12h重置一次！
                time.restart();
                lastPointKey = 0;
            }
        }
    }
    else{
        if ( m_sys_status_1_ && !p_motor1_->getIsRunning()){
            //p_motor1_->setSetSpeed(0);
            emit getCanFrame(p_motor1_->getID());
            updateMotor1Display();

            refreshCustomPlotData1();
            static QTime time(QTime::currentTime());
            // calculate two new data points:
            double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
            static double lastPointKey = 0;
            if (key-lastPointKey > 0.5){
                emit sendToSqlDB(ui->lineEdit_product_no_1->text(),ui->lineEdit_exp_fw_id_1->text(),
                                 ui->lineEdit_exp_aim_1->text(),ui->lineEdit_exp_time_1->text(),
                                 ui->lineEdit_exp_environment_1->text(),ui->lineEdit_exp_address_1->text(),makeSqlVector(*p_motor1_));
                lastPointKey = key;
                if (key > 43200){
                    //12h重置一次！
                    time.restart();
                    lastPointKey = 0;
                }
            }
        }
    }

    //...etc motor
}
void MainWindow::updateMotor2()
{
    if(m_sys_status_2_)
        p_motor2_->setAveVoltageAndCurrent();
    if (p_motor2_->getIsRunning()){
        //斜坡模式不用发
        emit getCanFrame(p_motor2_->getID());
        //        if (!p_motor2_->getXpStatus() && !p_motor2_->getsdsinStatus()&&!p_motor2_->gettimeconstant()&&!p_motor2_->getAutoTestSpdMode()&&
        //                !p_motor2_->getHXMode()&&!p_motor2_->getAutoTestTorMode()){
        //          if(ui->pushButton_auto_test_noair_power_2->text() =="启动"){
        //            switch(ui->comboBox_motor_test_mode_2->currentIndex()){
        //            case 0://速度模式
        //                p_motor2_->setFlywheelMode(FLYWHEEL_MODE_SPD);
        //                p_motor2_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_2->text().toDouble());
        //                break;
        //            case 3://力矩模式
        //                p_motor2_->setFlywheelMode(FLYWHEEL_MODE_TOR);
        //                p_motor2_->setSetTorque(ui->doubleSpinBox_motor_test_spd_2->text().toDouble());
        //                break;
        //            default:
        //                break;
        //            }
        //        }
        //计算从数据采集卡的十个平均电压和电流
        //        if(m_sys_status_2_)
        //        p_motor2_->setAveVoltageAndCurrent();
        //更新显示界面
        updateMotor2Display();
        refreshCustomPlotData2();
        //非真空速度测试
        if (p_motor2_->getAirMode()){
            emit sendCurrentSpdForAutoTest2(p_motor2_->getSpeed());
        }
        //一键测试，速度模式
        else if (p_motor2_->getAutoTestSpdMode()){
            emit refreshAutoTestSpd2();
        }
        else if(p_motor2_->getAutoTestTorMode()){
            emit refreshAutoTestTor2();
        }
        else if(p_motor2_->getAutoTestCurMode()){
            emit refreshAutotestCur2();
        }
        /*
        //这是非真空一键测试的
        else if (p_motor1_->getNoAirMode()){
            emit sendCurrentSpdForAutoTest(p_motor1_->getSpeed());
        }*/
        //更新数据库,为了防止插入太快，每隔0.5S插入一次 这个时间是独立的

        static QTime time(QTime::currentTime());
        // calculate two new data points:
        double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
        static double lastPointKey = 0;
        if (key-lastPointKey > 0.5){//把0.5改成0.1后是一秒插2个数据
            emit sendToSqlDB2(ui->lineEdit_product_no_2->text(),ui->lineEdit_exp_fw_id_2->text(),
                              ui->lineEdit_exp_aim_2->text(),ui->lineEdit_exp_time_2->text(),
                              ui->lineEdit_exp_environment_2->text(),ui->lineEdit_exp_address_2->text(),makeSqlVector(*p_motor2_));
            lastPointKey = key;
            if (key > 43200){
                //12h重置一次！
                time.restart();
                lastPointKey = 0;
            }
        }
    }
    else{
        if(m_sys_status_2_ && !p_motor2_->getIsRunning())
        {
            //           p_motor2_->setSetSpeed(0);
            emit getCanFrame(p_motor2_->getID());
            updateMotor2Display();
            refreshCustomPlotData2();
            static QTime time(QTime::currentTime());
            // calculate two new data points:
            double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
            static double lastPointKey = 0;
            if (key-lastPointKey > 0.5){//把0.5改成0.1后是一秒插2个数据
                emit sendToSqlDB2(ui->lineEdit_product_no_2->text(),ui->lineEdit_exp_fw_id_2->text(),
                                  ui->lineEdit_exp_aim_2->text(),ui->lineEdit_exp_time_2->text(),
                                  ui->lineEdit_exp_environment_2->text(),ui->lineEdit_exp_address_2->text(),makeSqlVector(*p_motor2_));
                lastPointKey = key;
                if (key > 43200){
                    //12h重置一次！
                    time.restart();
                    lastPointKey = 0;
                }
            }
        }
    }

    //...etc motor
}
void MainWindow::updateMotor3()
{
    if(m_sys_status_3_)
        p_motor3_->setAveVoltageAndCurrent();
    if (p_motor3_->getIsRunning()){
        //斜坡模式不用发
        emit getCanFrame(p_motor3_->getID());
        //        if (!p_motor3_->getXpStatus() && !p_motor3_->getsdsinStatus()&&!p_motor3_->gettimeconstant()&&!p_motor3_->getAutoTestSpdMode()&&
        //                !p_motor3_->getHXMode()&&!p_motor3_->getAutoTestTorMode()){
        //          if(ui->pushButton_auto_test_noair_power_3->text() =="启动"){
        //            switch(ui->comboBox_motor_test_mode_3->currentIndex()){
        //            case 0://速度模式
        //                p_motor3_->setFlywheelMode(FLYWHEEL_MODE_SPD);
        //                p_motor3_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_3->text().toDouble());
        //                break;
        //            case 3://力矩模式
        //                p_motor3_->setFlywheelMode(FLYWHEEL_MODE_TOR);
        //                p_motor3_->setSetTorque(ui->doubleSpinBox_motor_test_spd_3->text().toDouble());
        //                break;
        //            default:
        //                break;
        //            }
        //        }
        //计算从数据采集卡的十个平均电压和电流
        //        if(m_sys_status_3_)
        //        p_motor3_->setAveVoltageAndCurrent();
        //更新显示界面
        updateMotor3Display();
        refreshCustomPlotData3();
        //非真空速度测试
        if (p_motor3_->getAirMode()){
            emit sendCurrentSpdForAutoTest3(p_motor3_->getSpeed());
        }
        //一键测试，速度模式
        else if (p_motor3_->getAutoTestSpdMode()){
            emit refreshAutoTestSpd3();
        }
        else if(p_motor3_->getAutoTestTorMode()){
            emit refreshAutoTestTor3();
        }
        else if(p_motor3_->getAutoTestCurMode()){
            emit refreshAutotestCur3();
        }
        /*
        //这是非真空一键测试的
        else if (p_motor1_->getNoAirMode()){
            emit sendCurrentSpdForAutoTest(p_motor1_->getSpeed());
        }*/
        //更新数据库,为了防止插入太快，每隔0.5S插入一次

        static QTime time(QTime::currentTime());
        // calculate two new data points:
        double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
        static double lastPointKey = 0;
        if (key-lastPointKey > 0.5){
            emit sendToSqlDB3(ui->lineEdit_product_no_3->text(),ui->lineEdit_exp_fw_id_3->text(),
                              ui->lineEdit_exp_aim_3->text(),ui->lineEdit_exp_time_3->text(),
                              ui->lineEdit_exp_environment_3->text(),ui->lineEdit_exp_address_3->text(),makeSqlVector(*p_motor3_));
            lastPointKey = key;
            if (key > 43200){
                //12h重置一次！
                time.restart();
                lastPointKey = 0;
            }
        }
    }
    else{
        if(m_sys_status_3_ && !p_motor3_->getIsRunning())
        {
            //            p_motor3_->setSetSpeed(0);
            emit getCanFrame(p_motor3_->getID());
            updateMotor3Display();
            refreshCustomPlotData3();
            static QTime time(QTime::currentTime());
            // calculate two new data points:
            double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
            static double lastPointKey = 0;
            if (key-lastPointKey > 0.5){
                emit sendToSqlDB3(ui->lineEdit_product_no_3->text(),ui->lineEdit_exp_fw_id_3->text(),
                                  ui->lineEdit_exp_aim_3->text(),ui->lineEdit_exp_time_3->text(),
                                  ui->lineEdit_exp_environment_3->text(),ui->lineEdit_exp_address_3->text(),makeSqlVector(*p_motor3_));
                lastPointKey = key;
                if (key > 43200){
                    //12h重置一次！
                    time.restart();
                    lastPointKey = 0;
                }
            }

        }
    }
    //...etc motor
}
void MainWindow::updateMotor4()
{
    if(m_sys_status_4_)
        p_motor4_->setAveVoltageAndCurrent();
    if (p_motor4_->getIsRunning()){
        //斜坡模式不用发
        emit getCanFrame(p_motor4_->getID());
        //        if (!p_motor4_->getXpStatus() && !p_motor4_->getsdsinStatus()&&!p_motor4_->gettimeconstant()&&!p_motor4_->getAutoTestSpdMode()&&
        //                !p_motor4_->getHXMode()&&!p_motor4_->getAutoTestTorMode()){//多路测试这个判断有问题
        //          if(ui->pushButton_auto_test_noair_power_4->text() =="启动"){
        //            switch(ui->comboBox_motor_test_mode_4->currentIndex()){
        //            case 0://速度模式
        //                p_motor4_->setFlywheelMode(FLYWHEEL_MODE_SPD);
        //                p_motor4_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_4->text().toDouble());
        //                break;
        //            case 3://力矩模式
        //                p_motor4_->setFlywheelMode(FLYWHEEL_MODE_TOR);
        //                p_motor4_->setSetTorque(ui->doubleSpinBox_motor_test_spd_4->text().toDouble());
        //                break;
        //            default:
        //                break;
        //            }
        //        }
        //计算从数据采集卡的十个平均电压和电流
        //        if(m_sys_status_4_)
        //        p_motor4_->setAveVoltageAndCurrent();
        //更新显示界面
        updateMotor4Display();
        refreshCustomPlotData4();
        //非真空速度测试
        if (p_motor4_->getAirMode()){
            emit sendCurrentSpdForAutoTest4(p_motor4_->getSpeed());
        }
        //一键测试，速度模式
        else if (p_motor4_->getAutoTestSpdMode()){
            emit refreshAutoTestSpd4();
        }
        else if(p_motor4_->getAutoTestTorMode()){
            emit refreshAutoTestTor4();
        }
        else if(p_motor4_->getAutoTestCurMode()){
            emit refreshAutotestCur4();
        }
        /*
        //这是非真空一键测试的
        else if (p_motor1_->getNoAirMode()){
            emit sendCurrentSpdForAutoTest(p_motor1_->getSpeed());
        }*/
        //更新数据库,为了防止插入太快，每隔0.5S插入一次

        static QTime time(QTime::currentTime());
        // calculate two new data points:
        double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
        static double lastPointKey = 0;
        if (key-lastPointKey > 0.5){
            emit sendToSqlDB4(ui->lineEdit_product_no_4->text(),ui->lineEdit_exp_fw_id_4->text(),
                              ui->lineEdit_exp_aim_4->text(),ui->lineEdit_exp_time_4->text(),
                              ui->lineEdit_exp_environment_4->text(),ui->lineEdit_exp_address_4->text(),makeSqlVector(*p_motor4_));
            lastPointKey = key;
            if (key > 43200){
                //12h重置一次！
                time.restart();
                lastPointKey = 0;
            }
        }
    }
    else{
        if(m_sys_status_4_ && !p_motor4_->getIsRunning())
        {
            //            p_motor4_->setSetSpeed(0);
            emit getCanFrame(p_motor4_->getID());
            updateMotor4Display();
            refreshCustomPlotData4();
            static QTime time(QTime::currentTime());
            double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
            static double lastPointKey = 0;
            if (key-lastPointKey > 0.5){
                emit sendToSqlDB4(ui->lineEdit_product_no_4->text(),ui->lineEdit_exp_fw_id_4->text(),
                                  ui->lineEdit_exp_aim_4->text(),ui->lineEdit_exp_time_4->text(),
                                  ui->lineEdit_exp_environment_4->text(),ui->lineEdit_exp_address_4->text(),makeSqlVector(*p_motor4_));
                lastPointKey = key;
                if (key > 43200){
                    //12h重置一次！
                    time.restart();
                    lastPointKey = 0;
                }
            }
        }
    }

    //...etc motor
}
void MainWindow::updateMotor5()
{
    if(m_sys_status_5_)
        p_motor5_->setAveVoltageAndCurrent();
    if (p_motor5_->getIsRunning()){
        //斜坡模式不用发
        emit getCanFrame(p_motor5_->getID());
        //        if (!p_motor5_->getXpStatus() && !p_motor5_->getsdsinStatus()&&!p_motor5_->gettimeconstant()&&!p_motor5_->getAutoTestSpdMode()&&
        //                !p_motor5_->getHXMode()&&!p_motor5_->getAutoTestTorMode()){
        //          if(ui->pushButton_auto_test_noair_power_5->text() =="启动"){
        //            switch(ui->comboBox_motor_test_mode_5->currentIndex()){
        //            case 0://速度模式
        //                p_motor5_->setFlywheelMode(FLYWHEEL_MODE_SPD);
        //                p_motor5_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_5->text().toDouble());
        //                break;
        //            case 3://力矩模式
        //                p_motor5_->setFlywheelMode(FLYWHEEL_MODE_TOR);
        //                p_motor5_->setSetTorque(ui->doubleSpinBox_motor_test_spd_5->text().toDouble());
        //                break;
        //            default:
        //                break;
        //            }
        //        }
        //计算从数据采集卡的十个平均电压和电流
        //         if(m_sys_status_5_)
        //        p_motor5_->setAveVoltageAndCurrent();
        //更新显示界面
        updateMotor5Display();
        refreshCustomPlotData5();
        //非真空速度测试
        if (p_motor5_->getAirMode()){
            emit sendCurrentSpdForAutoTest5(p_motor5_->getSpeed());
        }
        //一键测试，速度模式
        else if (p_motor5_->getAutoTestSpdMode()){
            emit refreshAutoTestSpd5();
        }
        else if(p_motor5_->getAutoTestTorMode()){
            //p_motor5_->setSpeed(1);
            emit refreshAutoTestTor5();
        }
        else if(p_motor5_->getAutoTestCurMode()){
            emit refreshAutotestCur5();
        }
        /*
        //这是非真空一键测试的
        else if (p_motor1_->getNoAirMode()){
            emit sendCurrentSpdForAutoTest(p_motor1_->getSpeed());
        }*/
        //更新数据库,为了防止插入太快，每隔0.5S插入一次

        static QTime time(QTime::currentTime());
        // calculate two new data points:
        double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
        static double lastPointKey = 0;
        if (key-lastPointKey > 0.5){
            emit sendToSqlDB5(ui->lineEdit_product_no_5->text(),ui->lineEdit_exp_fw_id_5->text(),
                              ui->lineEdit_exp_aim_5->text(),ui->lineEdit_exp_time_5->text(),
                              ui->lineEdit_exp_environment_5->text(),ui->lineEdit_exp_address_5->text(),makeSqlVector(*p_motor5_));
            lastPointKey = key;
            if (key > 43200){
                //12h重置一次！
                time.restart();
                lastPointKey = 0;
            }
        }
    }
    else{
        if(m_sys_status_5_ && !p_motor5_->getIsRunning())
        {
            //            p_motor5_->setSetSpeed(0);
            emit getCanFrame(p_motor5_->getID());
            updateMotor5Display();
            refreshCustomPlotData5();
            static QTime time(QTime::currentTime());
            double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
            static double lastPointKey = 0;
            if (key-lastPointKey > 0.5){
                emit sendToSqlDB5(ui->lineEdit_product_no_5->text(),ui->lineEdit_exp_fw_id_5->text(),
                                  ui->lineEdit_exp_aim_5->text(),ui->lineEdit_exp_time_5->text(),
                                  ui->lineEdit_exp_environment_5->text(),ui->lineEdit_exp_address_5->text(),makeSqlVector(*p_motor5_));
                lastPointKey = key;
                if (key > 43200){
                    //12h重置一次！
                    time.restart();
                    lastPointKey = 0;
                }
            }
        }
    }

    //...etc motor
}


void MainWindow::updateMotor6()
{
    if(m_sys_status_6_)
        p_motor6_->setAveVoltageAndCurrent();
    if (p_motor6_->getIsRunning()){
        //斜坡模式不用发
        emit getCanFrame(p_motor6_->getID());
        //        if (!p_motor6_->getXpStatus() && !p_motor6_->getsdsinStatus()&&!p_motor6_->gettimeconstant()&&!p_motor6_->getAutoTestSpdMode()&&
        //                !p_motor6_->getHXMode()&&!p_motor6_->getAutoTestTorMode()){
        //          if(ui->pushButton_auto_test_noair_power_6->text() =="启动"){
        //            switch(ui->comboBox_motor_test_mode_6->currentIndex()){
        //            case 0://速度模式
        //                p_motor6_->setFlywheelMode(FLYWHEEL_MODE_SPD);
        //                p_motor6_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_6->text().toDouble());
        //                break;
        //            case 3://力矩模式
        //                p_motor6_->setFlywheelMode(FLYWHEEL_MODE_TOR);
        //                p_motor6_->setSetTorque(ui->doubleSpinBox_motor_test_spd_6->text().toDouble());
        //                break;
        //            default:
        //                break;
        //            }
        //        }
        //计算从数据采集卡的十个平均电压和电流
        //         if(m_sys_status_6_)
        //        p_motor6_->setAveVoltageAndCurrent();
        //更新显示界面
        updateMotor6Display();
        refreshCustomPlotData6();
        //非真空速度测试
        if (p_motor6_->getAirMode()){
            emit sendCurrentSpdForAutoTest6(p_motor6_->getSpeed());
        }
        //一键测试，速度模式
        else if (p_motor6_->getAutoTestSpdMode()){
            emit refreshAutoTestSpd6();
        }
        else if(p_motor6_->getAutoTestTorMode()){
            emit refreshAutoTestTor6();
        }
        else if(p_motor6_->getAutoTestCurMode()){
            emit refreshAutotestCur6();
        }
        /*
        //这是非真空一键测试的
        else if (p_motor1_->getNoAirMode()){
            emit sendCurrentSpdForAutoTest(p_motor1_->getSpeed());
        }*/
        //更新数据库,为了防止插入太快，每隔0.5S插入一次

        static QTime time(QTime::currentTime());
        // calculate two new data points:
        double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
        static double lastPointKey = 0;
        if (key-lastPointKey > 0.5){
            emit sendToSqlDB5(ui->lineEdit_product_no_6->text(),ui->lineEdit_exp_fw_id_6->text(),
                              ui->lineEdit_exp_aim_6->text(),ui->lineEdit_exp_time_6->text(),
                              ui->lineEdit_exp_environment_6->text(),ui->lineEdit_exp_address_6->text(),makeSqlVector(*p_motor6_));
            lastPointKey = key;
            if (key > 43200){
                //12h重置一次！
                time.restart();
                lastPointKey = 0;
            }
        }
    }
    else{
        if(m_sys_status_6_ && !p_motor6_->getIsRunning())
        {
            //            p_motor6_->setSetSpeed(0);
            emit getCanFrame(p_motor6_->getID());
            updateMotor6Display();
            refreshCustomPlotData6();
            static QTime time(QTime::currentTime());
            double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
            static double lastPointKey = 0;
            if (key-lastPointKey > 0.5){
                emit sendToSqlDB6(ui->lineEdit_product_no_6->text(),ui->lineEdit_exp_fw_id_6->text(),
                                  ui->lineEdit_exp_aim_6->text(),ui->lineEdit_exp_time_6->text(),
                                  ui->lineEdit_exp_environment_6->text(),ui->lineEdit_exp_address_6->text(),makeSqlVector(*p_motor6_));
                lastPointKey = key;
                if (key > 43200){
                    //12h重置一次！
                    time.restart();
                    lastPointKey = 0;
                }
            }
        }
    }

    //...etc motor
}

//更新电机1---数据显示
void MainWindow::updateMotor1Display()
{
    //update lineedit
    //ui->lineEdit_sys_vol_1->setText(QString::number(p_motor1_->getVoltage(),'f',3));//'f'是计数的方法例如'g'表示的是科学计数法，表示的是精度
    ui->lineEdit_sys_vol_1->setText(QString::number(p_motor1_->getAveVoltage(),'f',2));//从数据采集卡中读到的电压
    ui->lineEdit_sys_cur_1->setText(QString::number(p_motor1_->getAveCurrent(),'f',3));//从数据采集卡中读到的电流，在mainwindow.cpp设置他们的值

    if(p_motor1_->getAutoTestTorMode() || ui->comboBox_motor_test_mode_1->currentIndex() == 3)
    {
        ui->label_setval_right_1->setText("设置力矩");
        ui->lineEdit_motor_set_spd_1->setText(QString::number(p_motor1_->getSetTorque(),'f',2));
        ui->label_52_1->setText("mNm");
    }
    else if(p_motor1_->getAutoTestCurMode() || ui->comboBox_motor_test_mode_1->currentIndex() == 7)
    {
        ui->label_setval_right_1->setText("设置电流");
        ui->lineEdit_motor_set_spd_1->setText(QString::number(p_motor1_->getSetCurrent(),'f',2));
        ui->label_52_1->setText("mA");

    }
    else {
        ui->label_setval_right_1->setText("设置速度");
        ui->lineEdit_motor_set_spd_1->setText(QString::number(p_motor1_->getSetSpeed(),'f',2));
        ui->label_52_1->setText("rpm");
    }


    /*
    if (ui->checkBox_hs_mode_1->isChecked())
        ui->lineEdit_motor_set_spd_1->setText(QString::number(p_motor1_->getSetSpeed(),'f',2));
    if (ui->checkBox_lj_mode_1->isChecked()){
        ui->label_setval_right_1->setText("设置力矩");
        ui->lineEdit_motor_set_spd_1->setText(QString::number(p_motor1_->getSetTorque(),'f',2));
        ui->label_52_1->setText("mNm");//FW2-6
        }*/
    ui->lineEdit_motor_act_spd_1->setText(QString::number(p_motor1_->getSpeed(),'f',2));
    ui->lineEdit_motor_cur_1->setText(QString::number(p_motor1_->getCurrent(),'f',3));
    //ui->lineEdit_motor_act_tor_1->setText(QString::number(p_motor1_->getReactionMoment_()));
    ui->lineEdit_motor_act_tor_1->setText(QString::number(p_motor1_->getAveTorque(),'f',2));
    ui->lineEdit_motor_hope_tor_1->setText(QString::number(p_motor1_->getHopeAveTorque(),'f',2));
    ui->lineEdit_motor_jdl_1->setText(QString::number(p_motor1_->getAngularMomentum500(),'f',4));
    ui->lineEdit_motor_jdl_czpc_1->setText(QString::number(p_motor1_->getAngularMomentumConst500(),'f',4));
    ui->lineEdit_motor_jdl_dtpc_1->setText(QString::number(p_motor1_->getAngularMomentumDynamic500(),'f',4));
    ui->lineEdit_motor_wate_1->setText(QString::number(p_motor1_->getWate(),'f',2));
    ui->lineEdit_motor_wate_overview_1->setText(QString::number(p_motor1_->getWate(),'f',2));
    ui->lineEdit_time_constant_1->setText(QString::number(p_motor1_->getTimeConstant(),'f',3));
}
void MainWindow::updateMotor2Display()
{
    //update lineedit
    ui->lineEdit_sys_vol_2->setText(QString::number(p_motor2_->getAveVoltage(),'f',2));//从数据采集卡中读到的电压
    ui->lineEdit_sys_cur_2->setText(QString::number(p_motor2_->getAveCurrent(),'f',3));//从数据采集卡中读到的电流，在mainwindow.cpp设置他们的值

    if(p_motor2_->getAutoTestTorMode() || ui->comboBox_motor_test_mode_2->currentIndex() == 3)
    {
        ui->label_setval_right_2->setText("设置力矩");
        ui->lineEdit_motor_set_spd_2->setText(QString::number(p_motor2_->getSetTorque(),'f',3));
        ui->label_52_2->setText("mNm");
    }
    else if(p_motor2_->getAutoTestCurMode() || ui->comboBox_motor_test_mode_2->currentIndex() == 7)
    {
        ui->label_setval_right_2->setText("设置电流");
        ui->lineEdit_motor_set_spd_2->setText(QString::number(p_motor2_->getSetCurrent(),'f',2));
        ui->label_52_2->setText("mA");

    }
    else{
        ui->label_setval_right_2->setText("设置速度");
        ui->lineEdit_motor_set_spd_2->setText(QString::number(p_motor2_->getSetSpeed(),'f',3));
        ui->label_52_2->setText("rpm");
    }

    ui->lineEdit_motor_act_spd_2->setText(QString::number(p_motor2_->getSpeed(),'f',2));
    ui->lineEdit_motor_cur_2->setText(QString::number(p_motor2_->getCurrent(),'f',3));
    //ui->lineEdit_motor_act_tor_1->setText(QString::number(p_motor1_->getReactionMoment_()));
    ui->lineEdit_motor_act_tor_2->setText(QString::number(p_motor2_->getAveTorque(),'f',2));
    ui->lineEdit_motor_hope_tor_2->setText(QString::number(p_motor2_->getHopeAveTorque(),'f',2));
    ui->lineEdit_motor_jdl_2->setText(QString::number(p_motor2_->getAngularMomentum500(),'f',4));
    ui->lineEdit_motor_jdl_czpc_2->setText(QString::number(p_motor2_->getAngularMomentumConst500(),'f',4));
    ui->lineEdit_motor_jdl_dtpc_2->setText(QString::number(p_motor2_->getAngularMomentumDynamic500(),'f',4));
    ui->lineEdit_motor_wate_2->setText(QString::number(p_motor2_->getWate(),'f',2));
    ui->lineEdit_motor_wate_overview_2->setText(QString::number(p_motor2_->getWate(),'f',2));
    ui->lineEdit_time_constant_2->setText(QString::number(p_motor2_->getTimeConstant(),'f',3));
}
void MainWindow::updateMotor3Display()
{
    //update lineedit
    ui->lineEdit_sys_vol_3->setText(QString::number(p_motor3_->getAveVoltage(),'f',2));//从数据采集卡中读到的电压
    ui->lineEdit_sys_cur_3->setText(QString::number(p_motor3_->getAveCurrent(),'f',3));//从数据采集卡中读到的电流，在mainwindow.cpp设置他们的值
    //ui->lineEdit_motor_set_spd_1->setText(QString::number(p_motor1_->getSetSpeed()));

    if(p_motor3_->getAutoTestTorMode() || ui->comboBox_motor_test_mode_3->currentIndex() == 3)
    {
        ui->label_setval_right_3->setText("设置力矩");
        ui->lineEdit_motor_set_spd_3->setText(QString::number(p_motor3_->getSetTorque(),'f',3));
        ui->label_52_3->setText("mNm");
    }
    else if(p_motor3_->getAutoTestCurMode() || ui->comboBox_motor_test_mode_3->currentIndex() == 7)
    {
        ui->label_setval_right_3->setText("设置电流");
        ui->lineEdit_motor_set_spd_3->setText(QString::number(p_motor3_->getSetCurrent(),'f',2));
        ui->label_52_3->setText("mA");

    }
    else{
        ui->label_setval_right_3->setText("设置速度");
        ui->lineEdit_motor_set_spd_3->setText(QString::number(p_motor3_->getSetSpeed(),'f',3));
        ui->label_52_3->setText("rpm");
    }
    ui->lineEdit_motor_act_spd_3->setText(QString::number(p_motor3_->getSpeed(),'f',2));
    ui->lineEdit_motor_cur_3->setText(QString::number(p_motor3_->getCurrent(),'f',3));
    //ui->lineEdit_motor_act_tor_1->setText(QString::number(p_motor1_->getReactionMoment_()));
    ui->lineEdit_motor_act_tor_3->setText(QString::number(p_motor3_->getAveTorque(),'f',2));
    ui->lineEdit_motor_hope_tor_3->setText(QString::number(p_motor3_->getHopeAveTorque(),'f',2));
    ui->lineEdit_motor_jdl_3->setText(QString::number(p_motor3_->getAngularMomentum500(),'f',4));
    ui->lineEdit_motor_jdl_czpc_3->setText(QString::number(p_motor3_->getAngularMomentumConst500(),'f',4));
    ui->lineEdit_motor_jdl_dtpc_3->setText(QString::number(p_motor3_->getAngularMomentumDynamic500(),'f',4));
    ui->lineEdit_motor_wate_3->setText(QString::number(p_motor3_->getWate(),'f',2));
    ui->lineEdit_motor_wate_overview_3->setText(QString::number(p_motor3_->getWate(),'f',2));
    ui->lineEdit_time_constant_3->setText(QString::number(p_motor3_->getTimeConstant(),'f',3));
}
void MainWindow::updateMotor4Display()
{
    //update lineedit
    ui->lineEdit_sys_vol_4->setText(QString::number(p_motor4_->getAveVoltage(),'f',2));//从数据采集卡中读到的电压
    ui->lineEdit_sys_cur_4->setText(QString::number(p_motor4_->getAveCurrent(),'f',3));//从数据采集卡中读到的电流，在mainwindow.cpp设置他们的值
    //ui->lineEdit_motor_set_spd_1->setText(QString::number(p_motor1_->getSetSpeed()));

    if(p_motor4_->getAutoTestTorMode() || ui->comboBox_motor_test_mode_4->currentIndex() == 3)
    {
        ui->label_setval_right_4->setText("设置力矩");
        ui->lineEdit_motor_set_spd_4->setText(QString::number(p_motor4_->getSetTorque(),'f',3));
        ui->label_52_4->setText("mNm");
    }
    else if(p_motor4_->getAutoTestCurMode() || ui->comboBox_motor_test_mode_4->currentIndex() == 7)
    {
        ui->label_setval_right_4->setText("设置电流");
        ui->lineEdit_motor_set_spd_4->setText(QString::number(p_motor4_->getSetCurrent(),'f',2));
        ui->label_52_4->setText("mA");

    }
    else{
        ui->label_setval_right_4->setText("设置速度");
        ui->lineEdit_motor_set_spd_4->setText(QString::number(p_motor4_->getSetSpeed(),'f',3));
        ui->label_52_4->setText("rpm");
    }
    ui->lineEdit_motor_act_spd_4->setText(QString::number(p_motor4_->getSpeed(),'f',2));
    ui->lineEdit_motor_cur_4->setText(QString::number(p_motor4_->getCurrent(),'f',3));
    //ui->lineEdit_motor_act_tor_1->setText(QString::number(p_motor1_->getReactionMoment_()));
    ui->lineEdit_motor_act_tor_4->setText(QString::number(p_motor4_->getAveTorque(),'f',2));
    ui->lineEdit_motor_hope_tor_4->setText(QString::number(p_motor4_->getHopeAveTorque(),'f',2));
    ui->lineEdit_motor_jdl_4->setText(QString::number(p_motor4_->getAngularMomentum500(),'f',4));
    ui->lineEdit_motor_jdl_czpc_4->setText(QString::number(p_motor4_->getAngularMomentumConst500(),'f',4));
    ui->lineEdit_motor_jdl_dtpc_4->setText(QString::number(p_motor4_->getAngularMomentumDynamic500(),'f',4));
    ui->lineEdit_motor_wate_4->setText(QString::number(p_motor4_->getWate(),'f',2));
    ui->lineEdit_motor_wate_overview_4->setText(QString::number(p_motor4_->getWate(),'f',2));
    ui->lineEdit_time_constant_4->setText(QString::number(p_motor4_->getTimeConstant(),'f',3));
}
void MainWindow::updateMotor5Display()
{
    //update lineedit
    ui->lineEdit_sys_vol_5->setText(QString::number(p_motor5_->getAveVoltage(),'f',2));//从数据采集卡中读到的电压
    ui->lineEdit_sys_cur_5->setText(QString::number(p_motor5_->getAveCurrent(),'f',3));//从数据采集卡中读到的电流，在mainwindow.cpp设置他们的值
    if(p_motor5_->getAutoTestTorMode() || ui->comboBox_motor_test_mode_5->currentIndex() == 3)
    {
        ui->label_setval_right_5->setText("设置力矩");
        ui->lineEdit_motor_set_spd_5->setText(QString::number(p_motor5_->getSetTorque(),'f',3));
        ui->label_52_5->setText("mNm");
    }
    else if(p_motor5_->getAutoTestCurMode() || ui->comboBox_motor_test_mode_5->currentIndex() == 7)
    {
        ui->label_setval_right_5->setText("设置电流");
        ui->lineEdit_motor_set_spd_5->setText(QString::number(p_motor5_->getSetCurrent(),'f',2));
        ui->label_52_5->setText("mA");

    }
    else{
        ui->label_setval_right_5->setText("设置速度");
        ui->lineEdit_motor_set_spd_5->setText(QString::number(p_motor5_->getSetSpeed(),'f',3));
        ui->label_52_5->setText("rpm");
    }
    ui->lineEdit_motor_act_spd_5->setText(QString::number(p_motor5_->getSpeed(),'f',2));
    ui->lineEdit_motor_cur_5->setText(QString::number(p_motor5_->getCurrent(),'f',3));
    //ui->lineEdit_motor_act_tor_1->setText(QString::number(p_motor1_->getReactionMoment_()));
    ui->lineEdit_motor_act_tor_5->setText(QString::number(p_motor5_->getAveTorque(),'f',2));
    ui->lineEdit_motor_hope_tor_5->setText(QString::number(p_motor5_->getHopeAveTorque(),'f',2));
    ui->lineEdit_motor_jdl_5->setText(QString::number(p_motor5_->getAngularMomentum500(),'f',4));
    ui->lineEdit_motor_jdl_czpc_5->setText(QString::number(p_motor5_->getAngularMomentumConst500(),'f',4));
    ui->lineEdit_motor_jdl_dtpc_5->setText(QString::number(p_motor5_->getAngularMomentumDynamic500(),'f',4));
    ui->lineEdit_motor_wate_5->setText(QString::number(p_motor5_->getWate(),'f',2));
    ui->lineEdit_motor_wate_overview_5->setText(QString::number(p_motor5_->getWate(),'f',2));
    ui->lineEdit_time_constant_5->setText(QString::number(p_motor5_->getTimeConstant(),'f',3));
}
void MainWindow::updateMotor6Display()
{
    //update lineedit
    ui->lineEdit_sys_vol_6->setText(QString::number(p_motor6_->getAveVoltage(),'f',2));//从数据采集卡中读到的电压
    ui->lineEdit_sys_cur_6->setText(QString::number(p_motor6_->getAveCurrent(),'f',3));//从数据采集卡中读到的电流，在mainwindow.cpp设置他们的值
    if(p_motor6_->getAutoTestTorMode() || ui->comboBox_motor_test_mode_6->currentIndex() == 3)
    {
        ui->label_setval_right_6->setText("设置力矩");
        ui->lineEdit_motor_set_spd_6->setText(QString::number(p_motor6_->getSetTorque(),'f',3));
        ui->label_52_6->setText("mNm");
    }
    else if(p_motor6_->getAutoTestCurMode() || ui->comboBox_motor_test_mode_6->currentIndex() == 7)
    {
        ui->label_setval_right_6->setText("设置电流");
        ui->lineEdit_motor_set_spd_6->setText(QString::number(p_motor6_->getSetCurrent(),'f',2));
        ui->label_52_6->setText("mA");

    }
    else{
        ui->label_setval_right_6->setText("设置速度");
        ui->lineEdit_motor_set_spd_6->setText(QString::number(p_motor6_->getSetSpeed(),'f',3));
        ui->label_52_6->setText("rpm");
    }
    ui->lineEdit_motor_act_spd_6->setText(QString::number(p_motor6_->getSpeed(),'f',2));
    ui->lineEdit_motor_cur_6->setText(QString::number(p_motor6_->getCurrent(),'f',3));
    //ui->lineEdit_motor_act_tor_1->setText(QString::number(p_motor1_->getReactionMoment_()));
    ui->lineEdit_motor_act_tor_6->setText(QString::number(p_motor6_->getAveTorque(),'f',2));
    ui->lineEdit_motor_hope_tor_6->setText(QString::number(p_motor6_->getHopeAveTorque(),'f',2));
    ui->lineEdit_motor_jdl_6->setText(QString::number(p_motor6_->getAngularMomentum500(),'f',4));
    ui->lineEdit_motor_jdl_czpc_6->setText(QString::number(p_motor6_->getAngularMomentumConst500(),'f',4));
    ui->lineEdit_motor_jdl_dtpc_6->setText(QString::number(p_motor6_->getAngularMomentumDynamic500(),'f',4));
    ui->lineEdit_motor_wate_6->setText(QString::number(p_motor6_->getWate(),'f',2));
    ui->lineEdit_motor_wate_overview_6->setText(QString::number(p_motor6_->getWate(),'f',2));
    ui->lineEdit_time_constant_6->setText(QString::number(p_motor6_->getTimeConstant(),'f',3));
}

//转速改变响应函数
void MainWindow::on_doubleSpinBox_motor_test_spd_1_editingFinished()
{
    if(p_motor1_->getIsRunning()){
        switch(ui->comboBox_motor_test_mode_1->currentIndex()){
        case 0: p_motor1_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_1->text().toDouble());break;
        case 3: p_motor1_->setSetTorque(ui->doubleSpinBox_motor_test_spd_1->text().toDouble());break;
            //如果要滑行模式也能实现滚动滑轮改变setspd,要在这里加case
        default:
            break;
        }
    }
}
void MainWindow::on_doubleSpinBox_motor_test_spd_2_editingFinished()
{
    if(p_motor2_->getIsRunning()){
        switch(ui->comboBox_motor_test_mode_2->currentIndex()){
        case 0: p_motor2_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_2->text().toDouble());break;
        case 3: p_motor2_->setSetTorque(ui->doubleSpinBox_motor_test_spd_2->text().toDouble());break;
            //如果要滑行模式也能实现滚动滑轮改变setspd,要在这里加case
        default:
            break;
        }
    }
}
void MainWindow::on_doubleSpinBox_motor_test_spd_3_editingFinished()
{
    if(p_motor3_->getIsRunning()){
        switch(ui->comboBox_motor_test_mode_3->currentIndex()){
        case 0: p_motor3_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_3->text().toDouble());break;
        case 3: p_motor3_->setSetTorque(ui->doubleSpinBox_motor_test_spd_3->text().toDouble());break;
            //如果要滑行模式也能实现滚动滑轮改变setspd,要在这里加case
        default:
            break;
        }
    }
}
void MainWindow::on_doubleSpinBox_motor_test_spd_4_editingFinished()
{
    if(p_motor4_->getIsRunning()){
        switch(ui->comboBox_motor_test_mode_4->currentIndex()){
        case 0: p_motor4_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_4->text().toDouble());break;
        case 3: p_motor4_->setSetTorque(ui->doubleSpinBox_motor_test_spd_4->text().toDouble());break;
            //如果要滑行模式也能实现滚动滑轮改变setspd,要在这里加case
        default:
            break;
        }
    }
}
void MainWindow::on_doubleSpinBox_motor_test_spd_5_editingFinished()
{
    if(p_motor5_->getIsRunning()){
        switch(ui->comboBox_motor_test_mode_5->currentIndex()){
        case 0: p_motor5_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_5->text().toDouble());break;
        case 3: p_motor5_->setSetTorque(ui->doubleSpinBox_motor_test_spd_5->text().toDouble());break;
            //如果要滑行模式也能实现滚动滑轮改变setspd,要在这里加case
        default:
            break;
        }
    }
}
void MainWindow::on_doubleSpinBox_motor_test_spd_6_editingFinished()
{
    if(p_motor6_->getIsRunning()){
        switch(ui->comboBox_motor_test_mode_6->currentIndex()){
        case 0: p_motor6_->setSetSpeed(ui->doubleSpinBox_motor_test_spd_6->text().toDouble());break;
        case 3: p_motor6_->setSetTorque(ui->doubleSpinBox_motor_test_spd_6->text().toDouble());break;
            //如果要滑行模式也能实现滚动滑轮改变setspd,要在这里加case
        default:
            break;
        }
    }
}

//测试模式下拉框改变响应函数
void MainWindow::on_comboBox_motor_test_mode_1_currentIndexChanged(int index)
{

    ui->spinBox_sin_A_1->setEnabled(false);
    ui->doubleSpinBox_sin_K_1->setEnabled(false);
    ui->spinBox_sin_T_1->setEnabled(false);
    switch (index) {
    case 0://速度模式
        ui->doubleSpinBox_motor_test_spd_1->setValue(1000);
        ui->label_setval_1->setText("设置速度");
        ui->label_setval_right_1->setText("设置速度");
        ui->label_52_1->setText("rpm");
        ui->label_5_1->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_1->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);


        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_2->setText("转速增量");
        ui->label_6->setText("rpm");
        break;
    case 1://斜坡模式
        ui->doubleSpinBox_motor_test_spd_1->setValue(1000);
        ui->doubleSpinBox_motor_test_acc_1->setValue(10);
        ui->label_setval_1->setText("设置速度");//终速--
        ui->label_setval_right_1->setText("设置速度");
        ui->label_52_1->setText("rpm");
        ui->label_5_1->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_1->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_2->setText("转速增量");
        ui->label_6->setText("rpm");
        break;
    case 2://正弦模式
        ui->label_setval_1->setText("设置速度");
        ui->label_setval_right_1->setText("设置速度");
        ui->label_52_1->setText("rpm");
        ui->label_5_1->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_1->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->spinBox_sin_A_1->setEnabled(true);
        ui->spinBox_sin_A_1->setValue(1000);
        ui->doubleSpinBox_sin_K_1->setEnabled(true);
        ui->doubleSpinBox_sin_K_1->setValue(1);
        ui->spinBox_sin_T_1->setEnabled(true);
        ui->spinBox_sin_T_1->setValue(90);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_2->setText("转速增量");
        ui->label_6->setText("rpm");
        break;
    case 3://力矩模式
        ui->label_setval_1->setText("设置力矩");
        ui->label_setval_right_1->setText("设置力矩");
        ui->label_52_1->setText("mNm");
        ui->label_5_1->setText("mNm");
        ui->doubleSpinBox_motor_test_spd_1->setValue(0.3);
        ui->doubleSpinBox_motor_test_spd_1->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_2->setText("转速增量");
        ui->label_6->setText("rpm");
        break;

    case 4://时间常数计算
        ui->label_setval_1->setText("设置速度");
        ui->label_setval_right_1->setText("设置速度");
        ui->label_52_1->setText("rpm");
        ui->label_5_1->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_1->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_2->setText("转速增量");
        ui->label_6->setText("rpm");
        break;

    case 5://滑行模式
        ui->label_setval_1->setText("设置速度");
        ui->label_setval_right_1->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_1->setValue(2000);
        ui->label_52_1->setText("rpm");
        ui->label_5_1->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_1->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_2->setText("转速增量");
        ui->label_6->setText("rpm");
        break;
    case 6://寿命模式
        ui->label_setval_1->setText("设置速度");
        ui->label_setval_right_1->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_1->setValue(6000);
        ui->doubleSpinBox_motor_test_acc_1->setValue(180);
        ui->label_52_1->setText("rpm");
        ui->label_5_1->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_1->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);

        ui->label_2->setText("间隔时间");
        ui->label_6->setText("s");
        break;

    default:
        break;
    }
}
void MainWindow::on_comboBox_motor_test_mode_2_currentIndexChanged(int index)
{

    ui->spinBox_sin_A_2->setEnabled(false);
    ui->doubleSpinBox_sin_K_2->setEnabled(false);
    ui->spinBox_sin_T_2->setEnabled(false);
    switch (index) {
    case 0://速度模式
        ui->doubleSpinBox_motor_test_spd_2->setValue(1000);
        ui->label_setval_2->setText("设置速度");
        ui->label_setval_right_2->setText("设置速度");
        ui->label_52_2->setText("rpm");
        ui->label_5_2->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_2->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_2->setEnabled(false);


        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_71->setText("转速增量");
        ui->label_73->setText("rpm");
        break;
    case 1://斜坡模式
        ui->doubleSpinBox_motor_test_spd_2->setValue(1000);
        ui->label_setval_2->setText("设置速度");//终速--
        ui->label_setval_right_2->setText("设置速度");
        ui->doubleSpinBox_motor_test_acc_2->setValue(10);
        ui->label_52_2->setText("rpm");
        ui->label_5_2->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_2->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_2->setEnabled(true);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_71->setText("转速增量");
        ui->label_73->setText("rpm");
        break;
    case 2://正弦模式
        ui->label_setval_2->setText("设置速度");
        ui->label_setval_right_2->setText("设置速度");
        ui->label_52_2->setText("rpm");
        ui->label_5_2->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_2->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_2->setEnabled(false);
        ui->spinBox_sin_A_2->setEnabled(true);
        ui->spinBox_sin_A_2->setValue(1000);
        ui->doubleSpinBox_sin_K_2->setEnabled(true);
        ui->doubleSpinBox_sin_K_2->setValue(1);
        ui->spinBox_sin_T_2->setEnabled(true);
        ui->spinBox_sin_T_2->setValue(90);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_71->setText("转速增量");
        ui->label_73->setText("rpm");
        break;
    case 3://力矩模式
        ui->label_setval_2->setText("设置力矩");
        ui->label_setval_right_2->setText("设置力矩");
        ui->label_52_2->setText("mNm");
        ui->label_5_2->setText("mNm");
        ui->doubleSpinBox_motor_test_spd_2->setValue(0);
        ui->doubleSpinBox_motor_test_spd_2->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_2->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_71->setText("转速增量");
        ui->label_73->setText("rpm");
        break;

    case 4://时间常数计算
        ui->label_setval_2->setText("设置速度");
        ui->label_setval_right_2->setText("设置速度");
        ui->label_52_2->setText("rpm");
        ui->label_5_2->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_2->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_2->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_71->setText("转速增量");
        ui->label_73->setText("rpm");
        break;
    case 5://滑行模式
        ui->label_setval_2->setText("设置速度");
        ui->label_setval_right_2->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_2->setValue(2000);
        ui->label_52_2->setText("rpm");
        ui->label_5_2->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_2->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_2->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_71->setText("转速增量");
        ui->label_73->setText("rpm");
        break;
    case 6://寿命模式
        ui->label_setval_2->setText("设置速度");
        ui->label_setval_right_2->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_2->setValue(6000);
        ui->doubleSpinBox_motor_test_acc_2->setValue(180);
        ui->label_52_2->setText("rpm");
        ui->label_5_2->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_2->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_2->setEnabled(true);

        ui->label_71->setText("间隔时间");
        ui->label_73->setText("s");
        break;
    default:
        break;
    }
}
void MainWindow::on_comboBox_motor_test_mode_3_currentIndexChanged(int index)
{

    ui->spinBox_sin_A_3->setEnabled(false);
    ui->doubleSpinBox_sin_K_3->setEnabled(false);
    ui->spinBox_sin_T_3->setEnabled(false);
    switch (index) {
    case 0://速度模式
        ui->doubleSpinBox_motor_test_spd_3->setValue(1000);
        ui->label_setval_3->setText("设置速度");
        ui->label_setval_right_3->setText("设置速度");
        ui->label_52_3->setText("rpm");
        ui->label_5_3->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_3->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_3->setEnabled(false);


        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_132->setText("转速增量");
        ui->label_134->setText("rpm");
        break;
    case 1://斜坡模式
        ui->doubleSpinBox_motor_test_spd_3->setValue(1000);
        ui->doubleSpinBox_motor_test_acc_3->setValue(10);
        ui->label_setval_3->setText("设置速度");//终速--
        ui->label_setval_right_3->setText("设置速度");
        ui->label_52_3->setText("rpm");
        ui->label_5_3->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_3->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_3->setEnabled(true);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_132->setText("转速增量");
        ui->label_134->setText("rpm");
        break;
    case 2://正弦模式
        ui->label_setval_3->setText("设置速度");
        ui->label_setval_right_3->setText("设置速度");
        ui->label_52_3->setText("rpm");
        ui->label_5_3->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_3->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_3->setEnabled(false);
        ui->spinBox_sin_A_3->setEnabled(true);
        ui->spinBox_sin_A_3->setValue(1000);
        ui->doubleSpinBox_sin_K_3->setEnabled(true);
        ui->doubleSpinBox_sin_K_3->setValue(1);
        ui->spinBox_sin_T_3->setEnabled(true);
        ui->spinBox_sin_T_3->setValue(90);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_132->setText("转速增量");
        ui->label_134->setText("rpm");
        break;
    case 3://力矩模式
        ui->label_setval_3->setText("设置力矩");
        ui->label_setval_right_3->setText("设置力矩");
        ui->label_52_3->setText("mNm");
        ui->label_5_3->setText("mNm");
        ui->doubleSpinBox_motor_test_spd_3->setValue(0.3);
        ui->doubleSpinBox_motor_test_spd_3->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_3->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_132->setText("转速增量");
        ui->label_134->setText("rpm");
        break;

    case 4://时间常数计算
        ui->label_setval_3->setText("设置速度");
        ui->label_setval_right_3->setText("设置速度");
        ui->label_52_3->setText("rpm");
        ui->label_5_3->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_3->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_3->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_132->setText("转速增量");
        ui->label_134->setText("rpm");
        break;
    case 5://滑行模式
        ui->label_setval_3->setText("设置速度");
        ui->label_setval_right_3->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_3->setValue(2000);
        ui->label_52_3->setText("rpm");
        ui->label_5_3->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_3->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_3->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_132->setText("转速增量");
        ui->label_134->setText("rpm");
        break;
    case 6://寿命模式
        ui->label_setval_3->setText("设置速度");
        ui->label_setval_right_3->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_3->setValue(6000);
        ui->doubleSpinBox_motor_test_acc_3->setValue(180);
        ui->label_52_3->setText("rpm");
        ui->label_5_3->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_3->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_3->setEnabled(true);

        ui->label_132->setText("间隔时间");
        ui->label_134->setText("s");
        break;
    default:
        break;
    }
}
void MainWindow::on_comboBox_motor_test_mode_4_currentIndexChanged(int index)
{

    ui->spinBox_sin_A_4->setEnabled(false);
    ui->doubleSpinBox_sin_K_4->setEnabled(false);
    ui->spinBox_sin_T_4->setEnabled(false);
    switch (index) {
    case 0://速度模式
        ui->doubleSpinBox_motor_test_spd_4->setValue(1000);
        ui->label_setval_4->setText("设置速度");
        ui->label_setval_right_4->setText("设置速度");
        ui->label_52_4->setText("rpm");
        ui->label_5_4->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_4->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_4->setEnabled(false);


        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_140->setText("转速增量");
        ui->label_142->setText("rpm");
        break;
    case 1://斜坡模式
        ui->doubleSpinBox_motor_test_spd_4->setValue(1000);
        ui->doubleSpinBox_motor_test_acc_4->setValue(10);
        ui->label_setval_4->setText("设置速度");//终速--
        ui->label_setval_right_4->setText("设置速度");
        ui->label_52_4->setText("rpm");
        ui->label_5_4->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_4->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_4->setEnabled(true);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_140->setText("转速增量");
        ui->label_142->setText("rpm");
        break;
    case 2://正弦模式

        ui->label_setval_4->setText("设置速度");
        ui->label_setval_right_4->setText("设置速度");
        ui->label_52_4->setText("rpm");
        ui->label_5_4->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_4->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_4->setEnabled(false);
        ui->spinBox_sin_A_4->setEnabled(true);
        ui->spinBox_sin_A_4->setValue(1000);
        ui->doubleSpinBox_sin_K_4->setEnabled(true);
        ui->doubleSpinBox_sin_K_4->setValue(1);
        ui->spinBox_sin_T_4->setEnabled(true);
        ui->spinBox_sin_T_4->setValue(90);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_140->setText("转速增量");
        ui->label_142->setText("rpm");
        break;
    case 3://力矩模式
        ui->label_setval_4->setText("设置力矩");
        ui->label_setval_right_4->setText("设置力矩");
        ui->label_52_4->setText("mNm");
        ui->label_5_4->setText("mNm");
        ui->doubleSpinBox_motor_test_spd_4->setValue(0.3);
        ui->doubleSpinBox_motor_test_spd_4->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_4->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_140->setText("转速增量");
        ui->label_142->setText("rpm");
        break;

    case 4://时间常数计算
        ui->label_setval_4->setText("设置速度");
        ui->label_setval_right_4->setText("设置速度");
        ui->label_52_4->setText("rpm");
        ui->label_5_4->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_4->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_4->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_140->setText("转速增量");
        ui->label_142->setText("rpm");
        break;
    case 5://滑行模式
        ui->label_setval_4->setText("设置速度");
        ui->label_setval_right_4->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_4->setValue(2000);
        ui->label_52_4->setText("rpm");
        ui->label_5_4->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_4->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_4->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_140->setText("转速增量");
        ui->label_142->setText("rpm");
        break;
    case 6://寿命模式
        ui->label_setval_4->setText("设置速度");
        ui->label_setval_right_4->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_4->setValue(6000);
        ui->doubleSpinBox_motor_test_acc_4->setValue(180);
        ui->label_52_4->setText("rpm");
        ui->label_5_4->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_4->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_4->setEnabled(true);

        ui->label_140->setText("间隔时间");
        ui->label_142->setText("s");
        break;
    default:
        break;
    }
}
void MainWindow::on_comboBox_motor_test_mode_5_currentIndexChanged(int index)
{

    ui->spinBox_sin_A_5->setEnabled(false);
    ui->doubleSpinBox_sin_K_5->setEnabled(false);
    ui->spinBox_sin_T_5->setEnabled(false);
    switch (index) {
    case 0://速度模式
        ui->doubleSpinBox_motor_test_spd_5->setValue(1000);
        ui->label_setval_5->setText("设置速度");
        ui->label_setval_right_5->setText("设置速度");
        ui->label_52_5->setText("rpm");
        ui->label_5_5->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_5->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_5->setEnabled(false);


        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_148->setText("转速增量");
        ui->label_150->setText("rpm");
        break;
    case 1://斜坡模式
        ui->doubleSpinBox_motor_test_spd_5->setValue(1000);
        ui->doubleSpinBox_motor_test_acc_5->setValue(10);
        ui->label_setval_5->setText("设置速度");//终速--
        ui->label_setval_right_5->setText("设置速度");
        ui->label_52_5->setText("rpm");
        ui->label_5_5->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_5->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_5->setEnabled(true);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_148->setText("转速增量");
        ui->label_150->setText("rpm");
        break;
    case 2://正弦模式
        ui->label_setval_5->setText("设置速度");
        ui->label_setval_right_5->setText("设置速度");
        ui->label_52_5->setText("rpm");
        ui->label_5_5->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_5->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_5->setEnabled(false);
        ui->spinBox_sin_A_5->setEnabled(true);
        ui->spinBox_sin_A_5->setValue(1000);
        ui->doubleSpinBox_sin_K_5->setEnabled(true);
        ui->doubleSpinBox_sin_K_5->setValue(1);
        ui->spinBox_sin_T_5->setEnabled(true);
        ui->spinBox_sin_T_5->setValue(90);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_148->setText("转速增量");
        ui->label_150->setText("rpm");
        break;
    case 3://力矩模式
        ui->label_setval_5->setText("设置力矩");
        ui->label_setval_right_5->setText("设置力矩");
        ui->label_52_5->setText("mNm");
        ui->label_5_5->setText("mNm");
        ui->doubleSpinBox_motor_test_spd_5->setValue(0.3);
        ui->doubleSpinBox_motor_test_spd_5->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_5->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_148->setText("转速增量");
        ui->label_150->setText("rpm");
        break;

    case 4://时间常数计算
        ui->label_setval_5->setText("设置速度");
        ui->label_setval_right_5->setText("设置速度");
        ui->label_52_5->setText("rpm");
        ui->label_5_5->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_5->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_5->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_148->setText("转速增量");
        ui->label_150->setText("rpm");
        break;
    case 5://滑行模式
        ui->label_setval_5->setText("设置速度");
        ui->label_setval_right_5->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_5->setValue(2000);
        ui->label_52_5->setText("rpm");
        ui->label_5_5->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_5->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_5->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_148->setText("转速增量");
        ui->label_150->setText("rpm");
        break;
    case 6://寿命模式
        ui->label_setval_5->setText("设置速度");
        ui->label_setval_right_5->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_5->setValue(6000);
        ui->doubleSpinBox_motor_test_acc_5->setValue(180);
        ui->label_52_5->setText("rpm");
        ui->label_5_5->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_5->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_5->setEnabled(true);

        ui->label_148->setText("间隔时间");
        ui->label_150->setText("s");
        break;
    default:
        break;
    }
}
void MainWindow::on_comboBox_motor_test_mode_6_currentIndexChanged(int index)
{

    ui->spinBox_sin_A_6->setEnabled(false);
    ui->doubleSpinBox_sin_K_6->setEnabled(false);
    ui->spinBox_sin_T_6->setEnabled(false);
    switch (index) {
    case 0://速度模式
        ui->doubleSpinBox_motor_test_spd_6->setValue(1000);
        ui->label_setval_6->setText("设置速度");
        ui->label_setval_right_6->setText("设置速度");
        ui->label_52_6->setText("rpm");
        ui->label_5_6->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_6->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_6->setEnabled(false);


        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_156->setText("转速增量");
        ui->label_158->setText("rpm");
        break;
    case 1://斜坡模式
        ui->doubleSpinBox_motor_test_spd_6->setValue(1000);
        ui->doubleSpinBox_motor_test_acc_6->setValue(10);
        ui->label_setval_6->setText("设置速度");//终速--
        ui->label_setval_right_6->setText("设置速度");
        ui->label_52_6->setText("rpm");
        ui->label_5_6->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_6->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_6->setEnabled(true);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(false);
        ui->label_156->setText("转速增量");
        ui->label_158->setText("rpm");
        break;
    case 2://正弦模式
        ui->label_setval_6->setText("设置速度");
        ui->label_setval_right_6->setText("设置速度");
        ui->label_52_6->setText("rpm");
        ui->label_5_6->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_6->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_6->setEnabled(false);
        ui->spinBox_sin_A_6->setEnabled(true);
        ui->spinBox_sin_A_6->setValue(1000);
        ui->doubleSpinBox_sin_K_6->setEnabled(true);
        ui->doubleSpinBox_sin_K_6->setValue(1);
        ui->spinBox_sin_T_6->setEnabled(true);
        ui->spinBox_sin_T_6->setValue(90);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_156->setText("转速增量");
        ui->label_158->setText("rpm");
        break;
    case 3://力矩模式
        ui->label_setval_6->setText("设置力矩");
        ui->label_setval_right_6->setText("设置力矩");
        ui->label_52_6->setText("mNm");
        ui->label_5_6->setText("mNm");
        ui->doubleSpinBox_motor_test_spd_6->setValue(0.3);
        ui->doubleSpinBox_motor_test_spd_6->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_6->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_156->setText("转速增量");
        ui->label_158->setText("rpm");
        break;

    case 4://时间常数计算
        ui->label_setval_6->setText("设置速度");
        ui->label_setval_right_6->setText("设置速度");
        ui->label_52_6->setText("rpm");
        ui->label_5_6->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_6->setEnabled(false);
        ui->doubleSpinBox_motor_test_acc_6->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_156->setText("转速增量");
        ui->label_158->setText("rpm");
        break;
    case 5://滑行模式
        ui->label_setval_6->setText("设置速度");
        ui->label_setval_right_6->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_6->setValue(2000);
        ui->label_52_6->setText("rpm");
        ui->label_5_6->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_6->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_6->setEnabled(false);
        //        ui->doubleSpinBox_motor_test_acc_1->setEnabled(true);
        ui->label_156->setText("转速增量");
        ui->label_158->setText("rpm");
        break;
    case 6://寿命模式
        ui->label_setval_6->setText("设置速度");
        ui->label_setval_right_6->setText("设置速度");
        ui->doubleSpinBox_motor_test_spd_6->setValue(6000);
        ui->doubleSpinBox_motor_test_acc_6->setValue(180);
        ui->label_52_6->setText("rpm");
        ui->label_5_6->setText("rpm");
        ui->doubleSpinBox_motor_test_spd_6->setEnabled(true);
        ui->doubleSpinBox_motor_test_acc_6->setEnabled(true);

        ui->label_156->setText("间隔时间");
        ui->label_158->setText("s");
        break;
    default:
        break;
    }
}

//刷新界面速度、电流等图部分
void MainWindow::refreshCustomPlotData1()
{
    static QTime time(QTime::currentTime());//只初始化一次，下次进入该函数time内的值等于上次的值
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 0.5 s
    {

        // add data to lines:
        keyContainer.push_back(key);
        //tmpContainer.push_back(p_motor1_->getTemperature());
        torContainer.push_back(p_motor1_->getAveTorque());
        curContainer.push_back(p_motor1_->getCurrent());
        spdContainer.push_back(p_motor1_->getSpeed());
        setSpdContainer.push_back(p_motor1_->getSetSpeed());

        ui->qcp_motor_act_tor_1->graph(0)->setData(keyContainer,torContainer,true);
        ui->qcp_motor_cur_1->graph(0)->setData(keyContainer,curContainer,true);
        ui->qcp_motor_spd_1->graph(0)->setData(keyContainer,spdContainer,true);
        ui->qcp_motor_spd_1->graph(1)->setData(keyContainer,setSpdContainer,true);
        lastPointKey = key;
        int disp_num = ui->label_tab1_disp_num_1->text().toInt();
        if (keyContainer.size() >= disp_num){
            keyContainer.pop_front();
            //tmpContainer.pop_front();
            torContainer.pop_front();
            curContainer.pop_front();
            spdContainer.pop_front();
            setSpdContainer.pop_front();
        }
        ui->qcp_motor_act_tor_1->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_cur_1->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_1->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_1->graph(1)->rescaleValueAxis(true);

        ui->qcp_motor_cur_1->yAxis->setRange(*std::min_element(curContainer.begin(),curContainer.end()),
                                             *std::max_element(curContainer.begin(),curContainer.end()));
        ui->qcp_motor_spd_1->yAxis->setRange(*std::min_element(spdContainer.begin(),spdContainer.end()),
                                             *std::max_element(spdContainer.begin(),spdContainer.end()));
        ui->qcp_motor_act_tor_1->yAxis->setRange(*std::min_element(tmpContainer.begin(),tmpContainer.end()),
                                                 *std::max_element(tmpContainer.begin(),tmpContainer.end()));

        ui->qcp_motor_cur_1->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_1->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_1->xAxis->setRange(key, disp_num/20, Qt::AlignRight);

        ui->qcp_motor_cur_1->replot();
        ui->qcp_motor_act_tor_1->replot();
        ui->qcp_motor_spd_1->replot();

        if (key > 43200){
            //12h重置一次！
            time.restart();
            lastPointKey = 0;
            keyContainer.clear();
            //tmpContainer.clear();
            torContainer.clear();
            curContainer.clear();
            spdContainer.clear();
            setSpdContainer.clear();
        }

    }
    // make key axis range scroll with the data (at a constant range size of 8):




}
void MainWindow::refreshCustomPlotData2()
{
    static QTime time(QTime::currentTime());//只初始化一次，下次进入该函数time内的值等于上次的值
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 0.5 s
    {

        // add data to lines:
        keyContainer_2.push_back(key);
        //tmpContainer.push_back(p_motor1_->getTemperature());
        torContainer_2.push_back(p_motor2_->getAveTorque());
        curContainer_2.push_back(p_motor2_->getCurrent());
        spdContainer_2.push_back(p_motor2_->getSpeed());
        setSpdContainer_2.push_back(p_motor2_->getSetSpeed());

        ui->qcp_motor_act_tor_2->graph(0)->setData(keyContainer_2,torContainer_2,true);
        ui->qcp_motor_cur_2->graph(0)->setData(keyContainer_2,curContainer_2,true);
        ui->qcp_motor_spd_2->graph(0)->setData(keyContainer_2,spdContainer_2,true);
        ui->qcp_motor_spd_2->graph(1)->setData(keyContainer_2,setSpdContainer_2,true);
        lastPointKey = key;
        int disp_num = ui->label_tab1_disp_num_2->text().toInt();
        if (keyContainer_2.size() >= disp_num){
            keyContainer_2.pop_front();
            //tmpContainer.pop_front();
            torContainer_2.pop_front();
            curContainer_2.pop_front();
            spdContainer_2.pop_front();
            setSpdContainer_2.pop_front();
        }
        ui->qcp_motor_act_tor_2->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_cur_2->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_2->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_2->graph(1)->rescaleValueAxis(true);

        ui->qcp_motor_cur_2->yAxis->setRange(*std::min_element(curContainer_2.begin(),curContainer_2.end()),
                                             *std::max_element(curContainer_2.begin(),curContainer_2.end()));
        ui->qcp_motor_spd_2->yAxis->setRange(*std::min_element(spdContainer_2.begin(),spdContainer_2.end()),
                                             *std::max_element(spdContainer_2.begin(),spdContainer_2.end()));
        ui->qcp_motor_act_tor_2->yAxis->setRange(*std::min_element(tmpContainer_2.begin(),tmpContainer_2.end()),
                                                 *std::max_element(tmpContainer_2.begin(),tmpContainer_2.end()));

        ui->qcp_motor_cur_2->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_2->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_2->xAxis->setRange(key, disp_num/20, Qt::AlignRight);

        ui->qcp_motor_cur_2->replot();
        ui->qcp_motor_act_tor_2->replot();
        ui->qcp_motor_spd_2->replot();

        if (key > 43200){
            //12h重置一次！
            time.restart();
            lastPointKey = 0;
            keyContainer_2.clear();
            //tmpContainer.clear();
            torContainer_2.clear();
            curContainer_2.clear();
            spdContainer_2.clear();
            setSpdContainer_2.clear();
        }

    }
    // make key axis range scroll with the data (at a constant range size of 8):




}
void MainWindow::refreshCustomPlotData3()
{
    static QTime time(QTime::currentTime());//只初始化一次，下次进入该函数time内的值等于上次的值
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 0.5 s
    {

        // add data to lines:
        keyContainer_3.push_back(key);
        //tmpContainer.push_back(p_motor1_->getTemperature());
        torContainer_3.push_back(p_motor3_->getAveTorque());
        curContainer_3.push_back(p_motor3_->getCurrent());
        spdContainer_3.push_back(p_motor3_->getSpeed());
        setSpdContainer_3.push_back(p_motor3_->getSetSpeed());

        ui->qcp_motor_act_tor_3->graph(0)->setData(keyContainer_3,torContainer_3,true);
        ui->qcp_motor_cur_3->graph(0)->setData(keyContainer_3,curContainer_3,true);
        ui->qcp_motor_spd_3->graph(0)->setData(keyContainer_3,spdContainer_3,true);
        ui->qcp_motor_spd_3->graph(1)->setData(keyContainer_3,setSpdContainer_3,true);
        lastPointKey = key;
        int disp_num = ui->label_tab1_disp_num_3->text().toInt();
        if (keyContainer_3.size() >= disp_num){
            keyContainer_3.pop_front();
            //tmpContainer.pop_front();
            torContainer_3.pop_front();
            curContainer_3.pop_front();
            spdContainer_3.pop_front();
            setSpdContainer_3.pop_front();
        }
        ui->qcp_motor_act_tor_3->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_cur_3->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_3->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_3->graph(1)->rescaleValueAxis(true);

        ui->qcp_motor_cur_3->yAxis->setRange(*std::min_element(curContainer_3.begin(),curContainer_3.end()),
                                             *std::max_element(curContainer_3.begin(),curContainer_3.end()));
        ui->qcp_motor_spd_3->yAxis->setRange(*std::min_element(spdContainer_3.begin(),spdContainer_3.end()),
                                             *std::max_element(spdContainer_3.begin(),spdContainer_3.end()));
        ui->qcp_motor_act_tor_3->yAxis->setRange(*std::min_element(tmpContainer_3.begin(),tmpContainer_3.end()),
                                                 *std::max_element(tmpContainer_3.begin(),tmpContainer_3.end()));

        ui->qcp_motor_cur_3->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_3->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_3->xAxis->setRange(key, disp_num/20, Qt::AlignRight);

        ui->qcp_motor_cur_3->replot();
        ui->qcp_motor_act_tor_3->replot();
        ui->qcp_motor_spd_3->replot();

        if (key > 43200){
            //12h重置一次！
            time.restart();
            lastPointKey = 0;
            keyContainer_3.clear();
            //tmpContainer.clear();
            torContainer_3.clear();
            curContainer_3.clear();
            spdContainer_3.clear();
            setSpdContainer_3.clear();
        }

    }
    // make key axis range scroll with the data (at a constant range size of 8):




}
void MainWindow::refreshCustomPlotData4()
{
    static QTime time(QTime::currentTime());//只初始化一次，下次进入该函数time内的值等于上次的值
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 0.5 s
    {

        // add data to lines:
        keyContainer_4.push_back(key);
        //tmpContainer.push_back(p_motor1_->getTemperature());
        torContainer_4.push_back(p_motor4_->getAveTorque());
        curContainer_4.push_back(p_motor4_->getCurrent());
        spdContainer_4.push_back(p_motor4_->getSpeed());
        setSpdContainer_4.push_back(p_motor4_->getSetSpeed());

        ui->qcp_motor_act_tor_4->graph(0)->setData(keyContainer_4,torContainer_4,true);
        ui->qcp_motor_cur_4->graph(0)->setData(keyContainer_4,curContainer_4,true);
        ui->qcp_motor_spd_4->graph(0)->setData(keyContainer_4,spdContainer_4,true);
        ui->qcp_motor_spd_4->graph(1)->setData(keyContainer_4,setSpdContainer_4,true);
        lastPointKey = key;
        int disp_num = ui->label_tab1_disp_num_4->text().toInt();
        if (keyContainer_4.size() >= disp_num){
            keyContainer_4.pop_front();
            //tmpContainer.pop_front();
            torContainer_4.pop_front();
            curContainer_4.pop_front();
            spdContainer_4.pop_front();
            setSpdContainer_4.pop_front();
        }
        ui->qcp_motor_act_tor_4->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_cur_4->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_4->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_4->graph(1)->rescaleValueAxis(true);

        ui->qcp_motor_cur_4->yAxis->setRange(*std::min_element(curContainer_4.begin(),curContainer_4.end()),
                                             *std::max_element(curContainer_4.begin(),curContainer_4.end()));
        ui->qcp_motor_spd_4->yAxis->setRange(*std::min_element(spdContainer_4.begin(),spdContainer_4.end()),
                                             *std::max_element(spdContainer_4.begin(),spdContainer_4.end()));
        ui->qcp_motor_act_tor_4->yAxis->setRange(*std::min_element(tmpContainer_4.begin(),tmpContainer_4.end()),
                                                 *std::max_element(tmpContainer_4.begin(),tmpContainer_4.end()));

        ui->qcp_motor_cur_4->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_4->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_4->xAxis->setRange(key, disp_num/20, Qt::AlignRight);

        ui->qcp_motor_cur_4->replot();
        ui->qcp_motor_act_tor_4->replot();
        ui->qcp_motor_spd_4->replot();

        if (key > 43200){
            //12h重置一次！
            time.restart();
            lastPointKey = 0;
            keyContainer_4.clear();
            //tmpContainer.clear();
            torContainer_4.clear();
            curContainer_4.clear();
            spdContainer_4.clear();
            setSpdContainer_4.clear();
        }

    }
    // make key axis range scroll with the data (at a constant range size of 8):




}
void MainWindow::refreshCustomPlotData5()
{
    static QTime time(QTime::currentTime());//只初始化一次，下次进入该函数time内的值等于上次的值
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 0.5 s
    {

        // add data to lines:
        keyContainer_5.push_back(key);
        //tmpContainer.push_back(p_motor1_->getTemperature());
        torContainer_5.push_back(p_motor5_->getAveTorque());
        curContainer_5.push_back(p_motor5_->getCurrent());
        spdContainer_5.push_back(p_motor5_->getSpeed());
        setSpdContainer_5.push_back(p_motor5_->getSetSpeed());

        ui->qcp_motor_act_tor_5->graph(0)->setData(keyContainer_5,torContainer_5,true);
        ui->qcp_motor_cur_5->graph(0)->setData(keyContainer_5,curContainer_5,true);
        ui->qcp_motor_spd_5->graph(0)->setData(keyContainer_5,spdContainer_5,true);
        ui->qcp_motor_spd_5->graph(1)->setData(keyContainer_5,setSpdContainer_5,true);
        lastPointKey = key;
        int disp_num = ui->label_tab1_disp_num_5->text().toInt();
        if (keyContainer_5.size() >= disp_num){
            keyContainer_5.pop_front();
            //tmpContainer.pop_front();
            torContainer_5.pop_front();
            curContainer_5.pop_front();
            spdContainer_5.pop_front();
            setSpdContainer_5.pop_front();
        }
        ui->qcp_motor_act_tor_5->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_cur_5->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_5->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_5->graph(1)->rescaleValueAxis(true);

        ui->qcp_motor_cur_5->yAxis->setRange(*std::min_element(curContainer_5.begin(),curContainer_5.end()),
                                             *std::max_element(curContainer_5.begin(),curContainer_5.end()));
        ui->qcp_motor_spd_5->yAxis->setRange(*std::min_element(spdContainer_5.begin(),spdContainer_5.end()),
                                             *std::max_element(spdContainer_5.begin(),spdContainer_5.end()));
        ui->qcp_motor_act_tor_5->yAxis->setRange(*std::min_element(tmpContainer_5.begin(),tmpContainer_5.end()),
                                                 *std::max_element(tmpContainer_5.begin(),tmpContainer_5.end()));

        ui->qcp_motor_cur_5->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_5->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_5->xAxis->setRange(key, disp_num/20, Qt::AlignRight);

        ui->qcp_motor_cur_5->replot();
        ui->qcp_motor_act_tor_5->replot();
        ui->qcp_motor_spd_5->replot();

        if (key > 43200){
            //12h重置一次！
            time.restart();
            lastPointKey = 0;
            keyContainer_5.clear();
            //tmpContainer.clear();
            torContainer_5.clear();
            curContainer_5.clear();
            spdContainer_5.clear();
            setSpdContainer_5.clear();
        }

    }
    // make key axis range scroll with the data (at a constant range size of 8):




}
void MainWindow::refreshCustomPlotData6()
{
    static QTime time(QTime::currentTime());//只初始化一次，下次进入该函数time内的值等于上次的值
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds  elaspsed上次开始后得持续时间
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 0.5 s
    {

        // add data to lines:
        keyContainer_6.push_back(key);
        //tmpContainer.push_back(p_motor1_->getTemperature());
        torContainer_6.push_back(p_motor6_->getAveTorque());
        curContainer_6.push_back(p_motor6_->getCurrent());
        spdContainer_6.push_back(p_motor6_->getSpeed());
        setSpdContainer_6.push_back(p_motor6_->getSetSpeed());

        ui->qcp_motor_act_tor_6->graph(0)->setData(keyContainer_6,torContainer_6,true);
        ui->qcp_motor_cur_6->graph(0)->setData(keyContainer_6,curContainer_6,true);
        ui->qcp_motor_spd_6->graph(0)->setData(keyContainer_6,spdContainer_6,true);
        ui->qcp_motor_spd_6->graph(1)->setData(keyContainer_6,setSpdContainer_6,true);
        lastPointKey = key;
        int disp_num = ui->label_tab1_disp_num_6->text().toInt();
        if (keyContainer_6.size() >= disp_num){
            keyContainer_6.pop_front();
            //tmpContainer.pop_front();
            torContainer_6.pop_front();
            curContainer_6.pop_front();
            spdContainer_6.pop_front();
            setSpdContainer_6.pop_front();
        }
        ui->qcp_motor_act_tor_6->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_cur_6->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_6->graph(0)->rescaleValueAxis(true);
        ui->qcp_motor_spd_6->graph(1)->rescaleValueAxis(true);

        ui->qcp_motor_cur_6->yAxis->setRange(*std::min_element(curContainer_6.begin(),curContainer_6.end()),
                                             *std::max_element(curContainer_6.begin(),curContainer_6.end()));
        ui->qcp_motor_spd_6->yAxis->setRange(*std::min_element(spdContainer_6.begin(),spdContainer_6.end()),
                                             *std::max_element(spdContainer_6.begin(),spdContainer_6.end()));
        ui->qcp_motor_act_tor_6->yAxis->setRange(*std::min_element(tmpContainer_6.begin(),tmpContainer_6.end()),
                                                 *std::max_element(tmpContainer_6.begin(),tmpContainer_6.end()));

        ui->qcp_motor_cur_6->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_6->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_6->xAxis->setRange(key, disp_num/20, Qt::AlignRight);

        ui->qcp_motor_cur_6->replot();
        ui->qcp_motor_act_tor_6->replot();
        ui->qcp_motor_spd_6->replot();

        if (key > 43200){
            //12h重置一次！
            time.restart();
            lastPointKey = 0;
            keyContainer_6.clear();
            //tmpContainer.clear();
            torContainer_6.clear();
            curContainer_6.clear();
            spdContainer_6.clear();
            setSpdContainer_6.clear();
        }

    }
    // make key axis range scroll with the data (at a constant range size of 8):




}

//设置界面数据更新间隔，可以控制采样时间。
void MainWindow::on_doubleSpinBox_moto_test_time_1_valueChanged(double arg1)
{
    //让采样时间等于current_interval
    m_timer_update_.setInterval(arg1*1000);
    //    p_motor1_->setCurrentInterval(arg1);
    p_motor1_->setCurrentInterval(ui->doubleSpinBox_moto_test_time_1->text().toDouble());//设置电机类current_interval的接口
    //ui->doubleSpinBox_moto_test_time_1->text().toDouble()只改了一路，通过这样的方法算出的力矩是对的，不知道是为什么
}
void MainWindow::on_doubleSpinBox_moto_test_time_2_valueChanged(double arg2)
{
    //让采样时间等于current_interval
    m_timer_update_2.setInterval(arg2*1000);//
    p_motor2_->setCurrentInterval(arg2);
    //    p_motor2_->setCurrentInterval(ui->doubleSpinBox_test_interval_2->text().toDouble());//设置电机类current_interval的接口
}
void MainWindow::on_doubleSpinBox_moto_test_time_3_valueChanged(double arg3)
{
    //让采样时间等于current_interval
    m_timer_update_3.setInterval(arg3*1000);//
    p_motor3_->setCurrentInterval(arg3*1000);
    //    p_motor3_->setCurrentInterval(ui->doubleSpinBox_test_interval_3->text().toDouble());//设置电机类current_interval的接口
}
void MainWindow::on_doubleSpinBox_moto_test_time_4_valueChanged(double arg4)
{
    //让采样时间等于current_interval
    m_timer_update_4.setInterval(arg4*1000);//
    p_motor4_->setCurrentInterval(arg4);
    //    p_motor4_->setCurrentInterval(ui->doubleSpinBox_test_interval_4->text().toDouble());//设置电机类current_interval的接口
}
void MainWindow::on_doubleSpinBox_moto_test_time_5_valueChanged(double arg5)
{
    //让采样时间等于current_interval
    m_timer_update_5.setInterval(arg5*1000);//
    p_motor5_->setCurrentInterval(arg5);
    //    p_motor5_->setCurrentInterval(ui->doubleSpinBox_test_interval_5->text().toDouble());//设置电机类current_interval的接口
}
void MainWindow::on_doubleSpinBox_moto_test_time_6_valueChanged(double arg6)
{
    //让采样时间等于current_interval
    m_timer_update_6.setInterval(arg6*1000);//
    p_motor6_->setCurrentInterval(arg6);
    //    p_motor6_->setCurrentInterval(ui->doubleSpinBox_test_interval_6->text().toDouble());//设置电机类current_interval的接口
}

//查询按钮的槽函数 可以加个复选框，如果复选框的生成execel被勾上就执行生成execel的函数否则就跳过。
void MainWindow::on_pushButton_sql_query_clicked()
{
    if(m_sys_status_1_ && ui->lineEdit_sql_motor_id->text() == "MOTOR1" ){
        QMessageBox::warning(this,"警告","飞轮1正在运行，请关闭飞轮1的电源，再按查询按钮！");
        return;
    }

    else if(m_sys_status_2_ && ui->lineEdit_sql_motor_id->text() == "MOTOR2"){
        QMessageBox::warning(this,"警告","飞轮2正在运行，请关闭飞轮2的电源，再按查询按钮！");
        return;
    }
    else if(m_sys_status_3_ && ui->lineEdit_sql_motor_id->text() == "MOTOR3"){
        QMessageBox::warning(this,"警告","飞轮3正在运行，请关闭飞轮3的电源，再按查询按钮！");
        return;
    }
    else if(m_sys_status_4_ && ui->lineEdit_sql_motor_id->text() == "MOTOR4"){
        QMessageBox::warning(this,"警告","飞轮4正在运行，请关闭飞轮4的电源，再按查询按钮！");
        return;
    }
    else if(m_sys_status_5_ && ui->lineEdit_sql_motor_id->text() == "MOTOR5"){
        QMessageBox::warning(this,"警告","飞轮5正在运行，请关闭飞轮5的电源，再按查询按钮！");
        return;
    }
    else if(m_sys_status_6_ && ui->lineEdit_sql_motor_id->text() == "MOTOR6"){
        QMessageBox::warning(this,"警告","飞轮6正在运行，请关闭飞轮6的电源，再按查询按钮！");
        return;
    }
    tab_num_ = 0;
    //    p_sql_->getExpDataFromSqlDB(ui->lineEdit_sql_motor_id->text(),ui->lineEdit_sql_exp_id->text()
    //                                ,QString::number(ui->comboBox_sql_motor_mode->currentIndex()),ui->dateTimeEdit_start_time->dateTime().toString("yyyy-MM-dd hh:mm:ss"),ui->dateTimeEdit_end_time->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    emit sendQueryWordToDB(ui->lineEdit_sql_motor_id->text(),ui->lineEdit_sql_exp_id->text()
                           ,QString::number(ui->comboBox_sql_motor_mode->currentIndex()),ui->dateTimeEdit_start_time->dateTime().toString("yyyy-MM-dd hh:mm:ss"),ui->dateTimeEdit_end_time->dateTime().toString("yyyy-MM-dd hh:mm:ss"));;
}
void MainWindow::updataSqlTableView(QVector<QVector<QString> > res)
{
    if (res.empty()){
        QMessageBox::warning(this,"警告","请检查查询时间是否正确！");
        //logMsg(tr("未查询到数据！"));
        return;
    }
    page=1;
    ui->lineEdit_page->setText(QString::number(page));
    query_dispResult_=res;
    InquireDisplay();
    //res.resize(0);

}
void MainWindow::InquireDisplay( ){
    static QStandardItemModel  *model = nullptr;
    if (model == nullptr){
        model = new QStandardItemModel();//？
    }
    //    else{
    //        delete model;
    //        model = new QStandardItemModel();//？
    //    }
    if (tab_num_ == 0){
        model->clear();
        model->setColumnCount(23);
        model->setHeaderData(0,Qt::Horizontal,"实验编号");
        model->setHeaderData(1,Qt::Horizontal,"产品编号");
        model->setHeaderData(2,Qt::Horizontal,"飞轮编号");
        model->setHeaderData(3,Qt::Horizontal,"测试目的");
        model->setHeaderData(4,Qt::Horizontal,"测试时间");
        model->setHeaderData(5,Qt::Horizontal,"测试场景");
        model->setHeaderData(6,Qt::Horizontal,"测试地点");
        model->setHeaderData(7,Qt::Horizontal,"电压");
        model->setHeaderData(8,Qt::Horizontal,"电流");
        model->setHeaderData(9,Qt::Horizontal,"飞轮设定速度");
        model->setHeaderData(10,Qt::Horizontal,"飞轮实际速度");
        model->setHeaderData(11,Qt::Horizontal,"飞轮设定力矩");
        model->setHeaderData(12,Qt::Horizontal,"飞轮实际力矩");
        model->setHeaderData(13,Qt::Horizontal,"飞轮期望力矩");
        model->setHeaderData(14,Qt::Horizontal,"平均功耗");
        model->setHeaderData(15,Qt::Horizontal,"飞轮角动量");
        model->setHeaderData(16,Qt::Horizontal,"飞轮角动量最大常值偏差");
        model->setHeaderData(17,Qt::Horizontal,"飞轮角动量最大动态偏差");
        model->setHeaderData(18,Qt::Horizontal,"飞轮模式");
        model->setHeaderData(19,Qt::Horizontal,"时间常数");
        model->setHeaderData(20,Qt::Horizontal,"采集电流");
        model->setHeaderData(21,Qt::Horizontal,"设置电流");
        model->setHeaderData(22,Qt::Horizontal,"运行时间");
        ui->tableView_sql->setModel(model);
        ui->previous_page->setEnabled(true);
        ui->next_page->setEnabled(true);
    }
    //下面的没看懂
    for ( tab_num_=(page-1)*100;tab_num_<query_dispResult_.size();tab_num_++){//size = 999

        if(tab_num_<(page-1)*100)
        {
            continue;
        }

        if(tab_num_>((page-1)*100+99))//
        {
            break;
        }
        for (int j=0;j<7;++j){
            model->setItem(tab_num_-(page-1)*100,j,new QStandardItem(query_dispResult_.at(tab_num_).at(j)));
            model->item(tab_num_-(page-1)*100,j)->setTextAlignment(Qt::AlignCenter);
        }
        for (int j=7;j<9;++j){
            model->setItem(tab_num_-(page-1)*100,j,new QStandardItem(QString::number(query_dispResult_.at(tab_num_).at(j).toFloat(),'f',3)));
            model->item(tab_num_-(page-1)*100,j)->setTextAlignment(Qt::AlignCenter);
        }
        for (int j=9;j<15;++j){
            model->setItem(tab_num_-(page-1)*100,j,new QStandardItem(QString::number(query_dispResult_.at(tab_num_).at(j).toFloat(),'f',2)));
            model->item(tab_num_-(page-1)*100,j)->setTextAlignment(Qt::AlignCenter);
        }
        for (int j=15;j<18;++j){
            model->setItem(tab_num_-(page-1)*100,j,new QStandardItem(QString::number(query_dispResult_.at(tab_num_).at(j).toFloat(),'f',4)));
            model->item(tab_num_-(page-1)*100,j)->setTextAlignment(Qt::AlignCenter);
        }
        for (int j=18;j<22;++j){
            model->setItem(tab_num_-(page-1)*100,j,new QStandardItem(QString::number(query_dispResult_.at(tab_num_).at(j).toFloat(),'f',3)));
            model->item(tab_num_-(page-1)*100,j)->setTextAlignment(Qt::AlignCenter);
        }
        model->setItem(tab_num_-(page-1)*100,22,new QStandardItem(query_dispResult_.at(tab_num_).at(22)));
        model->item(tab_num_-(page-1)*100,21)->setTextAlignment(Qt::AlignCenter);

    }
    //加入复选框的判断 如果生成execel的复选框被选中就执行createExcel函数否则就跳过。
    //p_repoter_->createExcel(res);//这个可能有问题
}

void MainWindow::initSerialInfo()
{
    //此函数用于配置串口信息，需要在串口init前调用
    QFile log_file("config.log");
    if (!log_file.exists()){
        //若不存在则先创建配置文件
        if (!log_file.open(QIODevice::ReadOnly | QIODevice::Text)){
            logMsg("create config error!");
            return;
        }
        QTextStream ts(&log_file);
        ts<<"NO,J,K,A,T,LJ,SPD,CUR,CALNUM,SERIAL,BAUDRATE,CHECK\n";
        ts<<"1,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM1,115200,ODD\n";
        ts<<"2,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM2,115200,ODD\n";
        ts<<"3,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM8,115200,ODD\n";
        ts<<"4,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM4,115200,ODD\n";
        ts<<"5,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM5,115200,ODD\n";
        ts<<"6,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM6,115200,ODD\n";
        log_file.close();
    }
    if (!log_file.open(QIODevice::ReadWrite | QIODevice::Text)){
        logMsg("read config error!");
        return;
    }
    else{
        QTextStream ts(&log_file);
        ts.readLine();
        for (int i = 0;i < 6;i++){
            QString line = ts.readLine();
            QStringList lst = line.split(',');
            int no = lst.at(0).toInt();
            if (no != 1 &&no != 2&&no != 3&&
                    no != 4&&no != 5&&no != 6)
            {
                logMsg("config file error!");
                return;
            }
            else{
                logMsg("config success!");
                if (no == 1){
                    serial_channel_1 = lst.at(9);
                    serial_baudrate_1 = lst.at(10);
                }
                else if (no == 2){
                    serial_channel_2 = lst.at(9);
                    serial_baudrate_2 = lst.at(10);
                }
                else if (no == 3){
                    serial_channel_3 = lst.at(9);
                    serial_baudrate_3 = lst.at(10);
                }
                else if (no == 4){
                    serial_channel_4 = lst.at(9);
                    serial_baudrate_4 = lst.at(10);
                }
                else if (no == 5){
                    serial_channel_5 = lst.at(9);
                    serial_baudrate_5 = lst.at(10);
                }
                else if (no == 6){
                    serial_channel_6 = lst.at(9);
                    serial_baudrate_6 = lst.at(10);
                }
            }
        }
    }
}

void MainWindow::initSettingTab()
{
    //此函数配置motor相关信息，并不配置串口信息。
    
    QFile log_file("config.log");
    if (!log_file.exists()){
        //若不存在则先创建配置文件
        if (!log_file.open(QIODevice::ReadWrite | QIODevice::Text)){
            logMsg("create config error!");
            return;
        }
        QTextStream ts(&log_file);
        ts<<"NO,J,K,A,T,LJ,SPD,CUR,CALNUM,SERIAL,BAUDRATE,CHECK\n";
        ts<<"1,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM1,115200,ODD\n";
        ts<<"2,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM2,115200,ODD\n";
        ts<<"3,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM8,115200,ODD\n";
        ts<<"4,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM4,115200,ODD\n";
        ts<<"5,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM5,115200,ODD\n";
        ts<<"6,0.1111,1111,1111,1111,0.1111,0.1111,0.1111,1111,COM6,115200,ODD\n";
        ts<<"7,E:/\n";
        log_file.close();
    }
    if (!log_file.open(QIODevice::ReadWrite | QIODevice::Text)){
        logMsg("read config error!");
        return;
    }
    else{
        //serial port
        QStringList newPortStringList;
        newPortStringList.push_back("NULL");
        //        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        //            newPortStringList += info.portName();
        //        }
        QStringList baud_rate;
        //        baud_rate.push_back("1200");
        //        baud_rate.push_back("2400");
        //        baud_rate.push_back("4800");
        //        baud_rate.push_back("9600");
        //        baud_rate.push_back("14400");
        //        baud_rate.push_back("19200");
        //        baud_rate.push_back("38400");
        //        baud_rate.push_back("43000");
        //        baud_rate.push_back("57600");
        //        baud_rate.push_back("76800");
        //        baud_rate.push_back("115200");
        QStringList odd_even;
        //        odd_even.push_back("ODD");
        //        odd_even.push_back("EVEN");
        //        odd_even.push_back("NONE");

        logMsg("read config success!");
        QTextStream ts(&log_file);
        ts.readLine();



        for (int i = 0;i < 7;i++){
            QString line = ts.readLine();
            QStringList lst = line.split(',');
            int no = lst.at(0).toInt();
            if (no != 1 &&no != 2&&no != 3&&
                    no != 4&&no != 5&&no != 6&&no != 7)
            {
                logMsg("config file error!");
                return;
            }
            else{
                logMsg("config success!");
                if (no == 1){
                    //j
                    double j = lst.at(1).toDouble();
                    p_motor1_->setJ(j);
                    ui->doubleSpinBox_J_1->setValue(j);
                    ui->doubleSpinBox_fw1_J->setValue(j);
                    //k
                    double k = lst.at(2).toDouble();
                    p_motor1_->setK(k);
                    ui->doubleSpinBox_fw1_K->setValue(k);
                    ui->doubleSpinBox_sin_K_1->setValue(k);
                    
                    //a
                    double a = lst.at(3).toDouble();
                    p_motor1_->setA(a);
                    ui->doubleSpinBox_fw1_A->setValue(a);
                    ui->spinBox_sin_A_1->setValue(a);
                    //t
                    double t = lst.at(4).toDouble();
                    p_motor1_->setT(t);
                    ui->doubleSpinBox_fw1_T->setValue(t);
                    ui->spinBox_sin_T_1->setValue(t);

                    //lj
                    double lj = lst.at(5).toDouble();
                    p_can_driver_->setResolution_ratio(lj);
                    ui->doubleSpinBox_setting_lj->setValue(lj);
                    ui->doubleSpinBox_lj_resolution_ratio_1->setValue(lj);
                    //spd
                    double spd = lst.at(6).toDouble();
                    p_can_driver_->setSpdResolutionRatio(spd);
                    ui->doubleSpinBox_setting_spd->setValue(spd);
                    //cur
                    double cur = lst.at(7).toDouble();
                    p_can_driver_->setCurrentResolutionRatio(cur);
                    ui->doubleSpinBox_setting_cur->setValue(cur);
                    //calnum
                    int num = lst.at(8).toInt();
                    p_motor1_->setAverageNum(num);
                    ui->doubleSpinBox_setting_avgnum->setValue(num);

                    //portname
                    ui->comboBox_setting_portname->addItems(newPortStringList);
                    for (int j = 0;j<newPortStringList.size();++j){
                        if (lst.at(9) == newPortStringList.at(j)){
                            ui->comboBox_setting_portname->setCurrentIndex(j);
                        }
                    }
                    //baud
                    ui->comboBox_setting_baudrate->addItems(baud_rate);
                    for (int j = 0;j<baud_rate.size();++j){
                        if (lst.at(10) == baud_rate.at(j)){
                            ui->comboBox_setting_baudrate->setCurrentIndex(j);
                        }
                    }
                    //odd even
                    ui->comboBox_setting_oddenev->addItems(odd_even);


                }
                else if (no == 2){
                    //j
                    double j = lst.at(1).toDouble();
                    p_motor2_->setJ(j);
                    ui->doubleSpinBox_J_2->setValue(j);
                    ui->doubleSpinBox_fw2_J->setValue(j);
                    //k
                    double k = lst.at(2).toDouble();
                    p_motor2_->setK(k);
                    ui->doubleSpinBox_fw2_K->setValue(k);

                    //a
                    double a = lst.at(3).toDouble();
                    p_motor2_->setA(a);
                    ui->doubleSpinBox_fw2_A->setValue(a);
                    //t
                    double t = lst.at(4).toDouble();
                    p_motor2_->setT(t);
                    ui->doubleSpinBox_fw2_T->setValue(t);
                    //lj
                    double lj = lst.at(5).toDouble();
                    //                    p_driver2_->setResolution_ratio(lj);
                    ui->doubleSpinBox_setting_lj_2->setValue(lj);
                    ui->doubleSpinBox_lj_resolution_ratio_2->setValue(lj);
                    //spd
                    double spd = lst.at(6).toDouble();
                    //                    p_driver2_->setSpdResolutionRatio(spd);
                    ui->doubleSpinBox_setting_spd_2->setValue(spd);
                    //cur
                    double cur = lst.at(7).toDouble();
                    //                    p_driver2_->setCurrentResolutionRatio(cur);
                    ui->doubleSpinBox_setting_cur_2->setValue(cur);
                    //calnum
                    int num = lst.at(8).toInt();
                    p_motor2_->setAverageNum(num);
                    ui->doubleSpinBox_setting_avgnum_2->setValue(num);

                    //portname
                    ui->comboBox_setting_portname_2->addItems(newPortStringList);
                    for (int j = 0;j<newPortStringList.size();++j){
                        if (lst.at(9) == newPortStringList.at(j)){
                            ui->comboBox_setting_portname_2->setCurrentIndex(j);
                        }
                    }
                    //baud
                    ui->comboBox_setting_baudrate_2->addItems(baud_rate);
                    for (int j = 0;j<baud_rate.size();++j){
                        if (lst.at(10) == baud_rate.at(j)){
                            ui->comboBox_setting_baudrate_2->setCurrentIndex(j);
                        }
                    }
                    //odd even
                    ui->comboBox_setting_oddenev_2->addItems(odd_even);

                }
                else if (no == 3){
                    //j
                    double j = lst.at(1).toDouble();
                    p_motor3_->setJ(j);
                    ui->doubleSpinBox_J_3->setValue(j);
                    ui->doubleSpinBox_fw3_J->setValue(j);
                    //k
                    double k = lst.at(2).toDouble();
                    p_motor3_->setK(k);
                    ui->doubleSpinBox_fw3_K->setValue(k);

                    //a
                    double a = lst.at(3).toDouble();
                    p_motor3_->setA(a);
                    ui->doubleSpinBox_fw3_A->setValue(a);
                    //t
                    double t = lst.at(4).toDouble();
                    p_motor3_->setT(t);
                    ui->doubleSpinBox_fw3_T->setValue(t);
                    //lj
                    double lj = lst.at(5).toDouble();
                    //                    p_driver3_->setResolution_ratio(lj);
                    ui->doubleSpinBox_setting_lj_3->setValue(lj);
                    ui->doubleSpinBox_lj_resolution_ratio_3->setValue(lj);
                    //spd
                    double spd = lst.at(6).toDouble();
                    //                    p_driver3_->setSpdResolutionRatio(spd);
                    ui->doubleSpinBox_setting_spd_3->setValue(spd);
                    //cur
                    double cur = lst.at(7).toDouble();
                    //                    p_driver3_->setCurrentResolutionRatio(cur);
                    ui->doubleSpinBox_setting_cur_3->setValue(cur);
                    //calnum
                    int num = lst.at(8).toInt();
                    p_motor3_->setAverageNum(num);
                    ui->doubleSpinBox_setting_avgnum_3->setValue(num);

                    //portname
                    ui->comboBox_setting_portname_3->addItems(newPortStringList);
                    for (int j = 0;j<newPortStringList.size();++j){
                        if (lst.at(9) == newPortStringList.at(j)){
                            ui->comboBox_setting_portname_3->setCurrentIndex(j);
                        }
                    }
                    //baud
                    ui->comboBox_setting_baudrate_3->addItems(baud_rate);
                    for (int j = 0;j<baud_rate.size();++j){
                        if (lst.at(10) == baud_rate.at(j)){
                            ui->comboBox_setting_baudrate_3->setCurrentIndex(j);
                        }
                    }
                    //odd even
                    ui->comboBox_setting_oddenev_3->addItems(odd_even);
                }
                else if (no == 4){
                    //j
                    double j = lst.at(1).toDouble();
                    p_motor4_->setJ(j);
                    ui->doubleSpinBox_J_4->setValue(j);
                    ui->doubleSpinBox_fw4_J->setValue(j);
                    //k
                    double k = lst.at(2).toDouble();
                    p_motor4_->setK(k);
                    ui->doubleSpinBox_fw4_K->setValue(k);

                    //a
                    double a = lst.at(3).toDouble();
                    p_motor4_->setA(a);
                    ui->doubleSpinBox_fw4_A->setValue(a);
                    //t
                    double t = lst.at(4).toDouble();
                    p_motor4_->setT(t);
                    ui->doubleSpinBox_fw4_T->setValue(t);
                    //lj
                    double lj = lst.at(5).toDouble();
                    //                    p_driver4_->setResolution_ratio(lj);
                    ui->doubleSpinBox_setting_lj_4->setValue(lj);
                    ui->doubleSpinBox_lj_resolution_ratio_4->setValue(lj);
                    //spd
                    double spd = lst.at(6).toDouble();
                    //                    p_driver4_->setSpdResolutionRatio(spd);
                    ui->doubleSpinBox_setting_spd_4->setValue(spd);
                    //cur
                    double cur = lst.at(7).toDouble();
                    //                    p_driver4_->setCurrentResolutionRatio(cur);
                    ui->doubleSpinBox_setting_cur_4->setValue(cur);
                    //calnum
                    int num = lst.at(8).toInt();
                    p_motor4_->setAverageNum(num);
                    ui->doubleSpinBox_setting_avgnum_4->setValue(num);

                    //portname
                    ui->comboBox_setting_portname_4->addItems(newPortStringList);
                    for (int j = 0;j<newPortStringList.size();++j){
                        if (lst.at(9) == newPortStringList.at(j)){
                            ui->comboBox_setting_portname_4->setCurrentIndex(j);
                        }
                    }
                    //baud
                    ui->comboBox_setting_baudrate_4->addItems(baud_rate);
                    for (int j = 0;j<baud_rate.size();++j){
                        if (lst.at(10) == baud_rate.at(j)){
                            ui->comboBox_setting_baudrate_4->setCurrentIndex(j);
                        }
                    }
                    //odd even
                    ui->comboBox_setting_oddenev_4->addItems(odd_even);
                }
                else if (no == 5){
                    //j
                    double j = lst.at(1).toDouble();
                    p_motor5_->setJ(j);
                    ui->doubleSpinBox_J_5->setValue(j);
                    ui->doubleSpinBox_fw5_J->setValue(j);
                    //k
                    double k = lst.at(2).toDouble();
                    p_motor5_->setK(k);
                    ui->doubleSpinBox_fw5_K->setValue(k);

                    //a
                    double a = lst.at(3).toDouble();
                    p_motor5_->setA(a);
                    ui->doubleSpinBox_fw5_A->setValue(a);
                    //t
                    double t = lst.at(4).toDouble();
                    p_motor5_->setT(t);
                    ui->doubleSpinBox_fw5_T->setValue(t);
                    //lj
                    double lj = lst.at(5).toDouble();
                    //                    p_driver5_->setResolution_ratio(lj);
                    ui->doubleSpinBox_setting_lj_5->setValue(lj);
                    ui->doubleSpinBox_lj_resolution_ratio_5->setValue(lj);
                    //spd
                    double spd = lst.at(6).toDouble();
                    //                    p_driver5_->setSpdResolutionRatio(spd);
                    ui->doubleSpinBox_setting_spd_5->setValue(spd);
                    //cur
                    double cur = lst.at(7).toDouble();
                    //                    p_driver5_->setCurrentResolutionRatio(cur);
                    ui->doubleSpinBox_setting_cur_5->setValue(cur);
                    //calnum
                    int num = lst.at(8).toInt();
                    p_motor5_->setAverageNum(num);
                    ui->doubleSpinBox_setting_avgnum_5->setValue(num);

                    //portname
                    ui->comboBox_setting_portname_5->addItems(newPortStringList);
                    for (int j = 0;j<newPortStringList.size();++j){
                        if (lst.at(9) == newPortStringList.at(j)){
                            ui->comboBox_setting_portname_5->setCurrentIndex(j);
                        }
                    }
                    //baud
                    ui->comboBox_setting_baudrate_5->addItems(baud_rate);
                    for (int j = 0;j<baud_rate.size();++j){
                        if (lst.at(10) == baud_rate.at(j)){
                            ui->comboBox_setting_baudrate_5->setCurrentIndex(j);
                        }
                    }
                    //odd even
                    ui->comboBox_setting_oddenev_5->addItems(odd_even);
                }
                else if (no == 6){
                    //j
                    double j = lst.at(1).toDouble();
                    p_motor6_->setJ(j);
                    ui->doubleSpinBox_J_6->setValue(j);
                    ui->doubleSpinBox_fw6_J->setValue(j);
                    //k
                    double k = lst.at(2).toDouble();
                    p_motor6_->setK(k);
                    ui->doubleSpinBox_fw6_K->setValue(k);

                    //a
                    double a = lst.at(3).toDouble();
                    p_motor6_->setA(a);
                    ui->doubleSpinBox_fw6_A->setValue(a);
                    //t
                    double t = lst.at(4).toDouble();
                    p_motor6_->setT(t);
                    ui->doubleSpinBox_fw6_T->setValue(t);
                    //lj
                    double lj = lst.at(5).toDouble();
                    //                    p_driver6_->setResolution_ratio(lj);
                    ui->doubleSpinBox_setting_lj_6->setValue(lj);
                    ui->doubleSpinBox_lj_resolution_ratio_6->setValue(lj);
                    //spd
                    double spd = lst.at(6).toDouble();
                    //                    p_driver6_->setSpdResolutionRatio(spd);
                    ui->doubleSpinBox_setting_spd_6->setValue(spd);
                    //cur
                    double cur = lst.at(7).toDouble();
                    //                    p_driver6_->setCurrentResolutionRatio(cur);
                    ui->doubleSpinBox_setting_cur_6->setValue(cur);
                    //calnum
                    int num = lst.at(8).toInt();
                    p_motor6_->setAverageNum(num);
                    ui->doubleSpinBox_setting_avgnum_6->setValue(num);

                    //portname
                    ui->comboBox_setting_portname_6->addItems(newPortStringList);
                    for (int j = 0;j<newPortStringList.size();++j){
                        if (lst.at(9) == newPortStringList.at(j)){
                            ui->comboBox_setting_portname_6->setCurrentIndex(j);
                        }
                    }
                    //baud
                    ui->comboBox_setting_baudrate_6->addItems(baud_rate);
                    for (int j = 0;j<baud_rate.size();++j){
                        if (lst.at(10) == baud_rate.at(j)){
                            ui->comboBox_setting_baudrate_6->setCurrentIndex(j);
                        }
                    }
                    //odd even
                    ui->comboBox_setting_oddenev_6->addItems(odd_even);
                }
                else if (no == 7){
                    p_repoter_->setSavePath(lst.at(1));
                    ui->label_setting_save_path->setText(lst.at(1));
                }
            }
        }
    }
}

void MainWindow::saveConfigFile()
{


    QFile::remove("config.log");
    QFile log_file("config.log");

    //        ts<<"NO,J,K,A,T,LJ,SPD,CUR,CALNUM,SERIAL,BAUDRATE,CHECK\n";
    if (!log_file.open(QIODevice::ReadWrite | QIODevice::Text)){
        logMsg("read config error!can not open config file!");
        return;
    }
    else{
        //        logMsg("read config success!");
        QTextStream ts(&log_file);
        ts<<"NO,J,K,A,T,LJ,SPD,CUR,CALNUM,SERIAL,BAUDRATE,CHECK\n";
        ts<<"1,"<<p_motor1_->getJ()<<","<<ui->doubleSpinBox_fw1_K->text()<<","<<ui->doubleSpinBox_fw1_A->text()<<","<<ui->doubleSpinBox_fw1_T->text()<<","<<ui->doubleSpinBox_setting_lj->text()<<","<<ui->doubleSpinBox_setting_spd->text()<<","<<ui->doubleSpinBox_setting_cur->text()<<","<<ui->doubleSpinBox_setting_avgnum->text()<<","<<ui->comboBox_setting_portname->currentText()<<","<<ui->comboBox_setting_baudrate->currentText()<<"\n";
        ts<<"2,"<<p_motor2_->getJ()<<","<<ui->doubleSpinBox_fw2_K->text()<<","<<ui->doubleSpinBox_fw2_A->text()<<","<<ui->doubleSpinBox_fw2_T->text()<<","<<ui->doubleSpinBox_setting_lj_2->text()<<","<<ui->doubleSpinBox_setting_spd_2->text()<<","<<ui->doubleSpinBox_setting_cur_2->text()<<","<<ui->doubleSpinBox_setting_avgnum_2->text()<<","<<ui->comboBox_setting_portname_2->currentText()<<","<<ui->comboBox_setting_baudrate_2->currentText()<<"\n";
        ts<<"3,"<<p_motor3_->getJ()<<","<<ui->doubleSpinBox_fw3_K->text()<<","<<ui->doubleSpinBox_fw3_A->text()<<","<<ui->doubleSpinBox_fw3_T->text()<<","<<ui->doubleSpinBox_setting_lj_3->text()<<","<<ui->doubleSpinBox_setting_spd_3->text()<<","<<ui->doubleSpinBox_setting_cur_3->text()<<","<<ui->doubleSpinBox_setting_avgnum_3->text()<<","<<ui->comboBox_setting_portname_3->currentText()<<","<<ui->comboBox_setting_baudrate_3->currentText()<<"\n";
        ts<<"4,"<<p_motor4_->getJ()<<","<<ui->doubleSpinBox_fw4_K->text()<<","<<ui->doubleSpinBox_fw4_A->text()<<","<<ui->doubleSpinBox_fw4_T->text()<<","<<ui->doubleSpinBox_setting_lj_4->text()<<","<<ui->doubleSpinBox_setting_spd_4->text()<<","<<ui->doubleSpinBox_setting_cur_4->text()<<","<<ui->doubleSpinBox_setting_avgnum_4->text()<<","<<ui->comboBox_setting_portname_4->currentText()<<","<<ui->comboBox_setting_baudrate_4->currentText()<<"\n";
        ts<<"5,"<<p_motor5_->getJ()<<","<<ui->doubleSpinBox_fw5_K->text()<<","<<ui->doubleSpinBox_fw5_A->text()<<","<<ui->doubleSpinBox_fw5_T->text()<<","<<ui->doubleSpinBox_setting_lj_5->text()<<","<<ui->doubleSpinBox_setting_spd_5->text()<<","<<ui->doubleSpinBox_setting_cur_5->text()<<","<<ui->doubleSpinBox_setting_avgnum_5->text()<<","<<ui->comboBox_setting_portname_5->currentText()<<","<<ui->comboBox_setting_baudrate_5->currentText()<<"\n";
        ts<<"6,"<<p_motor6_->getJ()<<","<<ui->doubleSpinBox_fw6_K->text()<<","<<ui->doubleSpinBox_fw6_A->text()<<","<<ui->doubleSpinBox_fw6_T->text()<<","<<ui->doubleSpinBox_setting_lj_6->text()<<","<<ui->doubleSpinBox_setting_spd_6->text()<<","<<ui->doubleSpinBox_setting_cur_6->text()<<","<<ui->doubleSpinBox_setting_avgnum_6->text()<<","<<ui->comboBox_setting_portname_6->currentText()<<","<<ui->comboBox_setting_baudrate_6->currentText()<<"\n";
        ts<<"7,"<<ui->label_setting_save_path->text()<<"\n";
    }
    log_file.close();
}

bool MainWindow::checkIsMultiId()
{
    std::map<int,int> count;
    count[ui->lineEdit_fwid_1->text().toInt()]++;
    count[ui->lineEdit_fwid_2->text().toInt()]++;
    count[ui->lineEdit_fwid_3->text().toInt()]++;
    count[ui->lineEdit_fwid_4->text().toInt()]++;
    count[ui->lineEdit_fwid_5->text().toInt()]++;
    count[ui->lineEdit_fwid_6->text().toInt()]++;
    for(auto it = count.begin(); it != count.end(); it++){
        if(it->second > 1){
            QMessageBox::critical(this, "飞轮ID设置重复！", QString::number(it->first) + "号ID设置重复！请检查重新设置飞轮ID！");
            return false;
        }
    }
    return true;

}
/*
void MainWindow::currentProtect()
{
    if(p_motor1_->getActCur() > ui->doubleSpinBox_limit_current_1->text().toDouble()){
        on_pushButton_system_power_1_clicked();
        QMessageBox::warning(this,"警告","电流过大");
         }
    if(p_motor2_->getActCur() > ui->doubleSpinBox_limit_current_2->text().toDouble()){
        on_pushButton_system_power_2_clicked();
        QMessageBox::warning(this,"警告","电流过大");
         }
    if(p_motor3_->getActCur() > ui->doubleSpinBox_limit_current_3->text().toDouble()){
        on_pushButton_system_power_3_clicked();
        QMessageBox::warning(this,"警告","电流过大");
         }
    if(p_motor4_->getActCur() > ui->doubleSpinBox_limit_current_4->text().toDouble()){
        on_pushButton_system_power_4_clicked();
        QMessageBox::warning(this,"警告","电流过大");
         }
    if(p_motor5_->getActCur() > ui->doubleSpinBox_limit_current_5->text().toDouble()){
        on_pushButton_system_power_5_clicked();
        QMessageBox::warning(this,"警告","电流过大");
         }
    if(p_motor6_->getActCur() > ui->doubleSpinBox_limit_current_6->text().toDouble()){
        on_pushButton_system_power_6_clicked();
        QMessageBox::warning(this,"警告","电流过大");
         }
}*/
void MainWindow::on_previous_page_clicked()
{
    /* if(number==0){
       QMessageBox::warning(this,"警告","当前已经是第一页");
    }
    else{
        number=number - 1;
        p_sql_->getExpDataFromSqlDB(ui->lineEdit_sql_motor_id->text(),ui->lineEdit_sql_exp_id->text()
                                ,QString::number(ui->comboBox_sql_motor_mode->currentIndex()),ui->dateTimeEdit_start_time->dateTime().toString("yyyy-MM-dd hh::mm::ss"),ui->dateTimeEdit_end_time->dateTime().toString("yyyy-MM-dd hh::mm::ss"));
    }*/
    page--;
    if(page<1)
    {
        page=1;
    }
    ui->lineEdit_page->setText(QString::number(page));

    InquireDisplay( );
}
void MainWindow::on_next_page_clicked()
{
    /* number += 1;
    p_sql_->getExpDataFromSqlDB(ui->lineEdit_sql_motor_id->text(),ui->lineEdit_sql_exp_id->text()
                            ,QString::number(ui->comboBox_sql_motor_mode->currentIndex()),ui->dateTimeEdit_start_time->dateTime().toString("yyyy-MM-dd hh::mm::ss"),ui->dateTimeEdit_end_time->dateTime().toString("yyyy-MM-dd hh::mm::ss"));
*/
    m=1;
    page++;
    if(page>(query_dispResult_.size()/100))
    {
        page=(query_dispResult_.size()/100);

    }
    else{
        ui->lineEdit_page->setText(QString::number(page));
        InquireDisplay( );
    }



}
//生成报表
void MainWindow::on_pushButton_make_report_clicked()
{
    //再加个实验id号
    emit getLastExpData(ui->lineEdit_sql_exp_id->text(),ui->lineEdit_sql_motor_id->text(),QString::number(ui->comboBox_sql_motor_mode->currentIndex()));
}
//生成excel
/*
void MainWindow::exportExcel(QVector<QVector<QString> > res){
     p_repoter_->createExcel(res);
}
*/
//读数据采集卡中的数据并计算
void MainWindow::setMotorDataFromDAQCard(QVector<double> res)
{
    if (res.size() != 12) return;
    //这个功耗是根据什么算的
    p_motor1_->setVoltage(res[6]*10);
    p_motor1_->setActCur(res[0]);//2.5 - res[0]
    if(p_motor1_->getAveCurrent() > ui->doubleSpinBox_limit_current_1->text().toDouble() && m_sys_status_1_){
        //QMessageBox::warning(this,"警告","电流过大");
        on_pushButton_system_power_1_clicked();
        QMessageBox::warning(this,"警告","飞轮1电流过大");
        //ui->statusBar->showMessage("电流过大");

    }

    if (qAbs(p_motor1_->getAverageSpd()) > 6100 && m_sys_status_1_) {
        on_pushButton_system_power_1_clicked();
        QMessageBox::warning(this, "警告", "飞轮1转速过大,最大6100rpm");
    }
    if (qAbs(p_motor2_->getAverageSpd()) > 6100 && m_sys_status_2_) {
        on_pushButton_system_power_2_clicked();
        QMessageBox::warning(this, "警告", "飞轮2转速过大,最大6100rpm");
    }
    if (qAbs(p_motor3_->getAverageSpd()) > 6100 && m_sys_status_3_) {
        on_pushButton_system_power_3_clicked();
        QMessageBox::warning(this, "警告", "飞轮3转速过大,最大6100rpm");
    }
    if (qAbs(p_motor4_->getAverageSpd()) > 6100 && m_sys_status_4_) {
        on_pushButton_system_power_4_clicked();
        QMessageBox::warning(this, "警告", "飞轮4转速过大,最大6100rpm");
    }
    if (qAbs(p_motor5_->getAverageSpd()) > 6100 && m_sys_status_5_) {
        on_pushButton_system_power_5_clicked();
        QMessageBox::warning(this, "警告", "飞轮5转速过大,最大6100rpm");
    }
    if (qAbs(p_motor6_->getAverageSpd()) > 6100 && m_sys_status_6_) {
        on_pushButton_system_power_6_clicked();
        QMessageBox::warning(this, "警告", "飞轮6转速过大,最大6100rpm");
    }

    p_motor1_->setWate();

    p_motor2_->setVoltage(res[7]*10);
    p_motor2_->setActCur(res[1]);
    if(p_motor2_->getAveCurrent() > ui->doubleSpinBox_limit_current_2->text().toDouble()&& m_sys_status_2_){

        on_pushButton_system_power_2_clicked();
        QMessageBox::warning(this,"警告","飞轮2电流过大");

    }
    p_motor2_->setWate();

    p_motor3_->setVoltage(res[8]*10);
    p_motor3_->setActCur(res[2]);
    if(p_motor3_->getAveCurrent() > ui->doubleSpinBox_limit_current_3->text().toDouble()&& m_sys_status_3_){
        on_pushButton_system_power_3_clicked();
        QMessageBox::warning(this,"警告","飞轮3电流过大");

    }
    p_motor3_->setWate();

    p_motor4_->setVoltage(res[9]*10);
    p_motor4_->setActCur(res[3]);
    if(p_motor4_->getAveCurrent() > ui->doubleSpinBox_limit_current_4->text().toDouble()&& m_sys_status_4_){
        on_pushButton_system_power_4_clicked();
        QMessageBox::warning(this,"警告","飞轮4电流过大");

    }
    p_motor4_->setWate();

    p_motor5_->setVoltage(res[10]*10);
    p_motor5_->setActCur(res[4]);
    if(p_motor5_->getAveCurrent() > ui->doubleSpinBox_limit_current_5->text().toDouble()&& m_sys_status_5_){
        on_pushButton_system_power_5_clicked();
        QMessageBox::warning(this,"警告","飞轮5电流过大");

    }
    p_motor5_->setWate();

    p_motor6_->setVoltage(res[11]*10);
    p_motor6_->setActCur(res[5]);
    if(p_motor6_->getAveCurrent() > ui->doubleSpinBox_limit_current_6->text().toDouble()&& m_sys_status_6_){

        on_pushButton_system_power_6_clicked();
        QMessageBox::warning(this,"警告","飞轮6电流过大");

    }
    p_motor6_->setWate();

    //currentProtect();
}
//浪涌电流测试
void MainWindow::on_pushButton_ele_test_ly_mode_power_1_clicked()
{
    if (m_sys_status_1_) {
        QMessageBox::warning(this,"警告","请关闭该飞轮电源后测试！");
        return;
    }

    QVector<double> current;
    QQueue<double> last_20_cur_queue_;
    QVector<double> countDots;
    bool start = false;
    double last_20_cur_;
    p_daqcard_->getSurgeCurrent(1,current,1024);//数据采集卡的0-5通道采集6个飞轮的电流
    QVector<double> key;
    for (int i =0;i<current.size();++i){
        key.push_back(i);
        if (last_20_cur_queue_.size() <20 && current[i]>0.09 ){
            last_20_cur_queue_.push_back(current[i]);
        }
        else if(current[i]>0.09){
            while (last_20_cur_queue_.size() >= 20){
                last_20_cur_queue_.pop_front();
            }
            last_20_cur_queue_.push_back(current[i]);
        }
        last_20_cur_ = 0;
        for(double &t:last_20_cur_queue_){
            last_20_cur_ += t;
        }
        last_20_cur_ /= last_20_cur_queue_.size();
        if(!start&&current[i]>0.2)
            start = true;
        else if(start){
            countDots.push_back(current[i]);
        }
        if(last_20_cur_>0.13 && last_20_cur_<0.17)//b点的稳态条件
            start = false;
    }
    //在电流显示界面用第二条线显示浪涌电流曲线（可能要改）
    ui->qcp_motor_cur_1->graph(1)->setData(key,current,true);
    ui->qcp_motor_cur_1->graph(1)->rescaleValueAxis(true);
    ui->qcp_motor_cur_1->yAxis->setRange(*std::min_element(current.begin(),current.end()),
                                         *std::max_element(current.begin(),current.end()));

    ui->qcp_motor_cur_1->xAxis->setRange(0, key.size(), Qt::AlignLeft);
    double peak = *std::max_element(current.begin(),current.end())-0;
    QString lbl_Peak_ConstantTime;
    lbl_Peak_ConstantTime +=QString::number(peak);
    lbl_Peak_ConstantTime +="A";
    ui->label_Peak_ConstantTime_1->setText(lbl_Peak_ConstantTime);
    ui->qcp_motor_cur_1->replot();
}
void MainWindow::on_pushButton_ele_test_ly_mode_power_2_clicked()
{
    if (m_sys_status_2_) {
        QMessageBox::warning(this,"警告","请关闭该飞轮电源后测试！");
        return;
    }

    QVector<double> current;
    QQueue<double> last_20_cur_queue_;
    QVector<double> countDots;
    bool start = false;
    double last_20_cur_;
    p_daqcard_->getSurgeCurrent(2,current,1024);
    QVector<double> key;
    for (int i =0;i<current.size();++i){
        key.push_back(i);
        if (last_20_cur_queue_.size() <20 && current[i]>0.09 ){
            last_20_cur_queue_.push_back(current[i]);
        }
        else if(current[i]>0.09){
            while (last_20_cur_queue_.size() >= 20){
                last_20_cur_queue_.pop_front();
            }
            last_20_cur_queue_.push_back(current[i]);
        }
        last_20_cur_ = 0;
        for(double &t:last_20_cur_queue_){
            last_20_cur_ += t;
        }
        last_20_cur_ /= last_20_cur_queue_.size();
        if(!start&&current[i]>0.2)
            start = true;
        else if(start){
            countDots.push_back(current[i]);
        }
        if(last_20_cur_>0.13 && last_20_cur_<0.17)//b点的稳态条件
            start = false;
    }
    //在电流显示界面用第二条线显示浪涌电流曲线（可能要改）
    ui->qcp_motor_cur_2->graph(1)->setData(key,current,true);
    ui->qcp_motor_cur_2->graph(1)->rescaleValueAxis(true);
    ui->qcp_motor_cur_2->yAxis->setRange(*std::min_element(current.begin(),current.end()),
                                         *std::max_element(current.begin(),current.end()));

    ui->qcp_motor_cur_2->xAxis->setRange(0, key.size(), Qt::AlignLeft);
    double peak = *std::max_element(current.begin(),current.end())-0;
    QString lbl_Peak_ConstantTime;
    lbl_Peak_ConstantTime +=QString::number(peak);
    lbl_Peak_ConstantTime +="A";
    ui->label_Peak_ConstantTime_2->setText(lbl_Peak_ConstantTime);
    ui->qcp_motor_cur_2->replot();
}
void MainWindow::on_pushButton_ele_test_ly_mode_power_3_clicked()
{
    if (m_sys_status_3_) {
        QMessageBox::warning(this,"警告","请关闭该飞轮电源后测试！");
        return;
    }

    QVector<double> current;
    QQueue<double> last_20_cur_queue_;
    QVector<double> countDots;
    bool start = false;
    double last_20_cur_;
    p_daqcard_->getSurgeCurrent(3,current,1024);
    QVector<double> key;
    for (int i =0;i<current.size();++i){
        key.push_back(i);
        if (last_20_cur_queue_.size() <20 && current[i]>0.09 ){
            last_20_cur_queue_.push_back(current[i]);
        }
        else if(current[i]>0.09){
            while (last_20_cur_queue_.size() >= 20){
                last_20_cur_queue_.pop_front();
            }
            last_20_cur_queue_.push_back(current[i]);
        }
        last_20_cur_ = 0;
        for(double &t:last_20_cur_queue_){
            last_20_cur_ += t;
        }
        last_20_cur_ /= last_20_cur_queue_.size();
        if(!start&&current[i]>0.2)
            start = true;
        else if(start){
            countDots.push_back(current[i]);
        }
        if(last_20_cur_>0.13 && last_20_cur_<0.17)//b点的稳态条件
            start = false;
    }
    //在电流显示界面用第二条线显示浪涌电流曲线（可能要改）
    ui->qcp_motor_cur_3->graph(1)->setData(key,current,true);
    ui->qcp_motor_cur_3->graph(1)->rescaleValueAxis(true);
    ui->qcp_motor_cur_3->yAxis->setRange(*std::min_element(current.begin(),current.end()),
                                         *std::max_element(current.begin(),current.end()));

    ui->qcp_motor_cur_3->xAxis->setRange(0, key.size(), Qt::AlignLeft);
    double peak = *std::max_element(current.begin(),current.end())-0;
    QString lbl_Peak_ConstantTime;
    lbl_Peak_ConstantTime +=QString::number(peak);
    lbl_Peak_ConstantTime +="A";
    ui->label_Peak_ConstantTime_3->setText(lbl_Peak_ConstantTime);
    ui->qcp_motor_cur_3->replot();
}
void MainWindow::on_pushButton_ele_test_ly_mode_power_4_clicked()
{
    if (m_sys_status_4_) {
        QMessageBox::warning(this,"警告","请关闭该飞轮电源后测试！");
        return;
    }

    QVector<double> current;
    QQueue<double> last_20_cur_queue_;
    QVector<double> countDots;
    bool start = false;
    double last_20_cur_;
    p_daqcard_->getSurgeCurrent(4,current,1024);
    QVector<double> key;
    for (int i =0;i<current.size();++i){
        key.push_back(i);
        //        if (last_20_cur_queue_.size() <20 && current[i]>0.09 ){
        //          last_20_cur_queue_.push_back(current[i]);
        //        }
        //        else if(current[i]>0.09){
        //          while (last_20_cur_queue_.size() >= 20){
        //              last_20_cur_queue_.pop_front();
        //          }
        //          last_20_cur_queue_.push_back(current[i]);
        //        }
        //        last_20_cur_ = 0;
        //        for(double &t:last_20_cur_queue_){
        //            last_20_cur_ += t;
        //        }
        //        last_20_cur_ /= last_20_cur_queue_.size();
        //        if(!start&&current[i]>0.2)
        //            start = true;
        //        else if(start){
        //            countDots.push_back(current[i]);
        //        }
        //        if(last_20_cur_>0.13 && last_20_cur_<0.17)//b点的稳态条件
        //            start = false;
    }
    //在电流显示界面用第二条线显示浪涌电流曲线（可能要改）
    ui->qcp_motor_cur_4->graph(1)->setData(key,current,true);
    ui->qcp_motor_cur_4->graph(1)->rescaleValueAxis(true);
    ui->qcp_motor_cur_4->yAxis->setRange(*std::min_element(current.begin(),current.end()),
                                         *std::max_element(current.begin(),current.end()));

    ui->qcp_motor_cur_4->xAxis->setRange(0, key.size(), Qt::AlignLeft);
    double peak = *std::max_element(current.begin(),current.end())-0;
    QString lbl_Peak_ConstantTime;
    lbl_Peak_ConstantTime +=QString::number(peak);
    lbl_Peak_ConstantTime +="A";
    ui->label_Peak_ConstantTime_4->setText(lbl_Peak_ConstantTime);
    ui->qcp_motor_cur_4->replot();
}
void MainWindow::on_pushButton_ele_test_ly_mode_power_5_clicked()
{
    if (m_sys_status_5_) {
        QMessageBox::warning(this,"警告","请关闭该飞轮电源后测试！");
        return;
    }

    QVector<double> current;
    QQueue<double> last_20_cur_queue_;
    QVector<double> countDots;
    bool start = false;
    double last_20_cur_;
    p_daqcard_->getSurgeCurrent(5,current,1024);
    QVector<double> key;
    for (int i =0;i<current.size();++i){
        key.push_back(i);
        if (last_20_cur_queue_.size() <20 && current[i]>0.09 ){
            last_20_cur_queue_.push_back(current[i]);
        }
        else if(current[i]>0.09){
            while (last_20_cur_queue_.size() >= 20){
                last_20_cur_queue_.pop_front();
            }
            last_20_cur_queue_.push_back(current[i]);
        }
        last_20_cur_ = 0;
        for(double &t:last_20_cur_queue_){
            last_20_cur_ += t;
        }
        last_20_cur_ /= last_20_cur_queue_.size();
        if(!start&&current[i]>0.2)
            start = true;
        else if(start){
            countDots.push_back(current[i]);
        }
        if(last_20_cur_>0.13 && last_20_cur_<0.17)//b点的稳态条件
            start = false;
    }
    //在电流显示界面用第二条线显示浪涌电流曲线（可能要改）
    ui->qcp_motor_cur_5->graph(1)->setData(key,current,true);
    ui->qcp_motor_cur_5->graph(1)->rescaleValueAxis(true);
    ui->qcp_motor_cur_5->yAxis->setRange(*std::min_element(current.begin(),current.end()),
                                         *std::max_element(current.begin(),current.end()));

    ui->qcp_motor_cur_5->xAxis->setRange(0, key.size(), Qt::AlignLeft);
    double peak = *std::max_element(current.begin(),current.end())-0;
    QString lbl_Peak_ConstantTime;
    lbl_Peak_ConstantTime +=QString::number(peak);
    lbl_Peak_ConstantTime +="A";
    ui->label_Peak_ConstantTime_5->setText(lbl_Peak_ConstantTime);
    ui->qcp_motor_cur_5->replot();
}
void MainWindow::on_pushButton_ele_test_ly_mode_power_6_clicked()
{
    if (m_sys_status_6_) {
        QMessageBox::warning(this,"警告","请关闭该飞轮电源后测试！");
        return;
    }

    QVector<double> current;
    QQueue<double> last_20_cur_queue_;
    QVector<double> countDots;
    bool start = false;
    double last_20_cur_;
    p_daqcard_->getSurgeCurrent(6,current,1024);
    QVector<double> key;
    for (int i =0;i<current.size();++i){
        key.push_back(i);
        if (last_20_cur_queue_.size() <20 && current[i]>0.09 ){
            last_20_cur_queue_.push_back(current[i]);
        }
        else if(current[i]>0.09){
            while (last_20_cur_queue_.size() >= 20){
                last_20_cur_queue_.pop_front();
            }
            last_20_cur_queue_.push_back(current[i]);
        }
        last_20_cur_ = 0;
        for(double &t:last_20_cur_queue_){
            last_20_cur_ += t;
        }
        last_20_cur_ /= last_20_cur_queue_.size();
        if(!start&&current[i]>0.2)
            start = true;
        else if(start){
            countDots.push_back(current[i]);
        }
        if(last_20_cur_>0.13 && last_20_cur_<0.17)//b点的稳态条件
            start = false;
    }
    //在电流显示界面用第二条线显示浪涌电流曲线（可能要改）
    ui->qcp_motor_cur_6->graph(1)->setData(key,current,true);
    ui->qcp_motor_cur_6->graph(1)->rescaleValueAxis(true);
    ui->qcp_motor_cur_6->yAxis->setRange(*std::min_element(current.begin(),current.end()),
                                         *std::max_element(current.begin(),current.end()));

    ui->qcp_motor_cur_6->xAxis->setRange(0, key.size(), Qt::AlignLeft);
    double peak = *std::max_element(current.begin(),current.end())-0;
    QString lbl_Peak_ConstantTime;
    lbl_Peak_ConstantTime +=QString::number(peak);
    lbl_Peak_ConstantTime +="A";
    ui->label_Peak_ConstantTime_6->setText(lbl_Peak_ConstantTime);
    ui->qcp_motor_cur_6->replot();
}

//绘图间隔改变
void MainWindow::on_tab1_horizontalSlider_1_valueChanged(int value)
{
    ui->label_tab1_disp_num_1->setText(QString::number(value));
    if (!p_motor1_->getIsRunning()){
        double key = ui->qcp_motor_cur_1->xAxis->range().upper;
        double disp_num = value;
        ui->qcp_motor_cur_1->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_1->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_1->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_cur_1->replot();
        ui->qcp_motor_act_tor_1->replot();
        ui->qcp_motor_spd_1->replot();
    }
}
void MainWindow::on_tab1_horizontalSlider_2_valueChanged(int value)
{
    ui->label_tab1_disp_num_2->setText(QString::number(value));
    if (!p_motor2_->getIsRunning()){
        double key = ui->qcp_motor_cur_2->xAxis->range().upper;
        double disp_num = value;
        ui->qcp_motor_cur_2->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_2->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_2->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_cur_2->replot();
        ui->qcp_motor_act_tor_2->replot();
        ui->qcp_motor_spd_2->replot();
    }
}
void MainWindow::on_tab1_horizontalSlider_3_valueChanged(int value)
{
    ui->label_tab1_disp_num_3->setText(QString::number(value));
    if (!p_motor3_->getIsRunning()){
        double key = ui->qcp_motor_cur_3->xAxis->range().upper;
        double disp_num = value;
        ui->qcp_motor_cur_3->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_3->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_3->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_cur_3->replot();
        ui->qcp_motor_act_tor_3->replot();
        ui->qcp_motor_spd_3->replot();
    }
}
void MainWindow::on_tab1_horizontalSlider_4_valueChanged(int value)
{
    ui->label_tab1_disp_num_4->setText(QString::number(value));
    if (!p_motor4_->getIsRunning()){
        double key = ui->qcp_motor_cur_4->xAxis->range().upper;
        double disp_num = value;
        ui->qcp_motor_cur_4->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_4->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_4->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_cur_4->replot();
        ui->qcp_motor_act_tor_4->replot();
        ui->qcp_motor_spd_4->replot();
    }
}
void MainWindow::on_tab1_horizontalSlider_5_valueChanged(int value)
{
    ui->label_tab1_disp_num_5->setText(QString::number(value));
    if (!p_motor5_->getIsRunning()){
        double key = ui->qcp_motor_cur_5->xAxis->range().upper;
        double disp_num = value;
        ui->qcp_motor_cur_5->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_5->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_5->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_cur_5->replot();
        ui->qcp_motor_act_tor_5->replot();
        ui->qcp_motor_spd_5->replot();
    }
}
void MainWindow::on_tab1_horizontalSlider_6_valueChanged(int value)
{
    ui->label_tab1_disp_num_6->setText(QString::number(value));
    if (!p_motor6_->getIsRunning()){
        double key = ui->qcp_motor_cur_6->xAxis->range().upper;
        double disp_num = value;
        ui->qcp_motor_cur_6->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_spd_6->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_act_tor_6->xAxis->setRange(key, disp_num/20, Qt::AlignRight);
        ui->qcp_motor_cur_6->replot();
        ui->qcp_motor_act_tor_6->replot();
        ui->qcp_motor_spd_6->replot();
    }
}

//一键测试模式启动按钮
void MainWindow::on_pushButton_auto_test_noair_power_1_clicked()
{
    static bool this_mode_running = false;
    if (!m_sys_status_1_){
        QMessageBox::warning(this,"警告","飞轮电源未打开！");
    }
    else{
        if(!p_motor1_->getIsRunning() && !this_mode_running){
            //如果选择了该模式，启动测试流程
            p_motor1_->setIsRunning(true);
            this_mode_running = true;
            m_timer_get_data_.start();//打开定时器
            ui->pushButton_auto_test_noair_power_1->setText("停止");

            //筛选选中的测试模式---在这里添加更多的
            QVector<QString> selected_test_mode;
            if (ui->checkBox_hs_mode_1->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_SPD));

            if (ui->checkBox_lj_mode_1->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_TOR));

            if (ui->checkBox_xp_mode_1->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_XP));

            if (ui->checkBox_dl_mode_1->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_DL));

            emit startAutoTestModeNoAir(selected_test_mode);
            connect(p_motor1_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_1_clicked()));
        }
        else if (this_mode_running){


            //p_motor1_->setIsRunning(false);
            p_motor1_->setIsRunning(false);
            this_mode_running = false;
            //p_motor1_->setHXMode(false);
            p_motor1_->setAutoTestSpdMode(false);
            p_motor1_->setAutoTestTorMode(false);
            p_motor1_->setAutoTestCurMode(false);
            //            m_timer_get_data_.stop();//关闭定时器
            ui->pushButton_auto_test_noair_power_1->setText("启动");
            ui->lineEdit_AutoTest_StatusDisplay_1->setText("一键测试完成");
            ui->statusBar->showMessage("飞轮1真空性能测试结束！",5000);
            disconnect(p_motor1_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_1_clicked()));
            emit getLastExpData(tr(""),tr("MOTOR1"),tr(""));//生成报表的触发信号
            p_motor1_->setSetSpeed(0);//这个可能有问题
            QMessageBox::warning(this,"完成","飞轮1真空性能测试完成！");

        }
    }
}
void MainWindow::on_pushButton_auto_test_noair_power_2_clicked()
{
    static bool this_mode_running = false;
    if (!m_sys_status_2_){
        QMessageBox::warning(this,"警告","飞轮电源未打开！");
    }
    else{
        if(!p_motor2_->getIsRunning() && !this_mode_running){
            //如果选择了该模式，启动测试流程
            p_motor2_->setIsRunning(true);
            this_mode_running = true;
            m_timer_get_data_2.start();//打开定时器
            ui->pushButton_auto_test_noair_power_2->setText("停止");

            //筛选选中的测试模式---在这里添加更多的
            QVector<QString> selected_test_mode;
            if (ui->checkBox_hs_mode_2->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_SPD));

            if (ui->checkBox_lj_mode_2->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_TOR));

            if (ui->checkBox_xp_mode_2->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_XP));

            if (ui->checkBox_dl_mode_2->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_DL));

            emit startAutoTestModeNoAir2(selected_test_mode);
            connect(p_motor2_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_2_clicked()));
        }
        else if (this_mode_running){

            p_motor2_->setIsRunning(false);
            this_mode_running = false;
            //p_motor2_->setHXMode(false);
            p_motor2_->setAutoTestSpdMode(false);
            p_motor2_->setAutoTestTorMode(false);
            p_motor2_->setAutoTestCurMode(false);

            //            m_timer_get_data_2.stop();//关闭定时器
            ui->pushButton_auto_test_noair_power_2->setText("启动");
            ui->lineEdit_AutoTest_StatusDisplay_2->setText("一键测试完成");
            ui->statusBar->showMessage("飞轮2真空性能测试结束！",5000);
            disconnect(p_motor2_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_2_clicked()));
            emit getLastExpData(tr(""),tr("MOTOR2"),tr(""));//生成报表的触发信号
            p_motor2_->setSetSpeed(0);
            QMessageBox::warning(this,"完成","飞轮2真空性能测试完成！");

        }
    }
}
void MainWindow::on_pushButton_auto_test_noair_power_3_clicked()
{
    static bool this_mode_running = false;
    if (!m_sys_status_3_){
        QMessageBox::warning(this,"警告","飞轮电源未打开！");
    }
    else{
        if(!p_motor3_->getIsRunning() && !this_mode_running){
            //如果选择了该模式，启动测试流程
            p_motor3_->setIsRunning(true);
            this_mode_running = true;
            m_timer_get_data_3.start();//打开定时器
            ui->pushButton_auto_test_noair_power_3->setText("停止");

            //筛选选中的测试模式---在这里添加更多的
            QVector<QString> selected_test_mode;
            if (ui->checkBox_hs_mode_3->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_SPD));

            if (ui->checkBox_lj_mode_3->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_TOR));

            if (ui->checkBox_xp_mode_3->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_XP));

            if (ui->checkBox_dl_mode_3->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_DL));

            emit startAutoTestModeNoAir3(selected_test_mode);
            connect(p_motor3_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_3_clicked()));
        }
        else if (this_mode_running){
            //p_motor3_->setSetTorque(0,0.853);

            p_motor3_->setIsRunning(false);
            this_mode_running = false;
            //p_motor3_->setHXMode(false);
            p_motor3_->setAutoTestSpdMode(false);
            p_motor3_->setAutoTestTorMode(false);
            p_motor3_->setAutoTestCurMode(false);
            //            m_timer_get_data_3.stop();//关闭定时器
            ui->pushButton_auto_test_noair_power_3->setText("启动");
            ui->lineEdit_AutoTest_StatusDisplay_3->setText("一键测试完成");
            ui->statusBar->showMessage("飞轮3真空性能测试结束！",5000);
            disconnect(p_motor3_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_3_clicked()));
            emit getLastExpData(tr(""),tr("MOTOR3"),tr(""));//生成报表的触发信号
            p_motor3_->setSetSpeed(0);
            QMessageBox::warning(this,"完成","飞轮3真空性能测试完成！");

        }
    }
}
void MainWindow::on_pushButton_auto_test_noair_power_4_clicked()
{
    static bool this_mode_running = false;
    if (!m_sys_status_4_){
        QMessageBox::warning(this,"警告","飞轮电源未打开！");
    }
    else{
        if(!p_motor4_->getIsRunning() && !this_mode_running){
            //如果选择了该模式，启动测试流程
            p_motor4_->setIsRunning(true);
            this_mode_running = true;
            m_timer_get_data_4.start();//打开定时器
            ui->pushButton_auto_test_noair_power_4->setText("停止");

            //筛选选中的测试模式---在这里添加更多的
            QVector<QString> selected_test_mode;
            if (ui->checkBox_hs_mode_4->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_SPD));

            if (ui->checkBox_lj_mode_4->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_TOR));

            if (ui->checkBox_xp_mode_4->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_XP));

            if (ui->checkBox_dl_mode_4->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_DL));

            emit startAutoTestModeNoAir4(selected_test_mode);
            connect(p_motor4_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_4_clicked()));
        }
        else if (this_mode_running){
            p_motor4_->setIsRunning(false);
            this_mode_running = false;
            //p_motor4_->setHXMode(false);
            p_motor4_->setAutoTestSpdMode(false);
            p_motor4_->setAutoTestTorMode(false);
            p_motor4_->setAutoTestCurMode(false);
            //            m_timer_get_data_4.stop();//关闭定时器
            ui->pushButton_auto_test_noair_power_4->setText("启动");
            ui->lineEdit_AutoTest_StatusDisplay_4->setText("一键测试完成");
            ui->statusBar->showMessage("飞轮4真空性能测试结束！",5000);
            disconnect(p_motor4_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_4_clicked()));
            emit getLastExpData(tr(""),tr("MOTOR4"),tr(""));//生成报表的触发信号
            p_motor4_->setSetSpeed(0);
            QMessageBox::warning(this,"完成","飞轮4真空性能测试完成！");

        }
    }
}
void MainWindow::on_pushButton_auto_test_noair_power_5_clicked()
{
    static bool this_mode_running = false;
    if (!m_sys_status_5_){
        QMessageBox::warning(this,"警告","飞轮电源未打开！");
    }
    else{
        if(!p_motor5_->getIsRunning() && !this_mode_running){
            //如果选择了该模式，启动测试流程
            p_motor5_->setIsRunning(true);
            this_mode_running = true;
            m_timer_get_data_5.start();//打开定时器
            ui->pushButton_auto_test_noair_power_5->setText("停止");

            //筛选选中的测试模式---在这里添加更多的
            QVector<QString> selected_test_mode;
            if (ui->checkBox_hs_mode_5->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_SPD));

            if (ui->checkBox_lj_mode_5->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_TOR));

            if (ui->checkBox_xp_mode_5->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_XP));

            if (ui->checkBox_dl_mode_5->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_DL));

            emit startAutoTestModeNoAir5(selected_test_mode);
            connect(p_motor5_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_5_clicked()));
        }
        else if (this_mode_running){
            p_motor5_->setIsRunning(false);
            this_mode_running = false;
            //p_motor5_->setHXMode(false);
            p_motor5_->setAutoTestSpdMode(false);
            p_motor5_->setAutoTestTorMode(false);
            p_motor5_->setAutoTestCurMode(false);
            //            m_timer_get_data_5.stop();//关闭定时器
            ui->pushButton_auto_test_noair_power_5->setText("启动");
            ui->lineEdit_AutoTest_StatusDisplay_5->setText("一键测试完成");
            ui->statusBar->showMessage("飞轮5真空性能测试结束！",5000);
            disconnect(p_motor5_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_5_clicked()));
            emit getLastExpData(tr(""),tr("MOTOR5"),tr(""));//生成报表的触发信号
            p_motor5_->setSetSpeed(0);
            QMessageBox::warning(this,"完成","飞轮5真空性能测试完成！");

        }
    }
}
void MainWindow::on_pushButton_auto_test_noair_power_6_clicked()
{
    static bool this_mode_running = false;
    if (!m_sys_status_6_){
        QMessageBox::warning(this,"警告","飞轮电源未打开！");
    }
    else{
        if(!p_motor6_->getIsRunning() && !this_mode_running){
            //如果选择了该模式，启动测试流程
            p_motor6_->setIsRunning(true);
            this_mode_running = true;
            m_timer_get_data_6.start();//打开定时器
            ui->pushButton_auto_test_noair_power_6->setText("停止");

            //筛选选中的测试模式---在这里添加更多的
            QVector<QString> selected_test_mode;
            if (ui->checkBox_hs_mode_6->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_SPD));

            if (ui->checkBox_lj_mode_6->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_TOR));

            if (ui->checkBox_xp_mode_6->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_XP));

            if (ui->checkBox_dl_mode_6->isChecked())
                selected_test_mode.push_back(QString::number(FLYWHEEL_MODE_DL));

            emit startAutoTestModeNoAir6(selected_test_mode);
            connect(p_motor6_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_6_clicked()));
        }
        else if (this_mode_running){//力矩清零
            p_motor6_->setIsRunning(false);
            this_mode_running = false;
            //p_motor6_->setHXMode(false);
            p_motor6_->setAutoTestSpdMode(false);
            p_motor6_->setAutoTestTorMode(false);
            p_motor6_->setAutoTestCurMode(false);

            //            m_timer_get_data_6.stop();//关闭定时器
            ui->pushButton_auto_test_noair_power_6->setText("启动");
            ui->lineEdit_AutoTest_StatusDisplay_6->setText("一键测试完成");
            ui->statusBar->showMessage("飞轮6真空性能测试结束！",5000);
            disconnect(p_motor6_,SIGNAL(autoTestEnd()),this,SLOT(on_pushButton_auto_test_noair_power_6_clicked()));
            emit getLastExpData(tr(""),tr("MOTOR6"),tr(""));//生成报表的触发信号
            p_motor6_->setSetSpeed(0);
            QMessageBox::warning(this,"完成","飞轮6真空性能测试完成！");

        }
    }
}




void MainWindow::on_pushButton_all_sys_power_clicked()
{//1.判断飞轮是否被勾选2.打开已选择的飞轮电源或直接调用其函数
    if(ui->checkBox_fw1_power->isChecked())
    {//发送打开fw1电源开关的信号
        on_pushButton_system_power_1_clicked();
    }
    if(ui->checkBox_fw2_power->isChecked())
    {//发送打开fw2电源开关的信号
        on_pushButton_system_power_2_clicked();
    }
    if(ui->checkBox_fw3_power->isChecked())
    {//发送打开fw3电源开关的信号
        on_pushButton_system_power_3_clicked();
    }
    if(ui->checkBox_fw4_power->isChecked())
    {//发送打开fw4电源开关的信号
        on_pushButton_system_power_4_clicked();
    }
    if(ui->checkBox_fw5_power->isChecked())
    {//发送打开fw5电源开关的信号
        on_pushButton_system_power_5_clicked();
    }
    if(ui->checkBox_fw6_power->isChecked())
    {//发送打开fw6电源开关的信号
        on_pushButton_system_power_6_clicked();
    }
}
void MainWindow::on_pushButton_all_power_cancel_clicked()
{
    if(ui->checkBox_fw1_power->isChecked())
    {//发送打开fw1电源开关的信号
        ui->checkBox_fw1_power->setCheckState(Qt::Unchecked);
        on_pushButton_system_power_1_clicked();
    }
    if(ui->checkBox_fw2_power->isChecked())
    {//发送打开fw2电源开关的信号
        ui->checkBox_fw2_power->setCheckState(Qt::Unchecked);
        on_pushButton_system_power_2_clicked();
    }
    if(ui->checkBox_fw3_power->isChecked())
    {//发送打开fw3电源开关的信号
        ui->checkBox_fw3_power->setCheckState(Qt::Unchecked);
        on_pushButton_system_power_3_clicked();
    }
    if(ui->checkBox_fw4_power->isChecked())
    {//发送打开fw4电源开关的信号
        ui->checkBox_fw4_power->setCheckState(Qt::Unchecked);
        on_pushButton_system_power_4_clicked();
    }
    if(ui->checkBox_fw5_power->isChecked())
    {//发送打开fw5电源开关的信号
        ui->checkBox_fw5_power->setCheckState(Qt::Unchecked);
        on_pushButton_system_power_5_clicked();
    }
    if(ui->checkBox_fw6_power->isChecked())
    {//发送打开fw6电源开关的信号
        ui->checkBox_fw6_power->setCheckState(Qt::Unchecked);
        on_pushButton_system_power_6_clicked();
    }
}
void MainWindow::on_pushButton_all_parameter_clicked()
{
    //FW1
    //j
    p_motor1_->setJ(ui->doubleSpinBox_fw1_J->text().toDouble());
    ui->doubleSpinBox_J_1->setValue(ui->doubleSpinBox_fw1_J->text().toDouble());
    //k
    p_motor1_->setK(ui->doubleSpinBox_fw1_K->text().toDouble());
    ui->doubleSpinBox_sin_K_1->setValue(ui->doubleSpinBox_fw1_K->text().toDouble());
    //a
    p_motor1_->setA(ui->doubleSpinBox_fw1_A->text().toDouble());
    ui->spinBox_sin_A_1->setValue(ui->doubleSpinBox_fw1_A->text().toDouble());
    //t
    p_motor1_->setT(ui->doubleSpinBox_fw1_T->text().toDouble());
    ui->spinBox_sin_T_1->setValue(ui->doubleSpinBox_fw1_A->text().toDouble());

    //lj
    p_can_driver_->setResolution_ratio(ui->doubleSpinBox_setting_lj->value());
    ui->doubleSpinBox_lj_resolution_ratio_1->setValue(ui->doubleSpinBox_setting_lj->value());

    //spd
    p_can_driver_->setSpdResolutionRatio(ui->doubleSpinBox_setting_spd->value());
    //cur
    p_can_driver_->setCurrentResolutionRatio(ui->doubleSpinBox_setting_cur->value());
    //calnum
    p_motor1_->setAverageNum((uint)ui->doubleSpinBox_setting_avgnum->value());


    //FW2
    //j
    p_motor2_->setJ(ui->doubleSpinBox_fw2_J->text().toDouble());
    ui->doubleSpinBox_J_2->setValue(ui->doubleSpinBox_fw2_J->text().toDouble());
    //k
    p_motor2_->setK(ui->doubleSpinBox_fw2_K->text().toDouble());
    ui->doubleSpinBox_sin_K_2->setValue(ui->doubleSpinBox_fw2_K->text().toDouble());
    //a
    p_motor2_->setA(ui->doubleSpinBox_fw2_A->text().toDouble());
    ui->spinBox_sin_A_2->setValue(ui->doubleSpinBox_fw2_A->text().toDouble());
    //t
    p_motor2_->setT(ui->doubleSpinBox_fw2_T->text().toDouble());
    ui->spinBox_sin_T_2->setValue(ui->doubleSpinBox_fw2_A->text().toDouble());

    //lj
    //    p_driver2_->setResolution_ratio(ui->doubleSpinBox_setting_lj_2->value());
    ui->doubleSpinBox_lj_resolution_ratio_2->setValue(ui->doubleSpinBox_setting_lj->value());

    //spd
    //    p_driver2_->setSpdResolutionRatio(ui->doubleSpinBox_setting_spd_2->value());
    //cur
    //    p_driver2_->setCurrentResolutionRatio(ui->doubleSpinBox_setting_cur_2->value());
    //calnum
    p_motor2_->setAverageNum((uint)ui->doubleSpinBox_setting_avgnum_2->value());

    //FW3
    //j
    p_motor3_->setJ(ui->doubleSpinBox_fw3_J->text().toDouble());
    ui->doubleSpinBox_J_3->setValue(ui->doubleSpinBox_fw3_J->text().toDouble());
    //k
    p_motor3_->setK(ui->doubleSpinBox_fw3_K->text().toDouble());
    ui->doubleSpinBox_sin_K_3->setValue(ui->doubleSpinBox_fw3_K->text().toDouble());
    //a
    p_motor3_->setA(ui->doubleSpinBox_fw3_A->text().toDouble());
    ui->spinBox_sin_A_3->setValue(ui->doubleSpinBox_fw3_A->text().toDouble());
    //t
    p_motor3_->setT(ui->doubleSpinBox_fw3_T->text().toDouble());
    ui->spinBox_sin_T_3->setValue(ui->doubleSpinBox_fw3_A->text().toDouble());

    //lj
    //    p_driver3_->setResolution_ratio(ui->doubleSpinBox_setting_lj_3->value());
    ui->doubleSpinBox_lj_resolution_ratio_3->setValue(ui->doubleSpinBox_setting_lj->value());

    //spd
    //    p_driver3_->setSpdResolutionRatio(ui->doubleSpinBox_setting_spd_3->value());
    //cur
    //    p_driver3_->setCurrentResolutionRatio(ui->doubleSpinBox_setting_cur_3->value());
    //calnum
    p_motor3_->setAverageNum((uint)ui->doubleSpinBox_setting_avgnum_3->value());

    //FW4
    //j
    p_motor4_->setJ(ui->doubleSpinBox_fw4_J->text().toDouble());
    ui->doubleSpinBox_J_4->setValue(ui->doubleSpinBox_fw4_J->text().toDouble());
    //k
    p_motor4_->setK(ui->doubleSpinBox_fw4_K->text().toDouble());
    ui->doubleSpinBox_sin_K_4->setValue(ui->doubleSpinBox_fw4_K->text().toDouble());
    //a
    p_motor4_->setA(ui->doubleSpinBox_fw4_A->text().toDouble());
    ui->spinBox_sin_A_4->setValue(ui->doubleSpinBox_fw4_A->text().toDouble());
    //t
    p_motor4_->setT(ui->doubleSpinBox_fw4_T->text().toDouble());
    ui->spinBox_sin_T_4->setValue(ui->doubleSpinBox_fw4_A->text().toDouble());

    //lj
    //    p_driver4_->setResolution_ratio(ui->doubleSpinBox_setting_lj_4->value());
    ui->doubleSpinBox_lj_resolution_ratio_4->setValue(ui->doubleSpinBox_setting_lj->value());

    //spd
    //    p_driver4_->setSpdResolutionRatio(ui->doubleSpinBox_setting_spd_4->value());
    //cur
    //    p_driver4_->setCurrentResolutionRatio(ui->doubleSpinBox_setting_cur_4->value());
    //calnum
    p_motor4_->setAverageNum((uint)ui->doubleSpinBox_setting_avgnum_4->value());

    //FW5
    //j
    p_motor5_->setJ(ui->doubleSpinBox_fw5_J->text().toDouble());
    ui->doubleSpinBox_J_5->setValue(ui->doubleSpinBox_fw5_J->text().toDouble());
    //k
    p_motor5_->setK(ui->doubleSpinBox_fw5_K->text().toDouble());
    ui->doubleSpinBox_sin_K_5->setValue(ui->doubleSpinBox_fw5_K->text().toDouble());
    //a
    p_motor5_->setA(ui->doubleSpinBox_fw5_A->text().toDouble());
    ui->spinBox_sin_A_5->setValue(ui->doubleSpinBox_fw5_A->text().toDouble());
    //t
    p_motor5_->setT(ui->doubleSpinBox_fw5_T->text().toDouble());
    ui->spinBox_sin_T_5->setValue(ui->doubleSpinBox_fw5_A->text().toDouble());

    //lj
    //    p_driver5_->setResolution_ratio(ui->doubleSpinBox_setting_lj_5->value());
    ui->doubleSpinBox_lj_resolution_ratio_5->setValue(ui->doubleSpinBox_setting_lj->value());

    //spd
    //    p_driver5_->setSpdResolutionRatio(ui->doubleSpinBox_setting_spd_5->value());
    //cur
    //    p_driver5_->setCurrentResolutionRatio(ui->doubleSpinBox_setting_cur_5->value());
    //calnum
    p_motor5_->setAverageNum((uint)ui->doubleSpinBox_setting_avgnum_5->value());

    //FW6
    //j
    p_motor6_->setJ(ui->doubleSpinBox_fw6_J->text().toDouble());
    ui->doubleSpinBox_J_6->setValue(ui->doubleSpinBox_fw6_J->text().toDouble());
    //k
    p_motor6_->setK(ui->doubleSpinBox_fw6_K->text().toDouble());
    ui->doubleSpinBox_sin_K_6->setValue(ui->doubleSpinBox_fw6_K->text().toDouble());
    //a
    p_motor6_->setA(ui->doubleSpinBox_fw6_A->text().toDouble());
    ui->spinBox_sin_A_6->setValue(ui->doubleSpinBox_fw6_A->text().toDouble());
    //t
    p_motor6_->setT(ui->doubleSpinBox_fw6_T->text().toDouble());
    ui->spinBox_sin_T_6->setValue(ui->doubleSpinBox_fw6_A->text().toDouble());

    //lj
    //    p_driver6_->setResolution_ratio(ui->doubleSpinBox_setting_lj_6->value());
    ui->doubleSpinBox_lj_resolution_ratio_6->setValue(ui->doubleSpinBox_setting_lj->value());

    //spd
    //    p_driver6_->setSpdResolutionRatio(ui->doubleSpinBox_setting_spd_6->value());
    //cur
    //    p_driver6_->setCurrentResolutionRatio(ui->doubleSpinBox_setting_cur_6->value());
    //calnum
    p_motor6_->setAverageNum((uint)ui->doubleSpinBox_setting_avgnum_6->value());


    //savepath
    p_repoter_->setSavePath(ui->label_setting_save_path->text());

    saveConfigFile();

    QMessageBox::warning(this,"设置成功！","设置参数成功，请重启以生效！");
}
void MainWindow::on_pushButton_all_parameter_cancel_clicked()
{   //fw1
    //    ui->doubleSpinBox_fw1_J->clear();
    //    ui->doubleSpinBox_fw1_K->clear();
    //    ui->doubleSpinBox_fw1_A->clear();
    //    ui->doubleSpinBox_fw1_T->clear();
    //    //fw2
    //    ui->doubleSpinBox_fw2_J->clear();
    //    ui->doubleSpinBox_fw2_K->clear();
    //    ui->doubleSpinBox_fw2_A->clear();
    //    ui->doubleSpinBox_fw2_T->clear();
    //    //fw3
    //    ui->doubleSpinBox_fw3_J->clear();
    //    ui->doubleSpinBox_fw3_K->clear();
    //    ui->doubleSpinBox_fw3_A->clear();
    //    ui->doubleSpinBox_fw3_T->clear();
    //    //fw4
    //    ui->doubleSpinBox_fw4_J->clear();
    //    ui->doubleSpinBox_fw4_K->clear();
    //    ui->doubleSpinBox_fw4_A->clear();
    //    ui->doubleSpinBox_fw4_T->clear();
    //    //fw5
    //    ui->doubleSpinBox_fw5_J->clear();
    //    ui->doubleSpinBox_fw5_K->clear();
    //    ui->doubleSpinBox_fw5_A->clear();
    //    ui->doubleSpinBox_fw5_T->clear();
    //    //fw6
    //    ui->doubleSpinBox_fw6_J->clear();
    //    ui->doubleSpinBox_fw6_K->clear();
    //    ui->doubleSpinBox_fw6_A->clear();
    //    ui->doubleSpinBox_fw6_T->clear();
    QMessageBox::warning(this,"cancel！","请重启软件！");
}

void MainWindow::displayStatus1(int dst)
{
    if(dst == 0){
        ui->lineEdit_AutoTest_StatusDisplay_1->setText("正在进行速度测试");
        qDebug()<<"速度测试";
    }
    else if(dst == 1){
        ui->lineEdit_AutoTest_StatusDisplay_1->setText("正在进行力矩测试");
        qDebug()<<"力矩测试";
    }
    else if(dst == 2){
        ui->lineEdit_AutoTest_StatusDisplay_1->setText("正在进行斜坡测试");
        qDebug()<<"斜坡测试";
    }
    else if(dst == 3){
        ui->lineEdit_AutoTest_StatusDisplay_1->setText("正在进行电流测试");
         qDebug()<<"电流测试";
    }
}
void MainWindow::displayStatus2(int dst)
{
    if(dst == 0){
        ui->lineEdit_AutoTest_StatusDisplay_2->setText("正在进行速度测试");
        qDebug()<<"速度测试";
    }
    else if(dst == 1){
        ui->lineEdit_AutoTest_StatusDisplay_2->setText("正在进行力矩测试");
        qDebug()<<"力矩测试";
    }
    else if(dst == 2){
        ui->lineEdit_AutoTest_StatusDisplay_2->setText("正在进行斜坡测试");
        qDebug()<<"斜坡测试";
    }
    else if(dst == 3){
        ui->lineEdit_AutoTest_StatusDisplay_2->setText("正在进行电流测试");
         qDebug()<<"电流测试";
    }


}
void MainWindow::displayStatus3(int dst)
{
    if(dst == 0){
        ui->lineEdit_AutoTest_StatusDisplay_3->setText("正在进行速度测试");
        qDebug()<<"速度测试";
    }
    else if(dst == 1){
        ui->lineEdit_AutoTest_StatusDisplay_3->setText("正在进行力矩测试");
        qDebug()<<"力矩测试";
    }
    else if(dst == 2){
        ui->lineEdit_AutoTest_StatusDisplay_3->setText("正在进行斜坡测试");
        qDebug()<<"斜坡测试";
    }
    else if(dst == 3){
        ui->lineEdit_AutoTest_StatusDisplay_3->setText("正在进行电流测试");
         qDebug()<<"电流测试";
    }


}
void MainWindow::displayStatus4(int dst)
{
    if(dst == 0){
        ui->lineEdit_AutoTest_StatusDisplay_4->setText("正在进行速度测试");
        qDebug()<<"速度测试";
    }
    else if(dst == 1){
        ui->lineEdit_AutoTest_StatusDisplay_4->setText("正在进行力矩测试");
        qDebug()<<"力矩测试";
    }
    else if(dst == 2){
        ui->lineEdit_AutoTest_StatusDisplay_4->setText("正在进行斜坡测试");
        qDebug()<<"斜坡测试";
    }
    else if(dst == 3){
        ui->lineEdit_AutoTest_StatusDisplay_4->setText("正在进行电流测试");
         qDebug()<<"电流测试";
    }


}
void MainWindow::displayStatus5(int dst)
{
    if(dst == 0){
        ui->lineEdit_AutoTest_StatusDisplay_5->setText("正在进行速度测试");
        qDebug()<<"速度测试";
    }
    else if(dst == 1){
        ui->lineEdit_AutoTest_StatusDisplay_5->setText("正在进行力矩测试");
        qDebug()<<"力矩测试";
    }
    else if(dst == 2){
        ui->lineEdit_AutoTest_StatusDisplay_5->setText("正在进行斜坡测试");
        qDebug()<<"斜坡测试";
    }
    else if(dst == 3){
        ui->lineEdit_AutoTest_StatusDisplay_5->setText("正在进行电流测试");
         qDebug()<<"电流测试";
    }


}
void MainWindow::displayStatus6(int dst)
{
    if(dst == 0){
        ui->lineEdit_AutoTest_StatusDisplay_6->setText("正在进行速度测试");
        qDebug()<<"速度测试";
    }
    else if(dst == 1){
        ui->lineEdit_AutoTest_StatusDisplay_6->setText("正在进行力矩测试");
        qDebug()<<"力矩测试";
    }
    else if(dst == 2){
        ui->lineEdit_AutoTest_StatusDisplay_6->setText("正在进行斜坡测试");
        qDebug()<<"斜坡测试";
    }
    else if(dst == 3){
        ui->lineEdit_AutoTest_StatusDisplay_6->setText("正在进行电流测试");
         qDebug()<<"电流测试";
    }


}
void MainWindow::processDisplay(int val)
{
    if(p_pro_diag_ == nullptr){
        p_pro_diag_ = new QProgressDialog("正在生成测试报告EXCEL....请稍后！","请稍后",0,100,this);
        p_pro_diag_->exec();



    }
    else{
        if(val == 100){
            if(p_pro_diag_ != nullptr){
                delete p_pro_diag_;
                p_pro_diag_ = nullptr;
            }
        }
        else{
            if(p_pro_diag_->wasCanceled())
                emit stopCreateExcel();


            p_pro_diag_->setValue(val);
        }

    }

}

void MainWindow::on_pushButton_exportExecel_clicked()
{
    //    p_sql_->getExpDataFromSqlDB2(ui->lineEdit_sql_motor_id->text(),ui->lineEdit_sql_exp_id->text()
    //                                  ,QString::number(ui->comboBox_sql_motor_mode->currentIndex()),ui->dateTimeEdit_start_time->dateTime().toString("yyyy-MM-dd hh::mm::ss"),ui->dateTimeEdit_end_time->dateTime().toString("yyyy-MM-dd hh::mm::ss"));
    emit sendQueryExcelToDB(ui->lineEdit_sql_motor_id->text(),ui->lineEdit_sql_exp_id->text()
                            ,QString::number(ui->comboBox_sql_motor_mode->currentIndex()),ui->dateTimeEdit_start_time->dateTime().toString("yyyy-MM-dd hh:mm:ss"),ui->dateTimeEdit_end_time->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
}



//void MainWindow::on_doubleSpinBox_lj_resolution_ratio_1_valueChanged(double arg1)
//{
//    p_can_driver_->setResolution_ratio(arg1);
//    ui->doubleSpinBox_lj_resolution_ratio_2->setValue(arg1);
//    ui->doubleSpinBox_lj_resolution_ratio_3->setValue(arg1);
//    ui->doubleSpinBox_lj_resolution_ratio_4->setValue(arg1);
//    ui->doubleSpinBox_lj_resolution_ratio_5->setValue(arg1);
//    ui->doubleSpinBox_lj_resolution_ratio_6->setValue(arg1);
//}

//void MainWindow::on_doubleSpinBox_lj_resolution_ratio_2_valueChanged(double arg2)
//{
//    p_can_driver_->setResolution_ratio(arg2);
//    ui->doubleSpinBox_lj_resolution_ratio_1->setValue(arg2);
//    ui->doubleSpinBox_lj_resolution_ratio_3->setValue(arg2);
//    ui->doubleSpinBox_lj_resolution_ratio_4->setValue(arg2);
//    ui->doubleSpinBox_lj_resolution_ratio_5->setValue(arg2);
//    ui->doubleSpinBox_lj_resolution_ratio_6->setValue(arg2);
//}

//void MainWindow::on_doubleSpinBox_lj_resolution_ratio_3_valueChanged(double arg3)
//{
//    p_can_driver_->setResolution_ratio(arg3);
//    ui->doubleSpinBox_lj_resolution_ratio_1->setValue(arg3);
//    ui->doubleSpinBox_lj_resolution_ratio_2->setValue(arg3);
//    ui->doubleSpinBox_lj_resolution_ratio_4->setValue(arg3);
//    ui->doubleSpinBox_lj_resolution_ratio_5->setValue(arg3);
//    ui->doubleSpinBox_lj_resolution_ratio_6->setValue(arg3);
//}

//void MainWindow::on_doubleSpinBox_lj_resolution_ratio_4_valueChanged(double arg4)
//{
//    p_can_driver_->setResolution_ratio(arg4);
//    ui->doubleSpinBox_lj_resolution_ratio_1->setValue(arg4);
//    ui->doubleSpinBox_lj_resolution_ratio_2->setValue(arg4);
//    ui->doubleSpinBox_lj_resolution_ratio_3->setValue(arg4);
//    ui->doubleSpinBox_lj_resolution_ratio_5->setValue(arg4);
//    ui->doubleSpinBox_lj_resolution_ratio_6->setValue(arg4);
//}

//void MainWindow::on_doubleSpinBox_lj_resolution_ratio_5_valueChanged(double arg5)
//{
//    p_can_driver_->setResolution_ratio(arg5);
//    ui->doubleSpinBox_lj_resolution_ratio_1->setValue(arg5);
//    ui->doubleSpinBox_lj_resolution_ratio_2->setValue(arg5);
//    ui->doubleSpinBox_lj_resolution_ratio_3->setValue(arg5);
//    ui->doubleSpinBox_lj_resolution_ratio_4->setValue(arg5);
//    ui->doubleSpinBox_lj_resolution_ratio_6->setValue(arg5);
//}
//void MainWindow::on_doubleSpinBox_lj_resolution_ratio_6_valueChanged(double arg6)
//{
//    p_can_driver_->setResolution_ratio(arg6);
//    ui->doubleSpinBox_lj_resolution_ratio_1->setValue(arg6);
//    ui->doubleSpinBox_lj_resolution_ratio_2->setValue(arg6);
//    ui->doubleSpinBox_lj_resolution_ratio_3->setValue(arg6);
//    ui->doubleSpinBox_lj_resolution_ratio_4->setValue(arg6);
//    ui->doubleSpinBox_lj_resolution_ratio_5->setValue(arg6);
//}

void MainWindow::disp_hx_end(int id, double time)
{
    QMessageBox::warning(this,"滑行结束","飞轮"+QString::number(id)+"滑行结束，滑行时间为"+QString::number(time)+"s");
}


void MainWindow::on_pushButton_setting_select_path_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "choose src Directory","/");
    ui->label_setting_save_path->setText(path+ "/");
}

void MainWindow::solveCanData(uint id, QVector<uchar> data)
{
    if (data.size() == 8){
        //此帧含有速度、电流
        m_spd_union.val = 0;

        m_spd_union.data[0] = 0x0;
        m_spd_union.data[1] = data[2];
        m_spd_union.data[2] = data[1];
        m_spd_union.data[3] = data[0];

        //        m_spd.val = 1;
        m_spd_union.val = m_spd_union.val>>8;
        float spd = m_spd_union.val * p_can_driver_->getSpdResolutionRatio();

        float cur = ((data[3] << 8) | data[4]) * p_can_driver_->getCurrentResolutionRatio();


        if (id == p_motor1_->getID()){
            p_motor1_->setSpeed(spd);
            p_motor1_->setCurrent(cur);
        }
        else if (id == p_motor2_->getID()){
            p_motor2_->setSpeed(spd);
            p_motor2_->setCurrent(cur);
        }
        else if (id == p_motor3_->getID()){
            p_motor3_->setSpeed(spd);
            p_motor3_->setCurrent(cur);
        }
        else if (id == p_motor4_->getID()){
            p_motor4_->setSpeed(spd);
            p_motor4_->setCurrent(cur);
        }
        else if (id == p_motor5_->getID()){
            p_motor5_->setSpeed(spd);
            p_motor5_->setCurrent(cur);
        }
        else if (id == p_motor6_->getID()){
            p_motor6_->setSpeed(spd);
            p_motor6_->setCurrent(cur);
        }
        else{
            logMsg(tr("接收到不属于任何飞轮的CAN信息！编号为%1").arg(QString::number(id)));
        }
    }
}

void MainWindow::on_doubleSpinBox_lj_resolution_ratio_1_editingFinished()
{
    p_can_driver_->setResolution_ratio(ui->doubleSpinBox_lj_resolution_ratio_1->text().toDouble());
}



//void MainWindow::on_doubleSpinBox_motor_test_cur_1_editingFinished()
//{


//}

void MainWindow::on_doubleSpinBox_motor_test_cur_2_editingFinished()
{
    if(p_motor2_->getIsRunning()){
        switch (ui->comboBox_motor_test_mode_2->currentIndex()) {
        case 7: p_motor2_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_2->text().toDouble());break;
        default:
            break;
        }
    }
}

void MainWindow::on_doubleSpinBox_motor_test_cur_3_editingFinished()
{
    if(p_motor3_->getIsRunning()){
        switch (ui->comboBox_motor_test_mode_3->currentIndex()) {
        case 7: p_motor3_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_3->text().toDouble());break;
        default:
            break;
        }
    }
}
void MainWindow::on_doubleSpinBox_motor_test_cur_4_editingFinished()
{
    if(p_motor4_->getIsRunning()){
        switch (ui->comboBox_motor_test_mode_4->currentIndex()) {
        case 7: p_motor4_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_4->text().toDouble());break;
        default:
            break;
        }
    }
}
void MainWindow::on_doubleSpinBox_motor_test_cur_5_editingFinished()
{
    if(p_motor5_->getIsRunning()){
        switch (ui->comboBox_motor_test_mode_5->currentIndex()) {
        case 7: p_motor5_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_5->text().toDouble());break;
        default:
            break;
        }
    }
}
void MainWindow::on_doubleSpinBox_motor_test_cur_6_editingFinished()
{
    if(p_motor6_->getIsRunning()){
        switch (ui->comboBox_motor_test_mode_6->currentIndex()) {
        case 7: p_motor6_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_6->text().toDouble());break;
        default:
            break;
        }
    }
}

void MainWindow::on_doubleSpinBox_motor_test_cur_1_editingFinished()
{
    if(p_motor1_->getIsRunning()){
        switch (ui->comboBox_motor_test_mode_1->currentIndex()) {
        case 7: p_motor1_->setSetCurrent(ui->doubleSpinBox_motor_test_cur_1->text().toDouble());break;
        default:
            break;
        }
    }
}

void MainWindow::on_pushButton_read_spd_from_excel_clicked()
{
    if (!m_sys_status_1_){
        QMessageBox::warning(this,"警告","飞轮电源未打开！");
        return;
    }
    if(p_motor1_->getIsRunning()){
            QMessageBox::warning(this,"警告","其它模式运行中。");
            return;
    }
    if(ui->pushButton_start_cycle_spd->isEnabled())
        ui->pushButton_start_cycle_spd->setEnabled(false);
    if(!excel){
        excel = new ReadExcel;
    }
    connect(&mSendSpdListTimer, &QTimer::timeout, this, &MainWindow::sendSpeedListSlot,Qt::UniqueConnection);
    connect(excel, &ReadExcel::readDataFinished, this, &MainWindow::setButtonEnabled,Qt::UniqueConnection);
    connect(this, &MainWindow::excelFileSelected, excel, &ReadExcel::readDataFromExcel,Qt::UniqueConnection);
    QString file = QFileDialog::getOpenFileName(this,"打开",QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),"Excel 文件(*.xls *.xlsx)");
    if(file.isEmpty())
        return;
    excel->setPath(file);
    ui->pushButton_read_spd_from_excel->setEnabled(false);
    ui->pushButton_read_spd_from_excel->setText("正在读取");
}

void MainWindow::on_pushButton_start_cycle_spd_clicked()
{
    if(!mIsCycleStart){
        if(p_motor1_->getIsRunning()){
                QMessageBox::warning(this,"警告","其它模式运行中。");
                return;
        }

        mSpeedList = excel->getSpeedSequence();
        if(mSpeedList.isEmpty()){
            QMessageBox::warning(this,"警告","读取的速度序列为空！");
            return;
        }
        mIsCycleStart = true;

        keyContainer.resize(0);
        //tmpContainer.resize(0);
        torContainer.resize(0);
        curContainer.resize(0);
        spdContainer.resize(0);
        setSpdContainer.resize(0);

        ui->pushButton_start_cycle_spd->setText("关闭发送");
        ui->pushButton_read_spd_from_excel->setText("读取速度");
        ui->pushButton_read_spd_from_excel->setEnabled(false);
        p_motor1_->setIsRunning(true);
        p_motor1_->setFlywheelMode(FLYWHEEL_MODE_SPD);
        m_timer_update_.setInterval(ui->doubleSpinBox_test_spd_interval->text().toDouble()*1000);
        p_motor1_->setCurrentInterval(ui->doubleSpinBox_test_spd_interval->text().toDouble());

        mSendSpdListTimer.start(250);//这个定时器开始循环发送
    }else{
        if(!mSpeedList.isEmpty())
            mSpeedList.clear();
        p_motor1_->setSetSpeed(0);
        p_motor1_->setIsRunning(false);
        ui->pushButton_start_cycle_spd->setText("开始发送");
        ui->pushButton_start_cycle_spd->setEnabled(false);
        ui->pushButton_read_spd_from_excel->setEnabled(true);
        m_timer_update_.setInterval(500);
        mIsCycleStart = false;
    }
}

void MainWindow::setButtonEnabled()
{
    ui->pushButton_read_spd_from_excel->setText("读取完毕");
    ui->pushButton_start_cycle_spd->setEnabled(true);
}

void MainWindow::sendSpeedListSlot()
{
    if(!mSpeedList.isEmpty()) {
        p_motor1_->setSetSpeed(mSpeedList.front());
        logMsg("剩余发送速度个数 = " + QString::number(mSpeedList.size()));
        mSpeedList.pop_front();
    }else{
        logMsg("data empty");
        mSendSpdListTimer.stop();
        p_motor1_->setSetSpeed(0);
        p_motor1_->setIsRunning(false);
        ui->pushButton_start_cycle_spd->setText("开始发送");
        ui->pushButton_start_cycle_spd->setEnabled(false);
        ui->pushButton_read_spd_from_excel->setEnabled(true);
        m_timer_update_.setInterval(ui->doubleSpinBox_moto_test_time_1->text().toDouble()*1000);
        p_motor1_->setCurrentInterval(ui->doubleSpinBox_moto_test_time_1->text().toDouble());

        mIsCycleStart = false;
        disconnect(excel, nullptr, nullptr, nullptr);
        if(excel){
            delete excel;
            excel = nullptr;
        }
    }
}


void MainWindow::delay(int ms)
{
    static QEventLoop eventloop;
    QTimer::singleShot(ms, &eventloop, SLOT(quit()));
    eventloop.exec();//wait for 500ms
}

