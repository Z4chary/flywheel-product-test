#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include <QQueue>
#include <qmath.h>
#include <algorithm>
#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QMutexLocker>
#include <qdebug.h>

//计算常数定义
static const double J_ = 0.0;
static const double PI_ = 3.1415926;
static const double LIMIT_SET_CUR = 550;

//模式编号定义
enum FLYWHEEL_MODE{
    FLYWHEEL_MODE_SPD,          //速度
    FLYWHEEL_MODE_XP,           //斜坡
    FLYWHEEL_MODE_SIN,          //正弦模式
    FLYWHEEL_MODE_TOR,          //力矩
    FLYWHEEL_MODE_TIMECONSTANT, //时间常数
    FLYWHEEL_MODE_HX,            //滑行
    FLYWHEEL_MODE_SM,            //寿命模式
    FLYWHEEL_MODE_DL,           //电流

    FLYWHEEL_MODE_AUTOTESTNOAIR,//自动测试1
    FLYWHEEL_MODE_AUTOTEST2,    //自动测试2
    FLYWHEEL_MODE_AUTOTEST3,    //自动测试3


};
enum AUTO_TEST{
    AUTO_TEST_SPD,     //速度
    AUTO_TEST_TOR,    //力矩
    AUTO_TEST_XP,     //斜坡
    AUTO_TEST_DL      //电流
};

/*
 * 电机类
 * 如果后期发现时间效率达不到要求，部分计算算法可以优化
 *
 * @author LYC
 * @time 2019/07/15
 */
class MotorBasic : public QObject{

    Q_OBJECT

public:
    MotorBasic(): id_(0),vol_(0),cur_(0),
        spd_(0),acc_(0.1),setSpd_(0),torque_(0),
        setTorque_(0),setCurrent_(0),isRunning_(false),channel_("")
    {

    }


    ~MotorBasic(){

    }
signals:
    void sendMoTorSpd(double);
    //void sendMoTorSpd(double,double);
    void sendMoTorTor(double);

    void sendMoTorSpd2(uint,double);
    void sendMoTorTor2(uint,double);
    void sendMotorCur2(uint,double);//zzy
    void spdChanged(double);
    void sendErrorText(QString);
public slots:
    void setSetSpeed(const double spd){
        this->setSpd_ = spd;
        if(spd >= 9000){
            if (!with_id_){
                emit sendMoTorSpd(9000);
            }
            else{
                emit sendMoTorSpd2(id_,9000);
            }
        }else if(spd <= -9000){
            if (!with_id_){
                emit sendMoTorSpd(-9000);
            }
            else{
                emit sendMoTorSpd2(id_,-9000);
            }
        }else {
            //            qDebug()<<"setSpd_ ="<< setSpd_;
            if (!with_id_){
                emit sendMoTorSpd(setSpd_);
            }else{
                qDebug()<<id_;
                emit sendMoTorSpd2(id_,setSpd_);
            }
        }
    }
    void setSpeed(const double spd){
        this->spd_ = spd;
        emit spdChanged(spd);
    }
    //解析从串口读上的电流
    void setCurrent(const double cur){
        this->cur_ = cur;
    }
    //计算从数据采集卡的电压
    void setVoltage(const double vol){
        this->vol_ = vol;
    }
    //速度增量
    void setAccelerate(const double acc){
        this->acc_ = acc;
        emit sendMoTorSpd(acc_);
        //qDebug()<<"acc_ = "<< acc_;
    }
    void setId(const uint id){
        this->id_ = id;
    }
    void setIsRunning(const bool isRunning){
        this->isRunning_ = isRunning;
    }

    void setTorque(const double tor){

        this->torque_ = tor;

    }
    void setSetTorque(double setTor){

        mode_ = FLYWHEEL_MODE_TOR;
        this->setTorque_ = setTor;
        //        setTor *= 1000;
        //qDebug()<<"setSpd_"<<setSpd_;
        //qDebug()<<"setTorque_ = "<<setTorque_;
        if (!with_id_)
            emit sendMoTorTor(setTor);
        else
            emit sendMoTorTor2(id_,setTor);
    }

    void setSetCurrent(double cur){//设置电流模式的电流
        mode_ = FLYWHEEL_MODE_DL;
        this->setCurrent_ = cur;
        if(cur >= LIMIT_SET_CUR){
            emit sendMotorCur2(id_,550);
            emit sendErrorText("输入的电流大于"+QString::number(LIMIT_SET_CUR)+"A");
        }else if(cur <= -LIMIT_SET_CUR){
            emit sendMotorCur2(id_,-550);
            emit sendErrorText("输入的电流小于-"+QString::number(LIMIT_SET_CUR)+"A");
        }else {
            //            qDebug()<<"setSpd_ ="<< setSpd_;
            emit sendMotorCur2(id_, cur);
        }
    }

public:
    bool getWithID()const{
        return this->with_id_;
    }

