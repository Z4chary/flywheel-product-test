#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qstring.h>
#include <QThread>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QProgressDialog>
#include <QFile>
#include <QtSerialPort/QSerialPortInfo>
#include <QFileDialog>

#include "motor.h"
#include "motordriver.h"
#include "qcandriver.h"
#include "csqldatabase.h"
#include "qmotorreport.h"
#include "qdaqcard.h"
#include "readexcel.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool initDriver1();
    bool initDriver2();
    bool initDriver3();
    bool initDriver4();
    bool initDriver5();
    bool initDriver6();

    void initQCustomPlot1();
    void initQCustomPlot2();
    void initQCustomPlot3();
    void initQCustomPlot4();
    void initQCustomPlot5();
    void initQCustomPlot6();

    void updateMotor1Display();
    void updateMotor2Display();
    void updateMotor3Display();
    void updateMotor4Display();
    void updateMotor5Display();
    void updateMotor6Display();

    void initCombox();
    void initSql();             //数据库初始化
    void initReport();          //报表初始化
    void initDaqCard();         //数据采集卡初始化
    QVector<QString> makeSqlVector(Motor &);

    void InquireDisplay();

    //void currentProtect();

    //灵活加载参数相关3个函数
    void initSerialInfo();     //加载串口参数
    void initSettingTab();     //飞轮参数设置界面初始化
    void saveConfigFile();     //将飞轮参数保存到config file里

    bool checkIsMultiId();
    
    void delay(int ms);
signals:

    void stopCreateExcel();

    void sendQueryExcelToDB(QString,QString,QString,QString,QString);
    void sendQueryWordToDB(QString,QString,QString,QString,QString);

    void sendToSqlDB(QString,QString,QString,QString,QString,QString,QVector<QString>);
    void sendToSqlDB2(QString,QString,QString,QString,QString,QString,QVector<QString>);
    void sendToSqlDB3(QString,QString,QString,QString,QString,QString,QVector<QString>);
    void sendToSqlDB4(QString,QString,QString,QString,QString,QString,QVector<QString>);
    void sendToSqlDB5(QString,QString,QString,QString,QString,QString,QVector<QString>);
    void sendToSqlDB6(QString,QString,QString,QString,QString,QString,QVector<QString>);
    //请求从数据库中请求实验数据
    void getLastExpData(QString,QString,QString);
    //非真空getdata信号
    void sendCurrentSpdForAutoTest(double);
    void sendCurrentSpdForAutoTest2(double);
    void sendCurrentSpdForAutoTest3(double);
    void sendCurrentSpdForAutoTest4(double);
    void sendCurrentSpdForAutoTest5(double);
    void sendCurrentSpdForAutoTest6(double);

    //真空一键测试--发送需要测试的模式
    void startAutoTestModeNoAir(QVector<QString>);
    void startAutoTestModeNoAir2(QVector<QString>);
    void startAutoTestModeNoAir3(QVector<QString>);
    void startAutoTestModeNoAir4(QVector<QString>);
    void startAutoTestModeNoAir5(QVector<QString>);
    void startAutoTestModeNoAir6(QVector<QString>);
    //速度模式刷新信号
    void refreshAutoTestSpd();
    void refreshAutoTestSpd2();
    void refreshAutoTestSpd3();
    void refreshAutoTestSpd4();
    void refreshAutoTestSpd5();
    void refreshAutoTestSpd6();

    //力矩模式刷新信号
    void refreshAutoTestTor();
    void refreshAutoTestTor2();
    void refreshAutoTestTor3();
    void refreshAutoTestTor4();
    void refreshAutoTestTor5();
    void refreshAutoTestTor6();
    //电流模式刷新信号
    void refreshAutotestCur();
    void refreshAutotestCur2();
    void refreshAutotestCur3();
    void refreshAutotestCur4();
    void refreshAutotestCur5();
    void refreshAutotestCur6();
    //CAN 通信初始化信号
    void canRecovery(uint);
    //CAN 通信请求帧信号
    void getCanFrame(uint);

/********循环模式********/
    void excelFileSelected(QString file);
