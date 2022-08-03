#include "motor.h"


QList<double> Motor::getNoAirTestUnit() const
{
    return air_test_containor_raw_data_;
}
//真空测试模式
void Motor::initTestModeNOAir(QVector<QString> mode_lst)
{
    exp_id_ = QDateTime::currentDateTime().toString("yyMMddhhmmss");//记录最后一次实验数据？在执行sql语句那里有体现
    all_test_mode_.clear();//如果没有clear再按一件测试按钮会接着跑上一次存在all_test_mode_的模式
    for (QString &lst:mode_lst){
        all_test_mode_.push_back(lst.toInt());
    }
    //发射开始循环信号
    emit autoTestStart();
}

//将会被autoTestStart触发
void Motor::selectTestMode()
{
    if (all_test_mode_.isEmpty()){
        this->setSetSpeed(0);
        emit autoTestEnd();
        exp_id_.clear();
    }
    else{
        int cur_mode = all_test_mode_.front();
        all_test_mode_.pop_front();
        switch (cur_mode) {
        case FLYWHEEL_MODE_SPD:
            initAutoTestSpdMode();
            break;
        case FLYWHEEL_MODE_TOR:
            this->setSetSpeed(0);
            initAutoTestLJMode();
            break;
        case FLYWHEEL_MODE_XP:
            initAutoTestXpMode(5800,100);
            break;
        case FLYWHEEL_MODE_DL:
            initAutoTestDlMode();
            break;
        default:
            break;
        }
    }
}

void Motor::initAutoTestSpdMode()
{
    if (getIsRunning()){
        setFlywheelMode(FLYWHEEL_MODE_SPD);
        //实测转速，常值偏差，动态偏差
        auto_test_spd_mode_containor_first.clear();
        auto_test_spd_mode_containor_first = auto_test_spd_mode__containor_raw_data_first;
        /*
        //稳速功耗
        auto_test_spd_mode_containor_third.clear();
        auto_test_spd_mode_containor_third = auto_test_spd_mode__containor_raw_data_third;
        */
        if(p_timer_auto_test_ == nullptr){
            p_timer_auto_test_ = new QTimer;
        }

        connect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestSpd()),Qt::UniqueConnection);
        p_timer_auto_test_->setInterval(15000);//以前是5s
        is_timer_started = false;
        auto_test_spd_mode_first = true;
        //setSetSpeed(0);
    }
    else{
        auto_test_spd_mode_first = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestSpd()));
    }
}
//这个槽函数主要是干啥的
void Motor::nxtNoAirModeTestSpd()
{
    if(!getIsRunning()){
        auto_test_spd_mode_first = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestSpd()));
        return;
    }
    is_timer_started = false;
    p_timer_auto_test_->stop();
    auto_test_spd_mode_containor_first.pop_front();
    if (auto_test_spd_mode_containor_first.empty()){
        auto_test_spd_mode_first = false;
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestSpd()));

        //发射开始循环信号，以开启下一个模式
        emit autoTestStart();
    }
}
void Motor::initAutoTestLJMode()
{
    if (getIsRunning()){
        setFlywheelMode(FLYWHEEL_MODE_TOR);
        //ui->statusBar->showMessage("力矩测试",5000);
        auto_test_tor_mode_containor_.clear();
        auto_test_tor_mode_containor_ = auto_test_tor_mode__containor_raw_data_;

        if(p_timer_auto_test_ == nullptr){
            p_timer_auto_test_ = new QTimer;
        }

        connect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestTor()),Qt::UniqueConnection);
        p_timer_auto_test_->setInterval(100);//0.1s
        is_timer_started = false;
        auto_test_tor_mode_ = true;
        //setSetTorque(0);//这个不知道是干啥的
    }
    else{
        auto_test_tor_mode_ = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestTor()));
    }

}