    //设置速度
    double getSetSpeed() const{
        return this->setSpd_;
    }

    //实时速度
    double getSpeed() const{
        return this->spd_;
    }

    //电流
    double getCurrent() const{
        return this->cur_;
    }

    //电压
    double getVoltage() const{
        return this->vol_;
    }

    //ID

    uint getID() const{
        return this->id_;
    }
    //ACC
    double getAccelerate() const{
        return this->acc_;
    }

    //isrunning
    bool getIsRunning(){
        return isRunning_;
    }



    //torque
    double getSetTorque() const{
        return this->setTorque_;
    }

    double getTorque() const{
        return this->torque_;
    }

    double getSetCurrent() const{
        return setCurrent_;
    }
    //设置通道号,保存数据库时用
    QString getChannel()const{
        return this->channel_;
    }

    void setChannel(QString str){
        this->channel_ = str;
    }

    FLYWHEEL_MODE getFlywheelMode() const{
        return mode_;
    }


    void setFlywheelMode(FLYWHEEL_MODE mode){
        mode_ = mode;
    }

private:
    uint id_;
    double vol_;
    double cur_;//串口电流
    double spd_;
    double acc_;
    double setSpd_;

    double torque_;
    double setTorque_;
    double setCurrent_;//电流模式电流

    bool isRunning_;
    QString channel_;

    //模式代码
    FLYWHEEL_MODE mode_ = FLYWHEEL_MODE_SPD;

    bool with_id_ = true;

};

class Motor : public MotorBasic{

    Q_OBJECT

public:
    Motor():last_ten_cur_(0),
        last_ten_vol_(0),
        last_ten_spd_(0)
    {
        initXpMode(0,0);
        connect(this,SIGNAL(spdChanged(double)),this,SLOT(setLastTen(double)));

        //非真空测试数据初始化
        air_test_containor_raw_data_.append(0);
        air_test_containor_raw_data_.append(100);
        air_test_containor_raw_data_.append(500);
        air_test_containor_raw_data_.append(1000);
        air_test_containor_raw_data_.append(1500);
        air_test_containor_raw_data_.append(2000);
        air_test_containor_raw_data_.append(2500);
        air_test_containor_raw_data_.append(3000);
        air_test_containor_raw_data_.append(-3000);
        air_test_containor_raw_data_.append(-2500);
        air_test_containor_raw_data_.append(-2000);
        air_test_containor_raw_data_.append(-1500);
        air_test_containor_raw_data_.append(-1000);
        air_test_containor_raw_data_.append(-500);
        //一键测试，速度模式初始化1，3，5
        auto_test_spd_mode__containor_raw_data_first.append(0);
        auto_test_spd_mode__containor_raw_data_first.append(2);
        auto_test_spd_mode__containor_raw_data_first.append(10);
        auto_test_spd_mode__containor_raw_data_first.append(50);
        auto_test_spd_mode__containor_raw_data_first.append(100);
        auto_test_spd_mode__containor_raw_data_first.append(500);
        auto_test_spd_mode__containor_raw_data_first.append(1000);
        auto_test_spd_mode__containor_raw_data_first.append(2000);
        auto_test_spd_mode__containor_raw_data_first.append(3000);
        auto_test_spd_mode__containor_raw_data_first.append(4000);
        auto_test_spd_mode__containor_raw_data_first.append(5000);
        auto_test_spd_mode__containor_raw_data_first.append(6000);

        auto_test_spd_mode__containor_raw_data_first.append(-6000);
        auto_test_spd_mode__containor_raw_data_first.append(-5000);
        auto_test_spd_mode__containor_raw_data_first.append(-4000);
        auto_test_spd_mode__containor_raw_data_first.append(-3000);
        auto_test_spd_mode__containor_raw_data_first.append(-2000);
        auto_test_spd_mode__containor_raw_data_first.append(-1000);
        auto_test_spd_mode__containor_raw_data_first.append(-500);
        auto_test_spd_mode__containor_raw_data_first.append(-100);
        auto_test_spd_mode__containor_raw_data_first.append(-50);
        auto_test_spd_mode__containor_raw_data_first.append(-10);
        auto_test_spd_mode__containor_raw_data_first.append(-2);
        //一键测试，速度模式初始化2

        //一键测试，速度模式初始化4

        //一键测试，设定力矩模式初始化
        //以前
        //        auto_test_tor_mode__containor_raw_data_.append(3);
        //        auto_test_tor_mode__containor_raw_data_.append(-3);
        //        auto_test_tor_mode__containor_raw_data_.append(10);
        //        auto_test_tor_mode__containor_raw_data_.append(-10);
        //        auto_test_tor_mode__containor_raw_data_.append(15);
        //        auto_test_tor_mode__containor_raw_data_.append(-15);


        auto_test_tor_mode__containor_raw_data_.append(0.5);
        auto_test_tor_mode__containor_raw_data_.append(-0.5);
        auto_test_tor_mode__containor_raw_data_.append(1);
        auto_test_tor_mode__containor_raw_data_.append(-1);
        auto_test_tor_mode__containor_raw_data_.append(3);
        auto_test_tor_mode__containor_raw_data_.append(-3);
        auto_test_tor_mode__containor_raw_data_.append(10);
        auto_test_tor_mode__containor_raw_data_.append(-10);
        auto_test_tor_mode__containor_raw_data_.append(15);
        auto_test_tor_mode__containor_raw_data_.append(-15);
        auto_test_tor_mode__containor_raw_data_.append(20);
        auto_test_tor_mode__containor_raw_data_.append(-20);
        //一件测试电流模式zzy
        auto_test_cur_mode_container_raw_data_first.append(10);
        auto_test_cur_mode_container_raw_data_first.append(-10);
        auto_test_cur_mode_container_raw_data_first.append(50);
        auto_test_cur_mode_container_raw_data_first.append(-50);
        auto_test_cur_mode_container_raw_data_first.append(100);
        auto_test_cur_mode_container_raw_data_first.append(-100);
        auto_test_cur_mode_container_raw_data_first.append(200);
        auto_test_cur_mode_container_raw_data_first.append(-200);
        auto_test_cur_mode_container_raw_data_first.append(300);
        auto_test_cur_mode_container_raw_data_first.append(-300);
        auto_test_cur_mode_container_raw_data_first.append(500);
        auto_test_cur_mode_container_raw_data_first.append(-500);

        connect(this,SIGNAL(autoTestStart()),this,SLOT(selectTestMode()));
    }
    ~Motor(){

    }
    //得到数据采集卡的平均电压
    double getAveVoltage() const{
        return this->last_ten_vol_;
    }
    //得到数据采集卡的平均电流
    double getAveCurrent()const{
        return this->last_ten_cur_daqcard;
    }