public slots:
    void logMsg(QString);
    void processDisplay(int);


private slots:
    void on_pushButton_system_power_1_clicked();
    void on_pushButton_system_power_2_clicked();
    void on_pushButton_system_power_3_clicked();
    void on_pushButton_system_power_4_clicked();
    void on_pushButton_system_power_5_clicked();
    void on_pushButton_system_power_6_clicked();

    void on_pushButton_single_test_mode_1_clicked();
    void on_pushButton_single_test_mode_2_clicked();
    void on_pushButton_single_test_mode_3_clicked();
    void on_pushButton_single_test_mode_4_clicked();
    void on_pushButton_single_test_mode_5_clicked();
    void on_pushButton_single_test_mode_6_clicked();

    void updateMotor();
    void updateMotor2();
    void updateMotor3();
    void updateMotor4();
    void updateMotor5();
    void updateMotor6();

    void on_doubleSpinBox_motor_test_spd_1_editingFinished();
    void on_doubleSpinBox_motor_test_spd_2_editingFinished();
    void on_doubleSpinBox_motor_test_spd_3_editingFinished();
    void on_doubleSpinBox_motor_test_spd_4_editingFinished();
    void on_doubleSpinBox_motor_test_spd_5_editingFinished();
    void on_doubleSpinBox_motor_test_spd_6_editingFinished();

    void on_comboBox_motor_test_mode_1_currentIndexChanged(int index);
    void on_comboBox_motor_test_mode_2_currentIndexChanged(int index);
    void on_comboBox_motor_test_mode_3_currentIndexChanged(int index);
    void on_comboBox_motor_test_mode_4_currentIndexChanged(int index);
    void on_comboBox_motor_test_mode_5_currentIndexChanged(int index);
    void on_comboBox_motor_test_mode_6_currentIndexChanged(int index);

    void refreshCustomPlotData1();
    void refreshCustomPlotData2();
    void refreshCustomPlotData3();
    void refreshCustomPlotData4();
    void refreshCustomPlotData5();
    void refreshCustomPlotData6();

    void on_doubleSpinBox_moto_test_time_1_valueChanged(double arg1);
    void on_doubleSpinBox_moto_test_time_2_valueChanged(double arg2);
    void on_doubleSpinBox_moto_test_time_3_valueChanged(double arg3);
    void on_doubleSpinBox_moto_test_time_4_valueChanged(double arg4);
    void on_doubleSpinBox_moto_test_time_5_valueChanged(double arg5);
    void on_doubleSpinBox_moto_test_time_6_valueChanged(double arg6);

    void on_pushButton_sql_query_clicked();

    void updataSqlTableView(QVector<QVector<QString> >);
    void on_previous_page_clicked();
    void on_next_page_clicked();

    void on_pushButton_make_report_clicked();
    //void exportExcel(QVector<QVector<QString> >);

    void setMotorDataFromDAQCard(QVector<double> res);

    void on_pushButton_ele_test_ly_mode_power_1_clicked();
    void on_pushButton_ele_test_ly_mode_power_2_clicked();
    void on_pushButton_ele_test_ly_mode_power_3_clicked();
    void on_pushButton_ele_test_ly_mode_power_4_clicked();
    void on_pushButton_ele_test_ly_mode_power_5_clicked();
    void on_pushButton_ele_test_ly_mode_power_6_clicked();

    void on_tab1_horizontalSlider_1_valueChanged(int value);
    void on_tab1_horizontalSlider_2_valueChanged(int value);
    void on_tab1_horizontalSlider_3_valueChanged(int value);
    void on_tab1_horizontalSlider_4_valueChanged(int value);
    void on_tab1_horizontalSlider_5_valueChanged(int value);
    void on_tab1_horizontalSlider_6_valueChanged(int value);

    void on_pushButton_auto_test_noair_power_1_clicked();
    void on_pushButton_auto_test_noair_power_2_clicked();
    void on_pushButton_auto_test_noair_power_3_clicked();
    void on_pushButton_auto_test_noair_power_4_clicked();
    void on_pushButton_auto_test_noair_power_5_clicked();
    void on_pushButton_auto_test_noair_power_6_clicked();

    void on_doubleSpinBox_J_1_valueChanged(double arg);
    void on_doubleSpinBox_J_2_valueChanged(double arg);
    void on_doubleSpinBox_J_3_valueChanged(double arg);
    void on_doubleSpinBox_J_4_valueChanged(double arg);
    void on_doubleSpinBox_J_5_valueChanged(double arg);
    void on_doubleSpinBox_J_6_valueChanged(double arg);


    void on_doubleSpinBox_test_interval_1_valueChanged(double arg1);
    void on_doubleSpinBox_test_interval_2_valueChanged(double arg1);
    void on_doubleSpinBox_test_interval_3_valueChanged(double arg1);
    void on_doubleSpinBox_test_interval_4_valueChanged(double arg1);
    void on_doubleSpinBox_test_interval_5_valueChanged(double arg1);   
    void on_doubleSpinBox_test_interval_6_valueChanged(double arg1);

    void on_pushButton_all_sys_power_clicked();

    void on_pushButton_all_power_cancel_clicked();

    void on_pushButton_all_parameter_clicked();

    void on_pushButton_exportExecel_clicked();

    void on_pushButton_all_parameter_cancel_clicked();

    void displayStatus1(int dst);
    void displayStatus2(int dst);
    void displayStatus3(int dst);
    void displayStatus4(int dst);
    void displayStatus5(int dst);
    void displayStatus6(int dst);
    //void displayTorStatus();