void Motor::initAutoTestXpMode(double espd,double interval)
{
    this->xp_end_spd_ = espd;
    this->xp_spd_interval_ = interval;
    this->xp_direction = 0;
    this->xp_status_ = true;
    if (getIsRunning())
    {
        setFlywheelMode(FLYWHEEL_MODE_XP);
        if(p_timer_auto_test_ == nullptr)
        {
            p_timer_auto_test_ = new QTimer;
        }
        connect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(calAutoXpMode()),Qt::UniqueConnection);

        p_timer_auto_test_->setInterval(500);//以前是5s
        p_timer_auto_test_->start();
        is_timer_started = true;
    }
    else
    {
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(calAutoXpMode()));
    }
}

void Motor::initAutoTestDlMode()
{
    if(getIsRunning()){
        setFlywheelMode(FLYWHEEL_MODE_DL);
        auto_test_cur_mode_container_first.clear();
        auto_test_cur_mode_container_first = auto_test_cur_mode_container_raw_data_first;
        if(p_timer_auto_test_ == nullptr){
            p_timer_auto_test_ = new QTimer;
        }
        connect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestCur()),Qt::UniqueConnection);
        p_timer_auto_test_->setInterval(15000);//时间待设置
        is_timer_started = false;
        auto_test_cur_mode_first = true;
    }
    else{
        auto_test_cur_mode_first = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestCur()));
    }
}




void Motor::nxtNoAirModeTestTor()
{
    if(!getIsRunning()){
        auto_test_tor_mode_ = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestTor()));
        return;
    }
    is_timer_started = false;
    p_timer_auto_test_->stop();
    auto_test_tor_mode_containor_.pop_front();
    if (auto_test_tor_mode_containor_.empty()){
        auto_test_tor_mode_ = false;
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestTor()));

        setSetSpeed(0);
        QTime t_timer11;
        for (t_timer11.restart();t_timer11.elapsed() < 20000;)   //十秒延时
        {}

        //发射开始循环信号，以开启下一个模式
        emit autoTestStart();
    }
}

void Motor::nxtNoAirModeTestCur()
{
    if(!getIsRunning()){
        auto_test_cur_mode_first = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_, SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestCur()));
        return;
    }
    is_timer_started = false;
   // p_timer_auto_test_->stop();
    auto_test_cur_mode_container_first.pop_front();
    if(auto_test_cur_mode_container_first.empty()){
        auto_test_cur_mode_first = false;
        disconnect(p_timer_auto_test_, SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestCur()));
        emit autoTestStart();
    }
}
//控制一键测试的目标速度
//每0.5s触发一次
void Motor::setSpdOfAutoTestSpdMode()
{
    if(!getIsRunning()){
        auto_test_spd_mode_first = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestSpd()));
        return;
    }
    if (!auto_test_spd_mode_containor_first.empty()){
        emit autoTest(AUTO_TEST_SPD);
        this->setSetSpeed(auto_test_spd_mode_containor_first.front());
        if(qAbs(auto_test_spd_mode_containor_first.front()) >= 5000){
            if (qAbs(this->getAverageSpd() - auto_test_spd_mode_containor_first.front()) < 0.5){
                //当实际速度与设定速度之差小于10时打开定时器，打开定时器之后，过了5s后，将会触发nxtNoAirModeTestSpd
                //在nxtNoAirModeTestSpd函数体内关闭定时器并将定时器的标志位设置成false，将容器auto_test_spd_mode_containor_用掉的
                //对象删除并发送触发下一个模式的信号，随着时间的累加，当auto_test_spd_mode_containor_没有元素时，要断开nxtNoAirModeTestSpd
                //与5s的时间链接，否则会一直进入nxtNoAirModeTestSpd这个函数。
                if (!is_timer_started){
                    p_timer_auto_test_->start();
                    is_timer_started = true;
                }
            }

        }else{
            if (qAbs(this->getAverageSpd() - auto_test_spd_mode_containor_first.front()) < 0.8){
                //当实际速度与设定速度之差小于10时打开定时器，打开定时器之后，过了5s后，将会触发nxtNoAirModeTestSpd
                //在nxtNoAirModeTestSpd函数体内关闭定时器并将定时器的标志位设置成false，将容器auto_test_spd_mode_containor_用掉的
                //对象删除并发送触发下一个模式的信号，随着时间的累加，当auto_test_spd_mode_containor_没有元素时，要断开nxtNoAirModeTestSpd
                //与5s的时间链接，否则会一直进入nxtNoAirModeTestSpd这个函数。
                if (!is_timer_started){
                    p_timer_auto_test_->start();
                    is_timer_started = true;
                }
            }
        }

    }
}