    //得到温度
    double Temperature() const{
        return this->temperature_;
    }
    //得到功率
    double getWate() const{
        return this->wate_;
    }
    //500mN start
    //得到角动量
    double getAngularMomentum500() {
        QMutexLocker locker(&m_locker);
        return this->angular_momentum_;
    }
    //得到角动量最大常值偏差
    double getAngularMomentumConst500() {
        QMutexLocker locker(&m_locker);
        return this->jdl_max_const_d_;
    }
    //得到角动量最大动态偏差
    double getAngularMomentumDynamic500() {
        QMutexLocker locker(&m_locker);
        return this->jdl_max_dynamic_d_;
    }
    //500mN end


    //得到角动量
    double getAngularMomentum() const{
        return this->angular_momentum_;
    }
    //得到角动量常值偏差
    double getAngularMomentumConst() const{
        return this->angular_momentum_const_d;
    }
    //得到角动量动态偏差
    double getAngularMomentumDynamic() const{
        return this->angular_momentum_dynamic_d;
    }
    //得到平均力矩
    double getAveTorque() const{
        return this->AveTorque_;
    }

    //得到平均速度
    double getAverageSpd() const{
        return this->last_ten_spd_;
    }
    //得到期望平均力矩
    double getHopeAveTorque() const{
        return this->hopeAveTorque_;
    }
    //得到反作用力矩
    double getReactionMoment_() const{
        return this->reaction_moment_;
    }
    //得到时间常数
    double getTimeConstant() const{
        return this->timeconst;
    }
    //模式初始化
    //斜坡模式
    void initXpMode(double espd,double interval){
        setFlywheelMode(FLYWHEEL_MODE_XP);
        this->xp_end_spd_ = espd;
        this->xp_spd_interval_ = interval;    //转速增量
        this->xp_direction = 0;
        this->xp_status_ = false;
    }
    //速度正弦
    void initsdsin(double K,double A ,double T,int t){
        this->spd_sin_A = A;
        this->spd_sin_K = K;
        this->spd_sin_T = T;
        this->spd_time_ = t;
        this->spd_sin_status_ = false;
    }
    //滑行模式
    void initHXMode(double hxspd)
    {
        setFlywheelMode(FLYWHEEL_MODE_HX);
        is_hx_mode_init = true;
        reach_target_ = false;
        hx_mode_target_spd_ = hxspd;
        hx_finish = false;
        hx_time.restart();
        start_hx_timer = false;
        // connect(p_motor1_,&Motor::sendMoTorSpd,p_driver1_,&MotorDriver::ctlMotorSpd);
        // connect(this,&Motor::sendMoTorHX,)
    }
    double getJ() const{
        return this->J_;
    }