//    void on_doubleSpinBox_lj_resolution_ratio_1_valueChanged(double arg1);
//    void on_doubleSpinBox_lj_resolution_ratio_2_valueChanged(double arg2);
//    void on_doubleSpinBox_lj_resolution_ratio_3_valueChanged(double arg3);
    void on_pushButton_setting_select_path_clicked();

    void on_doubleSpinBox_lj_resolution_ratio_1_editingFinished();



    //    void on_doubleSpinBox_lj_resolution_ratio_4_valueChanged(double arg4);
//    void on_doubleSpinBox_lj_resolution_ratio_5_valueChanged(double arg5);
//    void on_doubleSpinBox_lj_resolution_ratio_6_valueChanged(double arg6);

    void disp_hx_end(int id,double time);//处理滑行结束信号

    void solveCanData(uint id,QVector<uchar> data);//解析can
   // void on_doubleSpinBox_motor_test_cur_1_editingFinished();
    void on_doubleSpinBox_motor_test_cur_1_editingFinished();
    void on_pushButton_read_spd_from_excel_clicked();


    void on_doubleSpinBox_motor_test_cur_2_editingFinished();
    void on_doubleSpinBox_motor_test_cur_3_editingFinished();
    void on_doubleSpinBox_motor_test_cur_4_editingFinished();
    void on_doubleSpinBox_motor_test_cur_5_editingFinished();
    void on_doubleSpinBox_motor_test_cur_6_editingFinished();


    void on_pushButton_start_cycle_spd_clicked();

    /********循环发送速度序列*********/
    void setButtonEnabled();
    void sendSpeedListSlot();