void Motor::setTorOfAutoTestTorMode()
{
    if(!getIsRunning()){
        auto_test_tor_mode_ = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestTor()));
        return;
    }
    if (!auto_test_tor_mode_containor_.empty()){
        emit autoTest(AUTO_TEST_TOR);
        this->setSetTorque(auto_test_tor_mode_containor_.front());
        if(qAbs(auto_test_tor_mode_containor_.front())<=1)
        {
            if(auto_test_tor_mode_containor_.front() >= 0)
            {
                if(getAverageSpd() >= 400){
                    if(!is_timer_started){
                        p_timer_auto_test_->start();
                        is_timer_started = true;
                    }
                }
            }
            else
            {
                if (getAverageSpd() < -400)
                {
                    if (!is_timer_started)
                    {
                        p_timer_auto_test_->start();
                        is_timer_started = true;
                    }
                }
            }
        }
        else if(qAbs(auto_test_tor_mode_containor_.front()) > 1&&qAbs(auto_test_tor_mode_containor_.front())<= 10){
            if(auto_test_tor_mode_containor_.front()>0){
                if (getSpeed() > 1000){//1表示的是实际力矩与设置力矩的偏差值
                    //当实际速度与设定速度之差小于10时打开定时器，打开定时器之后，过了5s后，将会触发nxtNoAirModeTestSpd
                    //在nxtNoAirModeTestSpd函数体内关闭定时器并将定时器的标志位设置成false，将容器auto_test_spd_mode_containor_用掉的
                    //对象删除并发送触发下一个模式的信号，随着时间的累加，当auto_test_spd_mode_containor_没有元素时，要断开nxtNoAirModeTestSpd
                    //与5s的时间链接，否则会一直进入nxtNoAirModeTestSpd这个函数。this->getAveTorque()可能要改成getHopeTorque()

                    if (!is_timer_started){
                        p_timer_auto_test_->start();
                        is_timer_started = true;
                    }
                }
            }
            else{
                if (getSpeed() < -1000){
                    if (!is_timer_started){
                        p_timer_auto_test_->start();
                        is_timer_started = true;
                    }
                }
            }
        }
        else{
            if(auto_test_tor_mode_containor_.front()>0){
                if (getSpeed() > 2000){//1表示的是实际力矩与设置力矩的偏差值
                    //当实际速度与设定速度之差小于10时打开定时器，打开定时器之后，过了5s后，将会触发nxtNoAirModeTestSpd
                    //在nxtNoAirModeTestSpd函数体内关闭定时器并将定时器的标志位设置成false，将容器auto_test_spd_mode_containor_用掉的
                    //对象删除并发送触发下一个模式的信号，随着时间的累加，当auto_test_spd_mode_containor_没有元素时，要断开nxtNoAirModeTestSpd
                    //与5s的时间链接，否则会一直进入nxtNoAirModeTestSpd这个函数。this->getAveTorque()可能要改成getHopeTorque()

                    if (!is_timer_started){
                        p_timer_auto_test_->start();
                        is_timer_started = true;
                    }
                }
            }
            else{
                if (getSpeed() < -2000){
                    if (!is_timer_started){
                        p_timer_auto_test_->start();
                        is_timer_started = true;
                    }
                }
            }
        }
    }
}