    //设置角动量
    void setJ(const double J){
        this->J_ = J;
        //qDebug()<<"J_ = "<<J_;

    }


    //设置正弦参数
    void setK(const double K){
        this->spd_sin_K = K;
    }
    void setA(const double A){
        this->spd_sin_A = A;
    }
    void setT(const double T){
        this->spd_sin_T = T;
    }

    //设置当前间隔
    void setCurrentInterval(const double interval){
        this->current_interval = interval;
    }

    double getCurrentInterval()
    {
        return  this->current_interval;
    }

    //设置斜坡计数count
    void setXpCount(const int count){
        this->count_ = count;
    }
    void setXpStatus(bool status){
        this->xp_status_ = status;
    }
    bool getXpStatus() const{
        return this->xp_status_;
    }
    void setsdsinStatus(bool status){
        this->spd_sin_status_ = status;
    }
    bool getsdsinStatus() const{
        return this->spd_sin_status_;
    }
    void settimeconstant(bool status){
        this->time_constant_= status;
    }
    bool gettimeconstant() const{
        return this->time_constant_;
    }

    bool getAirMode() const{
        return this->is_air_init_;
    }
    void setAirMode(bool mode){
        this->is_air_init_ = mode;
    }
    QString getExpId() const{
        return this->exp_id_;
    }

    //从外部得到当前非真空测试项目
    QList<double> getNoAirTestUnit() const;
    //读取采集到的电流
    double getActCur()const{
        return this->act_cur_;
    }

public slots:
    //寿命模式初始化
    void initSMMode(double spd,double time){
        sm_mode_target_spd_ = spd;
        sm_mode_elapse = time;
        sm_reach_target_ = 0;
    }
    //寿命模式实现
    void calSMModeSpd(){
        if (sm_reach_target_ == 0){
            //等待到达1k转
            this->setSetSpeed(1000);
            if (qAbs(this->getSpeed() - 1000) < 5){
                sm_reach_target_ = 1;
                sm_mode_timer.restart();
            }

        }
        else if (sm_reach_target_ == 1){
            // 等待到达6k
            this->setSetSpeed(sm_mode_target_spd_);
            if (qAbs(this->getSpeed() - sm_mode_target_spd_) < 5){
                sm_reach_target_ = 2;
            }
        }
        else if (sm_reach_target_ == 2){
            // 等待到达-6k
            this->setSetSpeed(-sm_mode_target_spd_);
            if (qAbs(this->getSpeed() + sm_mode_target_spd_) < 5){
                sm_reach_target_ = 3;
            }
        }
        else if (sm_reach_target_ == 3){
            // 等待到达1k
            this->setSetSpeed(1000);
            if (sm_mode_timer.elapsed()/1000 > sm_mode_elapse){
                initSMMode(sm_mode_target_spd_,sm_mode_elapse);
            }
        }
    }

    //得到平均解析电流
    double getAverageCur()const{
        return this->last_ten_cur_;
    }

    //设置采集到的电流
    void setActCur(double cur){
        this->act_cur_ = cur;
    }

    //get last ten
    void setAveVoltageAndCurrent(){
        average_num=10;
        if (last_ten_vol_queue_.size() <average_num){//更新从数据采集卡读上的电压
            last_ten_vol_queue_.push_back(this->getVoltage());
        }
        else{
            while (last_ten_vol_queue_.size() >= average_num){
                last_ten_vol_queue_.pop_front();
            }
            last_ten_vol_queue_.push_back(this->getVoltage());
        }

        if (last_ten_cur_queue_daqcard.size() <average_num){//更新从数据采集卡读上的电流
            last_ten_cur_queue_daqcard.push_back(this->getActCur());
        }
        else{
            while (last_ten_cur_queue_daqcard.size() >= average_num){
                last_ten_cur_queue_daqcard.pop_front();
            }
            last_ten_cur_queue_daqcard.push_back(this->getActCur());
        }

        last_ten_vol_ = 0;
        for(double &t:last_ten_vol_queue_){
            last_ten_vol_ += t;
        }
        last_ten_vol_ /= last_ten_vol_queue_.size();

        last_ten_cur_daqcard = 0;
        for(double &t:last_ten_cur_queue_daqcard){
            last_ten_cur_daqcard += t;
        }
        last_ten_cur_daqcard /= last_ten_cur_queue_daqcard.size();

    }
    void setLastTen(double spd){//更新从串口读上的数据
        //        qDebug()<<"motor thread:"<<QThread::currentThreadId();
        if (last_ten_cur_queue_.size() <average_num){//更新解析电流
            last_ten_cur_queue_.push_back(this->getCurrent());
        }
        else{
            while (last_ten_cur_queue_.size() >= average_num){
                last_ten_cur_queue_.pop_front();
            }
            last_ten_cur_queue_.push_back(this->getCurrent());
        }

        if (last_ten_spd_queue_.size() <average_num){
            last_ten_spd_queue_.push_back(spd);
        }
        else{
            while (last_ten_spd_queue_.size() >= average_num){
                last_ten_spd_queue_.pop_front();
            }
            last_ten_spd_queue_.push_back(spd);
        }
        last_ten_cur_ = 0;
        for(double &t:last_ten_cur_queue_){
            last_ten_cur_ += t;
        }
        last_ten_cur_ /= last_ten_cur_queue_.size();

        m_locker.lock();
        last_ten_spd_ = 0;
        for(double &t:last_ten_spd_queue_){
            last_ten_spd_ += t;
        }
        last_ten_spd_ /= last_ten_spd_queue_.size();
        m_locker.unlock();
        //cal
        setAngularMomentum500();
        setAngularMomentumConst500();
        setAngularMomentumDynamic500();
        setAveTorque();
        setHopeAveTorque();
    }