private:
    Ui::MainWindow *ui;
    QProgressDialog *p_pro_diag_ = nullptr;

    Motor* p_motor1_;
    Motor* p_motor2_;
    Motor* p_motor3_;
    Motor* p_motor4_;
    Motor* p_motor5_;
    Motor* p_motor6_;

    MotorDriver* p_driver1_;
    MotorDriver* p_driver2_;
    MotorDriver* p_driver3_;
    MotorDriver* p_driver4_;
    MotorDriver* p_driver5_;
    MotorDriver* p_driver6_;

    QCanDriver* p_can_driver_ = nullptr;
    //QCanDriver* p_can_driver1_ = nullptr;
    //QCanDriver* p_can_driver2_ = nullptr;
    //QCanDriver* p_can_driver3_ = nullptr;
    //QCanDriver* p_can_driver4_ = nullptr;
    //QCanDriver* p_can_driver5_ = nullptr;
    //QCanDriver* p_can_driver6_ = nullptr;


    union {
        uchar data[4];
        int val;
    }m_spd_union;

    QThread* p_driver_thread1_;
    QThread* p_driver_thread2_;
    QThread* p_driver_thread3_;
    QThread* p_driver_thread4_;
    QThread* p_driver_thread5_;
    QThread* p_driver_thread6_;

    QThread* p_motor_thread1_;
    QThread* p_motor_thread2_;
    QThread* p_motor_thread3_;
    QThread* p_motor_thread4_;
    QThread* p_motor_thread5_;
    QThread* p_motor_thread6_;
    //...etc


    bool m_sys_status_1_;
    bool m_sys_status_2_;
    bool m_sys_status_3_;
    bool m_sys_status_4_;
    bool m_sys_status_5_;
    bool m_sys_status_6_;
    //获取数据定时器
    QTimer m_timer_get_data_;    //控制发指令
    QTimer m_timer_get_data_2;
    QTimer m_timer_get_data_3;
    QTimer m_timer_get_data_4;
    QTimer m_timer_get_data_5;
    QTimer m_timer_get_data_6;
    //如果想要在其中一个模式更改进入该模式函数的时间，可以在这里加个定时器
    //如下
    //QTimer m_timer_get_data_XP;
    //发送控制指令定时器
    QTimer m_timer_update_;    //更新数据库
    QTimer m_timer_update_2;
    QTimer m_timer_update_3;
    QTimer m_timer_update_4;
    QTimer m_timer_update_5;
    QTimer m_timer_update_6;
    //界面刷新buffer
    QVector<double> keyContainer;
    QVector<double> tmpContainer;
    QVector<double> curContainer;
    QVector<double> spdContainer;
    QVector<double> setSpdContainer;
    QVector<double> torContainer;//力矩

    QVector<double> keyContainer_2;
    QVector<double> tmpContainer_2;
    QVector<double> curContainer_2;
    QVector<double> spdContainer_2;
    QVector<double> setSpdContainer_2;
    QVector<double> torContainer_2;//力矩

    QVector<double> keyContainer_3;
    QVector<double> tmpContainer_3;
    QVector<double> curContainer_3;
    QVector<double> spdContainer_3;
    QVector<double> setSpdContainer_3;
    QVector<double> torContainer_3;//力矩

    QVector<double> keyContainer_4;
    QVector<double> tmpContainer_4;
    QVector<double> curContainer_4;
    QVector<double> spdContainer_4;
    QVector<double> setSpdContainer_4;
    QVector<double> torContainer_4;//力矩

    QVector<double> keyContainer_5;
    QVector<double> tmpContainer_5;
    QVector<double> curContainer_5;
    QVector<double> spdContainer_5;
    QVector<double> setSpdContainer_5;
    QVector<double> torContainer_5;//力矩

    QVector<double> keyContainer_6;
    QVector<double> tmpContainer_6;
    QVector<double> curContainer_6;
    QVector<double> spdContainer_6;
    QVector<double> setSpdContainer_6;
    QVector<double> torContainer_6;//力矩

    //数据库部分
    SqlDataBase* p_sql_;
    QThread* p_sql_thread_;

    //查询显示
    QVector<QVector<QString>> query_dispResult_;
    uint tab_num_ = 0;
    uint number = 0;
    int page=1;
    uint m=0;

    //报表部分
    QMotorReport* p_repoter_;
    QThread* p_repoter_thread_;

    //数据采集卡部分
    QDaqcard* p_daqcard_;
    QThread* p_daqcard_thread_;

    //串口信息记录
    QString serial_channel_1;
    QString serial_baudrate_1;

    QString serial_channel_2;
    QString serial_baudrate_2;

    QString serial_channel_3;
    QString serial_baudrate_3;

    QString serial_channel_4;
    QString serial_baudrate_4;

    QString serial_channel_5;
    QString serial_baudrate_5;

    QString serial_channel_6;
    QString serial_baudrate_6;
/**********循环测试*********/
    ReadExcel *excel = nullptr;
    QList<double> mSpeedList;
    QTimer mSendSpdListTimer;
    bool mIsCycleStart = false;
};

#endif // MAINWINDOW_H