void Motor::setCurOfAutoTestCurMode()
{
    if(!getIsRunning()){
        auto_test_cur_mode_first = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestCur()));
        return;
    }
    if (!auto_test_cur_mode_container_first.empty()){
        emit autoTest(AUTO_TEST_DL);
        this->setSetCurrent(auto_test_cur_mode_container_first.front());
        if (!is_timer_started){
            p_timer_auto_test_->start();
            is_timer_started = true;
        }
        //        if(qAbs(auto_test_spd_mode_containor_first.front()) >= 5000){
        //            if (qAbs(this->getAverageSpd() - auto_test_spd_mode_containor_first.front()) < 0.5){
        //                //当实际速度与设定速度之差小于10时打开定时器，打开定时器之后，过了5s后，将会触发nxtNoAirModeTestSpd
        //                //在nxtNoAirModeTestSpd函数体内关闭定时器并将定时器的标志位设置成false，将容器auto_test_spd_mode_containor_用掉的
        //                //对象删除并发送触发下一个模式的信号，随着时间的累加，当auto_test_spd_mode_containor_没有元素时，要断开nxtNoAirModeTestSpd
        //                //与5s的时间链接，否则会一直进入nxtNoAirModeTestSpd这个函数。
        //                if (!is_timer_started){
        //                    p_timer_auto_test_->start();
        //                    is_timer_started = true;
        //                }
        //            }

        //        }else{
        //            if (qAbs(this->getAverageSpd() - auto_test_spd_mode_containor_first.front()) < 0.8){
        //                //当实际速度与设定速度之差小于10时打开定时器，打开定时器之后，过了5s后，将会触发nxtNoAirModeTestSpd
        //                //在nxtNoAirModeTestSpd函数体内关闭定时器并将定时器的标志位设置成false，将容器auto_test_spd_mode_containor_用掉的
        //                //对象删除并发送触发下一个模式的信号，随着时间的累加，当auto_test_spd_mode_containor_没有元素时，要断开nxtNoAirModeTestSpd
        //                //与5s的时间链接，否则会一直进入nxtNoAirModeTestSpd这个函数。
        //                if (!is_timer_started){
        //                    p_timer_auto_test_->start();
        //                    is_timer_started = true;
        //                }
        //            }
        //        }

    }
}

/*
void Motor::setTorOfAutoTestTorMode()
{
    if(!getIsRunning()){
        auto_test_tor_mode_ = false;
        p_timer_auto_test_->stop();
        disconnect(p_timer_auto_test_,SIGNAL(timeout()),this,SLOT(nxtNoAirModeTestTor()));
        return;
    }
    if (!auto_test_tor_mode_containor_.empty()){
        emit autoTest(AUTO_TEST_TOR);
        this->setSetTorque(auto_test_tor_mode_containor_.front());
        if(qAbs(auto_test_tor_mode_containor_.front())<1.2){
          //if (qAbs(this->getHopeAveTorque() - auto_test_tor_mode_containor_.front()) < 1){//1表示的是实际力矩与设置力矩的偏差值
          if (qAbs(getSpeed() > 500)){//1表示的是实际力矩与设置力矩的偏差值
        //当实际速度与设定速度之差小于10时打开定时器，打开定时器之后，过了5s后，将会触发nxtNoAirModeTestSpd
            //在nxtNoAirModeTestSpd函数体内关闭定时器并将定时器的标志位设置成false，将容器auto_test_spd_mode_containor_用掉的
            //对象删除并发送触发下一个模式的信号，随着时间的累加，当auto_test_spd_mode_containor_没有元素时，要断开nxtNoAirModeTestSpd
            //与5s的时间链接，否则会一直进入nxtNoAirModeTestSpd这个函数。this->getAveTorque()可能要改成getHopeTorque()
            if (!is_timer_started){
                p_timer_auto_test_->start();
                is_timer_started = true;
            }
          }
        }
        else{
            if (qAbs(getSpeed() > 1000)){//1表示的是实际力矩与设置力矩的偏差值
          //当实际速度与设定速度之差小于10时打开定时器，打开定时器之后，过了5s后，将会触发nxtNoAirModeTestSpd
              //在nxtNoAirModeTestSpd函数体内关闭定时器并将定时器的标志位设置成false，将容器auto_test_spd_mode_containor_用掉的
              //对象删除并发送触发下一个模式的信号，随着时间的累加，当auto_test_spd_mode_containor_没有元素时，要断开nxtNoAirModeTestSpd
              //与5s的时间链接，否则会一直进入nxtNoAirModeTestSpd这个函数。this->getAveTorque()可能要改成getHopeTorque()
              if (!is_timer_started){
                  p_timer_auto_test_->start();
                  is_timer_started = true;
              }
            }

    }

    }
}
*/