    //设置温度
    void setTemperature(const double tmp){
        temperature_ =  4050/(qLn(((7500*tmp)/(256-tmp))) + 4.98) - 273;
    }
    //设置功率
    void setWate(){
        wate_ = last_ten_cur_daqcard * last_ten_vol_;
    }
    //500mN start
    //设置角动量
    void setAngularMomentum500(){
        this->angular_momentum_ = 2*PI_* J_ * this->last_ten_spd_/60 ;
    }
    //设置角动量最大常值偏差
    void setAngularMomentumConst500(){
        //常值偏差 = 实测值-平均值
        this->jdl_max_const_d_ =this->getSpeed() - last_ten_spd_;

    }
    //设置角动量最大动态偏差
    void setAngularMomentumDynamic500(){
        //速度最大差值
        double max_bias = 0;
        for (double t:last_ten_spd_queue_){
            max_bias = qMax(qAbs(t - last_ten_spd_),max_bias);
        }
        this->jdl_max_dynamic_d_ = max_bias;
    }
    //500mN end

    //计算角动量
    void setAngularMomentum(){
        this->angular_momentum_ = 9.55*J_ * this->last_ten_spd_;
    }
    //计算角动量常值偏差
    void setAngularMomentumConst(){

        this->angular_momentum_const_d = 0.00428 * qAbs(this->getSpeed() - this->last_ten_spd_);
        //        this->angular_momentum_const_d = J_ * qAbs(this->getSpeed() - this->last_ten_spd_);
    }
    //计算角动量动态偏差
    void setAngularMomentumDynamic(){
        double tmp_max = *(std::max_element(last_ten_spd_queue_.begin(),last_ten_spd_queue_.end()));
        this->angular_momentum_dynamic_d = 0.00428 * qAbs(tmp_max - last_ten_spd_);

        //       this->angular_momentum_dynamic_d = J_ * qAbs(tmp_max - last_ten_spd_);
    }
    //计算反作用力矩
    void setReactionMoment(){
        if (last_ten_spd_queue_.size() <= 2) return;
        double last_spd = *last_ten_spd_queue_.rbegin();
        double last_last__spd = *(++last_ten_spd_queue_.rbegin());
        double current_reaction = (last_spd - last_last__spd)* J_ * 2 * PI_ / (60 * current_interval) ;
        //求平均10个
        if (last_ten_reaction_queue_.size() <average_num){
            last_ten_reaction_queue_.push_back(current_reaction);
        }
        else{
            while (last_ten_reaction_queue_.size()>=average_num){
                last_ten_reaction_queue_.pop_front();
            }
            last_ten_reaction_queue_.push_back(current_reaction);
        }

        for(double &t:last_ten_reaction_queue_){
            last_ten_reaction_ += t;
        }
        this->reaction_moment_ = last_ten_reaction_ / last_ten_reaction_queue_.size();
    }
    //计算实时力矩
    void setAveTorque(){

        if (last_ten_spd_queue_.size() <= 2) return;
        double last_spd = *last_ten_spd_queue_.rbegin();
        //qDebug()<<"last_spd = "<<last_spd;
        double last_last__spd = *(++last_ten_spd_queue_.rbegin());
        //qDebug()<<"last_last__spd = "<<last_last__spd;
        double current_torque = (last_spd - last_last__spd)* J_ * 2 * PI_ / (60 * getCurrentInterval());//current_interval可以通过界面上的
        this->AveTorque_ = current_torque;//*81


        //qDebug()<<this->AveTorque_;

    }

    //计算期望平均力矩
    void setHopeAveTorque(){
        if (last_ten_spd_queue_.size() <= 2) return;
        double last_spd = *last_ten_spd_queue_.rbegin();
        double last_last__spd = *(++last_ten_spd_queue_.rbegin());
        double expected_torque = (last_spd - last_last__spd)* J_ * 2 * PI_ / (60 *  getCurrentInterval());
        //求平均10个
        if (last_ten_AveTor_queue_.size() <average_num){

            last_ten_AveTor_queue_.push_back(expected_torque);
        }
        else{
            while (last_ten_AveTor_queue_.size() >= average_num){
                last_ten_AveTor_queue_.pop_front();
            }
            last_ten_AveTor_queue_.push_back(expected_torque);
        }

        for(double &t:last_ten_AveTor_queue_){
            last_ten_AveTor_ += t;
        }
        this->hopeAveTorque_ = last_ten_AveTor_ / last_ten_AveTor_queue_.size();//*81
        last_ten_AveTor_ = 0;
        //qDebug()<<hopeAveTorque_;


    }



    //单独斜坡模式
    void calXpMode(){
        setFlywheelMode(FLYWHEEL_MODE_XP);
        switch (xp_direction) {
        case 0:
            setSetSpeed(count_*xp_spd_interval_);
            ++count_;
            if(getSpeed() >= xp_end_spd_){
                //setSetSpeed(xp_end_spd_);
                xp_direction = 1;
            }
            break;
        case 1:
            --count_;
            setSetSpeed(count_*xp_spd_interval_);
            if(getSpeed() <= 0){
                setSetSpeed(0);
                xp_direction = 0;
                count_ = 0;
            }
            break;
        default:
            break;
        }
    }


    //一键测试下斜坡模式
    void calAutoXpMode(){
        if(!getIsRunning()){
            this->xp_status_ = false;
            p_timer_auto_test_->stop();
            disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(calAutoXpMode()));
            return;
        }

        setFlywheelMode(FLYWHEEL_MODE_XP);
        emit autoTest(AUTO_TEST_XP);
        switch (xp_direction) {
        case 0:
            setSetSpeed(count_*xp_spd_interval_);
            count_++;
            if(getSpeed() >= xp_end_spd_){
                // setSetSpeed(xp_end_spd_);
                xp_direction = 1;
            }
            break;
        case 1:
            --count_;
            if(count_>=0)
            {
                setSetSpeed(count_*xp_spd_interval_);
            }
            else{
                setSetSpeed(0);
            }


            if(getSpeed() <= 0)
            {
                if(qAbs(getSpeed()) >= 0.5)
                {
                    setSetSpeed(0);
                }
                else
                {
                    this->xp_status_ = false;
                    xp_direction = 0;
                    count_ = 0;
                    p_timer_auto_test_->stop();
                    disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(calAutoXpMode()));
                    emit autoTestStart();
                }
            }
            break;
        default:
            break;
        }
    }

    //速度正弦模式
    void calSpdCosMode(){
        setFlywheelMode(FLYWHEEL_MODE_SIN);
        double   Wc = spd_sin_K * spd_sin_A * sin(2 * PI_ * spd_time_/ spd_sin_T);//-pi / 3
        spd_time_++;//要乘0.1

        if(spd_time_ == 2000){
            spd_time_ = 0;
        }
        setSetSpeed(Wc);
    }
    void caltimeconstant(){


        //setAccelerate(tor_set_);
        double TimeConst = 0;
        double sumTimeConst = 0.0;
        setFlywheelMode(FLYWHEEL_MODE_TIMECONSTANT);
        setSetSpeed(t);
        if(qAbs(qAbs(getSpeed())-100)<0.2){
            t += 10;
            setSetSpeed(t);
            start=QTime::currentTime();
            tc = false;
        }
        if(getSpeed()-110 >=0 && tc == false){
            QTime start2 =QTime::currentTime();
            tc = true;
            TimeConst = double(start.msecsTo(start2)) / 3000;
            timeConst.push_back(TimeConst);
            t = 100;
            setSetSpeed(0);
            countConstant++;
            emit airTestEnd();
        }

        if(countConstant == 3)
        {
            for(double &t:timeConst)
                sumTimeConst+=t;
            timeconst = sumTimeConst/timeConst.size();
            countConstant = 0;
            timeConst.clear();
        }

    }

    //    //时间常数
    //    void caltimeconstant(){
    //        //setAccelerate(tor_set_);
    //        setFlywheelMode(FLYWHEEL_MODE_TIMECONSTANT);
    //        setSetSpeed(t);
    //        if(qAbs(qAbs(getSpeed())-100)<0.2){
    //            t += 10;
    //            setSetSpeed(t);
    //            start=QTime::currentTime();
    //             tc = false;
    //        }
    //        if(getSpeed()-110 >=0 && tc == false){
    //            QTime start2 =QTime::currentTime();
    //            tc = true;
    //            timeconst = double(start.msecsTo(start2)) / 3000;
    //            t = 100;
    //            setSetSpeed(0);
    //           emit airTestEnd();
    //        }

    //    }


    //负责控制滑行模式速度
    void setSpdOfAutoTestHXMode()
    {
        if(!getIsRunning()){
            is_hx_mode_init = false;
        }
        else{
            //此处应该有控制滑行模式的指令，但是50的飞轮没有
            if (!reach_target_){
                setSetSpeed(hx_mode_target_spd_);
            }
            else {
                //此处应该是开启滑行指令
                if (!getWithID())
                    emit sendMoTorHX();//将会触发滑行指令且在motordrive线程里执行
                //在mainwindowcpp文件里connect
                else{
                    emit sendMoTorHX2(getID());
                }
                //在mainwindowcpp文件里connect
                if (!start_hx_timer){
                    hx_time.restart();
                    start_hx_timer = true;
                }
                if (!hx_finish && this->getSpeed() < 1){
                    emit sendHXEnd(this->getID(),hx_time.elapsed()/1000);
                    hx_finish = true;
                }
            }
            if (abs(this->getSpeed() - hx_mode_target_spd_) < 1){
                if (!reach_target_){
                    reach_target_ = true;
                    hx_mode_target_spd_ = 0;
                }
                else{
                    reach_target_ = false;
                    is_hx_mode_init = false;
                    //发射开始循环信号，以开启下一个模式
                    //emit autoTestStart();
                }
            }
        }
    }

    //非真空一键测试函数1
    void initTestModeWithAir(){
        if (getIsRunning()){
            setFlywheelMode(FLYWHEEL_MODE_AUTOTESTNOAIR);

            exp_id_ = QDateTime::currentDateTime().toString("yyMMddhhmmss");
            if (is_air_init_){
                return;
            }
            air_test_containor_.clear();
            air_test_containor_ = air_test_containor_raw_data_;

            is_air_init_ = true;
            if(p_timer_auto_test_ == nullptr){
                p_timer_auto_test_ = new QTimer;
            }

            connect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtWithAirModeTestSpd()));
            p_timer_auto_test_->setInterval(10000);
            is_timer_started = false;
            setSetSpeed(0);
        }
        else{
            resetTestModeWithAir();
        }
    }
    //非真空一键测试函数1
    void resetTestModeWithAir(){
        if(!getIsRunning() && is_air_init_){
            exp_id_.clear();
            is_air_init_ = false;
            p_timer_auto_test_->stop();
            disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtWithAirModeTestSpd()));
            return;
        }
    }
    //为了减少定时器的使用，因此这个槽函数通过mainwindow的定时器调用。
    void runWithAirMode(double spd){
        if(!getIsRunning()){
            exp_id_.clear();
            is_air_init_ = false;
            p_timer_auto_test_->stop();
            disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtWithAirModeTestSpd()));
            return;
        }
        if (!air_test_containor_.empty()){
            this->setSetSpeed(air_test_containor_.front());
            if (abs(spd - air_test_containor_.front()) < 10){
                if (!is_timer_started){
                    p_timer_auto_test_->start();
                    is_timer_started = true;
                }
            }
        }
    }
    //非真空一键测试函数1
    void nxtWithAirModeTestSpd(){
        if(!getIsRunning()){
            exp_id_.clear();
            is_air_init_ = false;
            p_timer_auto_test_->stop();
            disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtWithAirModeTestSpd()));
            return;
        }
        is_timer_started = false;
        p_timer_auto_test_->stop();
        air_test_containor_.pop_front();
        if (air_test_containor_.empty()){
            is_air_init_ = false;
            disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtWithAirModeTestSpd()));
            emit airTestEnd();
        }
    }
    //真空一键测试函数-初始化
    void initTestModeNOAir(QVector<QString> mode_lst);

    //测试模式初始化
    void selectTestMode();

    //初始化一键测试
    void initAutoTestSpdMode();//速度模式
    void initAutoTestLJMode();//力矩模式
    void initAutoTestXpMode(double espd,double interval);//斜坡模式
    void initAutoTestDlMode();//电流模式 zzy
    //负责保持一段时间
    void nxtNoAirModeTestSpd();

    void nxtNoAirModeTestTor();

    void nxtNoAirModeTestCur();//zzy

    //负责控制速度
    void setSpdOfAutoTestSpdMode();


    bool getAutoTestSpdMode() const
    {
        return auto_test_spd_mode_first;
    }
    void setAutoTestSpdMode(bool status)
    {
        this->auto_test_spd_mode_first = status;
    }
    //负责控制力矩
    void setTorOfAutoTestTorMode();

    bool getAutoTestTorMode() const
    {
        return auto_test_tor_mode_;
    }
    void setAutoTestTorMode(bool status)
    {
        this->auto_test_tor_mode_ = status;
    }
    //负责控制电流
    void setCurOfAutoTestCurMode();

    void setAutoTestCurMode(bool status){
        this->auto_test_cur_mode_first = status;
    }
    bool getAutoTestCurMode() const{
        return auto_test_cur_mode_first;
    }
    void setHXMode(bool status){
        this->is_hx_mode_init = status;
    }


    bool getHXMode() const
    {
        return is_hx_mode_init;
    }

    void setAverageNum(uint num){
        if (num >=1)
            this->average_num = num;
    }

signals:
    void airTestEnd();
    void autoTestStart();
    void autoTestEnd();
    void sendMoTorHX();//触发滑行指令的信号
    void sendMoTorHX2(uint);
    void autoTest(int);
    //    void autoSpdTest();
    //    void autoTorTest();
    void sendHXEnd(int,double);//滑行结束信号，发送为编号和秒
private:
    uint average_num = 10;

    double temperature_;
    double wate_;


    //存放从串口读上的最近10个电流
    QQueue<double> last_ten_cur_queue_;
    double last_ten_cur_;
    //存放从数据采集卡读上的最近10个电流
    QQueue<double> last_ten_cur_queue_daqcard;
    double last_ten_cur_daqcard;
    //存放从串口读上的最近10个电压
    QQueue<double> last_ten_vol_queue_;
    double last_ten_vol_;



    //存放最近10个转速
    QQueue<double> last_ten_spd_queue_;
    double last_ten_spd_;
    //反作用力矩
    QQueue<double> last_ten_reaction_queue_;
    double last_ten_reaction_;
    //平均力矩
    QQueue<double> last_ten_AveTor_queue_;
    double last_ten_AveTor_;
    //平均力矩
    double AveTorque_ ;
    //期望平均力矩
    double hopeAveTorque_;


    //500mN start
    //角动量常值偏差
    QQueue<double> last_ten_jdl_const_d_queue_;//存放最近10个常值偏差
    double jdl_max_const_d_;//角动量最大常值偏差
    //角动量动态偏差
    QQueue<double> last_ten_jdl_dynamic_d_queue_;//存放最近10个动态偏差
    double jdl_max_dynamic_d_;//角动量最大动态偏差
    //500mN end

    //角动量
    double angular_momentum_;
    //转动惯量
    double J_ = 0.796;



    //角动量常值偏差
    double angular_momentum_const_d;
    //角动量动态偏差
    double angular_momentum_dynamic_d;
    //反作用力矩
    double reaction_moment_;
    //当前运行间隔
    double current_interval = 0.5;
    //实际电流（从数据采集卡采到的）
    double act_cur_ = 0;
    //斜坡模式
    bool xp_status_;
    double xp_end_spd_;
    double xp_spd_interval_;   //转速增量
    int count_ = 0;
    int xp_direction = 0;

    //速度正弦模式
    bool spd_sin_status_;
    double spd_sin_K;
    double spd_sin_A;
    double spd_sin_T;
    int spd_time_;
    //时间常数
    bool time_constant_;//
    bool tc = true;
    QTime start;
    double timeconst = 1.00;
    int t = 100;
    int countConstant = 0;
    QVector<double> timeConst;
    //非真空测试
    QList<double> air_test_containor_;
    QList<double> air_test_containor_raw_data_;
    bool is_air_init_ = false;
    bool is_timer_started = false;
    QTimer* p_timer_auto_test_ = nullptr;

    //真空测试
    QList<int> all_test_mode_;  //存放所有需要测试的项目
    //速度的一键测试
    QList<double> auto_test_spd_mode_containor_first;
    QList<double> auto_test_spd_mode__containor_raw_data_first;
    bool auto_test_spd_mode_first = false;
    //电流的一件测试
    QList<double> auto_test_cur_mode_container_first;
    QList<double> auto_test_cur_mode_container_raw_data_first;
    bool auto_test_cur_mode_first = false;

    /*
    //稳速功耗
    QList<double> auto_test_spd_mode_containor_third;
    QList<double> auto_test_spd_mode__containor_raw_data_third;
    bool auto_test_spd_mode_third = false;*/
    //力矩的一键测试
    QList<double> auto_test_tor_mode_containor_;
    QList<double> auto_test_tor_mode__containor_raw_data_;
    bool auto_test_tor_mode_ = false;
    QString exp_id_; //用来记录一键测试的编号（用以区分不同实验）

    //滑行模式
    bool is_hx_mode_init = false;
    double hx_mode_target_spd_;
    bool reach_target_ = false;
    bool hx_finish = false;
    QTime hx_time;
    bool start_hx_timer = false;
    //寿命模式
    bool is_sm_mode_init = false;
    double sm_mode_target_spd_;
    uint sm_reach_target_ = 0; //0 = 1k,1 = 6k,2 = -6k,3 = wait 3min
    QTime sm_mode_timer;
    uint sm_mode_elapse = 180; //寿命模式持续时间

    QMutex m_locker;
};

#endif // MOTOR_H
