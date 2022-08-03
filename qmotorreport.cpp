#include "qmotorreport.h"

QMotorReport::QMotorReport(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QVector<QVector<QString> >>("QVector<QVector<QString> >");
}
//这里也要改
void QMotorReport::setExpInfomation(const QVector<QString> &data)
{
    //数据库改变的话，size = 20 也需要改变
    if (data.size() != 23) {
        emit logMsg(tr("数据库错误，是否改变了表的结构？"));
        return;
    }
    //提取数据库中每一行的数据
    this->exp_id_             = data[0];//C1
    this->product_no_         = data[1];//C2
    this->flywheel_id_        = data[2];//C3
    this->exp_aim_            = data[3];//C4
    this->exp_time_           = data[4];//C5
    this->exp_environmental_  = data[5];//C6
    this->exp_address_        = data[6];//C7
    this->flywheel_vol_.push_back(data[7].toDouble());//C8
    this->flywheel_cur_.push_back(data[8].toDouble());//C9
    this->flywheel_setSpd_.push_back(data[9].toDouble());//C10
    this->flywheel_spd_.push_back(data[10].toDouble());//C11
    this->flywheel_setTor_.push_back(data[11].toDouble());//C12
    this->flywheel_tor_.push_back(data[12].toDouble());//C13
    this->flywheel_hopetor_.push_back(data[13].toDouble());//C14
    this->flywheel_wate_.push_back( data[14].toDouble());//C15
    this->flywheel_JDL_.push_back( data[15].toDouble());//C16
    this->flywheel_maxJDL_constPC_.push_back(data[16].toDouble());//C17
    this->flywheel_maxJDL_dynamicPC_.push_back(data[17].toDouble());//C18
    this->flywheel_mode_ = data[18];//C19
    this->flywheel_timeconstant_.push_back(data[19].toDouble());//C20
    this->flywheel_act_cur_.push_back(data[20].toDouble());//C21
    this->flywheel_setcur_.push_back(data[21].toDouble());
}
void QMotorReport::createWordReport()
{
    QString save_path = path +"EXPID-"+exp_id_ +"-300mNm"+ this->flywheel_id_+exp_aim_+"测试报表-" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QWord word;
    if (!word.createNewWord(save_path)){
        qDebug() << "qword:create word error!";
        return;
    }
    word.setPageOrientation(0);			//页面方向
    word.setWordPageView(3);			//页面视图
    //sword.setFontName(QString::fromLocal8Bit("宋体"));
    word.setParagraphAlignment(0);		//下面文字位置
    //    word.setFontSize(20);				//字体大小
    //    word.setFontBold(true);				//字体加粗
    //    word.insertText(tr("Electrical Equipment Infrared Diagnosis Report "));
    word.setFontBold(false);

    //    word.insertMoveDown();  //控制插入位置

    //    word.setFontSize(10);
    //    word.setParagraphAlignment(1);
    //    QString current_Time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //    word.insertText(tr("Report Generation Date:"));
    //    word.insertText(current_Time);


    word.intsertTable(86,10);             //创建表格
    word.setColumnHeight(1,1,10);

    //合并单元格
    word.MergeCells(1,1,1,1,10);          //第一行

    word.MergeCells(1,2,1,2,2);     //第二行
    word.MergeCells(1,2,7,2,9);
    word.MergeCells(1,2,5,2,6);

    word.MergeCells(1,3,1,3,2);     //第三行
    word.MergeCells(1,3,7,3,9);
    word.MergeCells(1,3,5,3,6);

    word.MergeCells(1,4,8,5,8);     //第四行
    word.MergeCells(1,4,5,4,7);

    word.MergeCells(1,5,1,60,1);     //第5行
    word.MergeCells(1,5,2,31,2);
    word.MergeCells(1,5,3,31,3);
    word.MergeCells(1,5,4,31,4);
    word.MergeCells(1,5,10,31,10);

    word.MergeCells(1,32,5,32,7);     //第23行

    word.MergeCells(1,33,2,58,2);     //第24行
    word.MergeCells(1,33,3,58,3);
    word.MergeCells(1,33,10,58,10);
    word.MergeCells(1,33,5,33,7);


    word.MergeCells(1,34,5,34,7);     //第25行

    word.MergeCells(1,35,5,35,7);     //第26行

    word.MergeCells(1,36,5,36,7);     //第27行

    word.MergeCells(1,37,5,37,7);     //第28行

    word.MergeCells(1,38,5,38,7);     //第29行

    word.MergeCells(1,39,5,39,7);     //第30行

    word.MergeCells(1,40,5,40,7);     //第31行

    word.MergeCells(1,41,5,41,7);     //第32行

    word.MergeCells(1,42,5,42,7);     //第33行

    word.MergeCells(1,43,5,43,7);

    word.MergeCells(1,44,5,44,7);     //第35行

    word.MergeCells(1,45,5,45,7);     //第36行

    word.MergeCells(1,46,5,46,7);     //第37行

    word.MergeCells(1,47,5,47,7);     //第38行

    word.MergeCells(1,48,5,48,7);     //第39行

    word.MergeCells(1,49,5,49,7);     //第40行

    word.MergeCells(1,50,5,50,7);     //第41行

    word.MergeCells(1,51,5,51,7);     //第42行

    word.MergeCells(1,52,5,52,7);

    word.MergeCells(1,53,5,53,7);

    word.MergeCells(1,54,5,54,7);

    word.MergeCells(1,55,5,55,7);

    word.MergeCells(1,56,5,56,7);

    word.MergeCells(1,57,5,57,7);

    word.MergeCells(1,58,5,58,7);

    word.MergeCells(1,59,5,59,7);

    word.MergeCells(1,60,5,60,7);

    word.MergeCells(1,61,1,73,1);
    word.MergeCells(1,61,3,61,4);
    word.MergeCells(1,61,4,61,6);
    word.MergeCells(1,61,5,61,7);

    word.MergeCells(1,62,3,62,4);
    word.MergeCells(1,62,4,62,6);
    word.MergeCells(1,62,5,62,7);


    word.MergeCells(1,63,3,63,4);
    word.MergeCells(1,63,4,63,6);
    word.MergeCells(1,63,5,63,7);

    word.MergeCells(1,64,3,64,4);
    word.MergeCells(1,64,4,64,6);
    word.MergeCells(1,64,5,64,7);

    word.MergeCells(1,65,3,65,4);
    word.MergeCells(1,65,4,65,6);
    word.MergeCells(1,65,5,65,7);

    //    word.MergeCells(1,66,3,66,4);
    //    word.MergeCells(1,66,4,66,6);
    //    word.MergeCells(1,66,5,66,7);

    word.MergeCells(1,66,3,66,4);
    word.MergeCells(1,66,4,66,6);
    word.MergeCells(1,66,5,66,7);

    word.MergeCells(1,67,3,67,4);     //第49行
    word.MergeCells(1,67,4,67,6);
    word.MergeCells(1,67,5,67,7);

    word.MergeCells(1,68,3,68,4);     //第60行
    word.MergeCells(1,68,4,68,6);
    word.MergeCells(1,68,5,68,7);

    word.MergeCells(1,69,3,69,4);     //第61行
    word.MergeCells(1,69,4,69,6);
    word.MergeCells(1,69,5,69,7);

    word.MergeCells(1,70,3,70,4);     //第52行
    word.MergeCells(1,70,4,70,6);
    word.MergeCells(1,70,5,70,7);

    word.MergeCells(1,71,3,71,4);     //第53行
    word.MergeCells(1,71,4,71,6);
    word.MergeCells(1,71,5,71,7);

    word.MergeCells(1,72,3,72,4);     //第54行
    word.MergeCells(1,72,4,72,6);
    word.MergeCells(1,72,5,72,7);

    word.MergeCells(1,73,3,73,4);     //第55行
    word.MergeCells(1,73,4,73,6);
    word.MergeCells(1,73,5,73,7);

    word.MergeCells(1,74,1,86,1); //zzy 电流模式 第一列单元格合并

    for(int i = 0; i < 13; i++) {
        word.MergeCells(1,74+i,3,74+i,4);     //第74-77行 的 3-4， 5-7， 8-10 列单元格合并
        word.MergeCells(1,74+i,4,74+i,6);
        word.MergeCells(1,74+i,5,74+i,7);
    }


    //第一行数据设置
    word.setCellFontBold(1,1,1,true);   //设置是否粗体
    //    word.setTableAutoFitBehavior(0);    //自动拉伸列
    word.setCellFontSize(1,1,1,14);
    //word.setColumnWidth(1,1,120);
    word.setRowAlignment(1,1,0);
    word.setCellString(1,1,1,tr("300mNms微飞轮整机交付电性能测试记录表"));

    //第二行数据设置
    word.setCellFontBold(1,2,1,false);   //设置是否粗体
    //    word.setTableAutoFitBehavior(0);    //自动拉伸列
    //    word.setColumnWidth(1,2,30);
    word.setRowAlignment(1,2,0);
    word.setCellFontSize(1,2,1,10);
    word.setCellString(1,2,1,tr("代号"));

    word.setCellFontBold(1,2,2,false);   //设置是否粗体
    word.setRowAlignment(1,2,0);
    word.setCellFontSize(1,2,2,10);
    word.setCellString(1,2,2,product_no_);

    word.setCellFontBold(1,2,3,false);   //设置是否粗体
    word.setRowAlignment(1,2,0);
    word.setCellFontSize(1,2,3,10);
    word.setCellString(1,2,3,tr("飞轮编号"));

    word.setCellFontBold(1,2,4,false);   //设置是否粗体
    word.setRowAlignment(1,2,0);
    word.setCellFontSize(1,2,4,10);
    word.setCellString(1,2,4,flywheel_id_);

    word.setCellFontBold(1,2,5,false);   //设置是否粗体
    //    word.setTableAutoFitBehavior(0);    //自动拉伸列
    //    word.setColumnWidth(1,3,60);
    word.setRowAlignment(1,2,0);
    word.setCellFontSize(1,2,5,10);
    word.setCellString(1,2,5,tr("测试目的"));                    //需要改

    word.setCellFontBold(1,2,6,false);   //设置是否粗体
    word.setRowAlignment(1,2,0);
    word.setCellFontSize(1,2,6,10);
    word.setCellString(1,2,6,exp_aim_);
    /* word.setCellFontBold(1,2,6,false);   //设置是否粗体
    word.setRowAlignment(1,2,0);
    word.setCellFontSize(1,2,6,10);
    word.setCellString(1,2,6,exp_vacuum_);*/

    //第三行数据设置
    word.setCellFontBold(1,3,1,false);   //设置是否粗体
    //    word.setTableAutoFitBehavior(0);    //自动拉伸列
    //    word.setColumnWidth(1,2,30);
    word.setRowAlignment(1,3,0);
    word.setCellFontSize(1,3,1,10);
    word.setCellString(1,3,1,tr("测试时间"));

    word.setCellFontBold(1,3,2,false);   //设置是否粗体
    word.setRowAlignment(1,3,0);
    word.setCellFontSize(1,3,2,10);
    word.setCellString(1,3,2,exp_time_);

    word.setCellFontBold(1,3,3,false);   //设置是否粗体
    word.setRowAlignment(1,3,0);
    word.setCellFontSize(1,3,3,10);
    word.setCellString(1,3,3,tr("测试地点"));

    word.setCellFontBold(1,3,4,false);   //设置是否粗体
    word.setRowAlignment(1,3,0);
    word.setCellFontSize(1,3,4,10);
    word.setCellString(1,3,4,exp_address_);
    /*  word.setCellFontBold(1,3,4,false);   //设置是否粗体
    word.setRowAlignment(1,3,0);
    word.setCellFontSize(1,3,4,10);
    word.setCellString(1,3,4,exp_address_);*/

    word.setCellFontBold(1,3,5,false);   //设置是否粗体
    //    word.setTableAutoFitBehavior(0);    //自动拉伸列
    //    word.setColumnWidth(1,3,60);
    word.setRowAlignment(1,3,0);
    word.setCellFontSize(1,3,5,10);
    word.setCellString(1,3,5,tr("测试场景"));

    word.setCellFontBold(1,3,6,false);   //设置是否粗体
    word.setRowAlignment(1,3,0);
    word.setCellFontSize(1,3,6,10);
    word.setCellString(1,3,6,exp_environmental_);

    /*  word.setCellFontBold(1,3,6,false);   //设置是否粗体
    word.setRowAlignment(1,3,0);
    word.setCellFontSize(1,3,6,10);
    word.setCellString(1,3,6,this->usr_name_);*/

    //第四行数据设置
    word.setCellFontBold(1,4,2,false);   //设置是否粗体
    word.setRowAlignment(1,4,0);
    word.setCellFontSize(1,4,2,8);
    word.setCellString(1,4,2,tr("序号"));

    word.setCellFontBold(1,4,3,false);   //设置是否粗体
    word.setRowAlignment(1,4,0);
    word.setCellFontSize(1,4,3,8);
    word.setCellString(1,4,3,tr("测试项目"));

    word.setCellFontBold(1,4,4,false);   //设置是否粗体
    word.setRowAlignment(1,4,0);
    word.setCellFontSize(1,4,4,8);
    word.setCellString(1,4,4,tr("指标值"));

    word.setCellFontBold(1,4,5,false);   //设置是否粗体
    word.setRowAlignment(1,4,0);
    word.setCellFontSize(1,4,5,8);
    word.setCellString(1,4,5,tr("实测(计算)值"));

    word.setCellFontBold(1,4,6,false);   //设置是否粗体
    word.setRowAlignment(1,4,0);
    word.setCellFontSize(1,4,6,8);
    word.setCellString(1,4,6,tr("指令转速"));

    word.setCellFontBold(1,4,7,false);   //设置是否粗体
    word.setRowAlignment(1,4,0);
    word.setCellFontSize(1,4,7,8);
    word.setCellString(1,4,7,tr("结论"));

    word.setCellFontBold(1,4,8,false);   //设置是否粗体
    word.setRowAlignment(1,4,0);
    word.setCellFontSize(1,4,8,8);
    word.setCellString(1,4,8,tr("备注"));

    //第五行数据设置
    word.setCellFontBold(1,5,1,false);   //设置是否粗体
    word.setRowAlignment(1,5,0);
    word.setCellFontSize(1,5,1,8);
    word.setCellString(1,5,1,tr("转速模式"));

    word.setCellFontBold(1,5,2,false);   //设置是否粗体
    word.setRowAlignment(1,5,0);
    word.setCellFontSize(1,5,2,8);
    word.setCellString(1,5,2,tr("1"));

    word.setCellFontBold(1,5,3,false);   //设置是否粗体
    word.setRowAlignment(1,5,0);
    word.setCellFontSize(1,5,3,8);
    word.setCellString(1,5,3,tr("调速范围（-6000rpm)~6000rpm角动量控制偏差、常值偏差"));

    word.setCellFontBold(1,5,4,false);   //设置是否粗体
    word.setRowAlignment(1,5,0);
    word.setCellFontSize(1,5,4,8);
    word.setCellString(1,5,4,tr("角动量动态偏差ΔH≤2rpm,常值偏差：H≤2rpm"));

    word.setCellFontBold(1,5,5,false);   //设置是否粗体
    word.setRowAlignment(1,5,0);
    word.setCellFontSize(1,5,5,8);
    word.setCellString(1,5,5,tr("实测转速"));

    word.setCellFontBold(1,5,6,false);   //设置是否粗体
    word.setRowAlignment(1,5,0);
    word.setCellFontSize(1,5,6,8);
    word.setCellString(1,5,6,tr("常值偏差"));

    word.setCellFontBold(1,5,7,false);   //设置是否粗体
    word.setRowAlignment(1,5,0);
    word.setCellFontSize(1,5,7,8);
    word.setCellString(1,5,7,tr("ΔH"));


    //第23行数据设置
    word.setCellFontBold(1,32,2,false);   //设置是否粗体
    word.setRowAlignment(1,32,0);
    word.setCellFontSize(1,32,2,8);
    word.setCellString(1,32,2,tr("2"));

    word.setCellFontBold(1,32,3,false);   //设置是否粗体
    word.setRowAlignment(1,32,0);
    word.setCellFontSize(1,32,3,8);
    word.setCellString(1,32,3,tr("最大反作用力矩"));

    word.setCellFontBold(1,32,4,false);   //设置是否粗体
    word.setRowAlignment(1,32,0);
    word.setCellFontSize(1,32,4,8);
    word.setCellString(1,32,4,tr("绝对值≥20mNm"));

    word.setCellFontBold(1,32,5,false);   //设置是否粗体
    word.setRowAlignment(1,32,0);
    word.setCellFontSize(1,32,5,8);
    word.setCellString(1,32,5,QString::number(m_result_max_tor_));

    word.setCellFontBold(1,32,6,false);   //设置是否粗体
    word.setRowAlignment(1,32,0);
    word.setCellFontSize(1,32,6,8);
    word.setCellString(1,32,6,tr("0~6000加速过程中的平均力矩"));

    word.setCellFontBold(1,32,7,false);   //设置是否粗体
    word.setRowAlignment(1,32,0);
    word.setCellFontSize(1,32,7,8);
    word.setCellString(1,32,7,QString::number(m_result_ave_tor_0_6000));

    //第24行数据设置
    word.setCellFontBold(1,33,2,false);   //设置是否粗体
    word.setRowAlignment(1,33,0);
    word.setCellFontSize(1,33,2,8);
    word.setCellString(1,33,2,tr("3"));

    word.setCellFontBold(1,33,3,false);   //设置是否粗体
    word.setRowAlignment(1,33,0);
    word.setCellFontSize(1,33,3,8);
    word.setCellString(1,33,3,tr("稳速功耗"));



    //第41行数据设置
    word.setCellFontBold(1,59,2,false);   //设置是否粗体
    word.setRowAlignment(1,59,0);
    word.setCellFontSize(1,59,2,8);
    word.setCellString(1,59,2,tr("4"));

    word.setCellFontBold(1,59,3,false);   //设置是否粗体
    word.setRowAlignment(1,59,0);
    word.setCellFontSize(1,59,3,8);
    word.setCellString(1,59,3,tr("最大功耗"));

    word.setCellFontBold(1,59,5,false);   //设置是否粗体
    word.setRowAlignment(1,59,0);
    word.setCellFontSize(1,59,5,8);
    word.setCellString(1,59,5,QString::number(m_result_max_wate_));

    word.setCellFontBold(1,59,8,false);   //设置是否粗体
    word.setRowAlignment(1,59,0);
    word.setCellFontSize(1,59,8,8);
    word.setCellString(1,59,8,tr("最大力矩时的功耗"));

    //第42行数据设置
    word.setCellFontBold(1,60,2,false);   //设置是否粗体
    word.setRowAlignment(1,60,0);
    word.setCellFontSize(1,60,2,8);
    word.setCellString(1,60,2,tr("5"));

    word.setCellFontBold(1,60,3,false);   //设置是否粗体
    word.setRowAlignment(1,60,0);
    word.setCellFontSize(1,60,3,8);
    word.setCellString(1,60,3,tr("最大角动量Hmax"));

    word.setCellFontBold(1,60,4,false);   //设置是否粗体
    word.setRowAlignment(1,60,0);
    word.setCellFontSize(1,60,4,8);
    word.setCellString(1,60,4,tr("绝对值300±1mNms"));

    word.setCellFontBold(1,60,5,false);   //设置是否粗体
    word.setRowAlignment(1,60,0);
    word.setCellFontSize(1,60,5,8);
    word.setCellString(1,60,5,QString::number(m_result_max_jdl_));

    word.setCellFontBold(1,60,8,false);   //设置是否粗体
    word.setRowAlignment(1,60,0);
    word.setCellFontSize(1,60,8,8);
    word.setCellString(1,60,8,tr("6000rpm时的角动量"));


    //第43行数据设置
    word.setCellFontBold(1,61,1,false);   //设置是否粗体
    word.setRowAlignment(1,61,0);
    word.setCellFontSize(1,61,1,8);
    word.setCellString(1,61,1,tr("力矩模式"));

    word.setCellFontBold(1,61,2,false);   //设置是否粗体
    word.setRowAlignment(1,61,0);
    word.setCellFontSize(1,61,2,8);
    word.setCellString(1,61,2,tr("序号"));

    word.setCellFontBold(1,61,3,false);   //设置是否粗体
    word.setRowAlignment(1,61,0);
    word.setCellFontSize(1,61,3,8);
    word.setCellString(1,61,3,tr("实测力矩"));

    word.setCellFontBold(1,61,4,false);   //设置是否粗体
    word.setRowAlignment(1,61,0);
    word.setCellFontSize(1,61,4,8);
    word.setCellString(1,61,4,tr("指令力矩"));

    word.setCellFontBold(1,61,5,false);   //设置是否粗体
    word.setRowAlignment(1,61,0);
    word.setCellFontSize(1,61,5,8);
    word.setCellString(1,61,5,tr("备注"));

    //第44行数据设置
    word.setCellFontBold(1,62,2,false);   //设置是否粗体
    word.setRowAlignment(1,62,0);
    word.setCellFontSize(1,62,2,8);
    word.setCellString(1,62,2,tr("1"));



    //第45行数据设置
    word.setCellFontBold(1,63,2,false);   //设置是否粗体
    word.setRowAlignment(1,63,0);
    word.setCellFontSize(1,63,2,8);
    word.setCellString(1,63,2,tr("2"));



    //第46行数据设置
    word.setCellFontBold(1,64,2,false);   //设置是否粗体
    word.setRowAlignment(1,64,0);
    word.setCellFontSize(1,64,2,8);
    word.setCellString(1,64,2,tr("3"));



    //第47行数据设置
    word.setCellFontBold(1,65,2,false);   //设置是否粗体
    word.setRowAlignment(1,65,0);
    word.setCellFontSize(1,65,2,8);
    word.setCellString(1,65,2,tr("4"));


    //第48行数据设置
    word.setCellFontBold(1,66,2,false);   //设置是否粗体
    word.setRowAlignment(1,66,0);
    word.setCellFontSize(1,66,2,8);
    word.setCellString(1,66,2,tr("5"));



    //第49行数据设置
    word.setCellFontBold(1,67,2,false);   //设置是否粗体
    word.setRowAlignment(1,67,0);
    word.setCellFontSize(1,67,2,8);
    word.setCellString(1,67,2,tr("6"));

    //第60行数据设置
    word.setCellFontBold(1,68,2,false);   //设置是否粗体
    word.setRowAlignment(1,68,0);
    word.setCellFontSize(1,68,2,8);
    word.setCellString(1,68,2,tr("7"));

    //第61行数据设置
    word.setCellFontBold(1,69,2,false);   //设置是否粗体
    word.setRowAlignment(1,69,0);
    word.setCellFontSize(1,69,2,8);
    word.setCellString(1,69,2,tr("8"));

    //第52行数据设置
    word.setCellFontBold(1,70,2,false);   //设置是否粗体
    word.setRowAlignment(1,70,0);
    word.setCellFontSize(1,70,2,8);
    word.setCellString(1,70,2,tr("9"));

    //第53行数据设置
    word.setCellFontBold(1,71,2,false);   //设置是否粗体
    word.setRowAlignment(1,71,0);
    word.setCellFontSize(1,71,2,8);
    word.setCellString(1,71,2,tr("10"));

    //第54行数据设置
    word.setCellFontBold(1,72,2,false);   //设置是否粗体
    word.setRowAlignment(1,72,0);
    word.setCellFontSize(1,72,2,8);
    word.setCellString(1,72,2,tr("11"));

    //第55行数据设置
    word.setCellFontBold(1,73,2,false);   //设置是否粗体
    word.setRowAlignment(1,73,0);
    word.setCellFontSize(1,73,2,8);
    word.setCellString(1,73,2,tr("12"));

    //电流 zzy

    word.setCellFontBold(1,74,1,false);   //设置是否粗体
    word.setRowAlignment(1,74,0);
    word.setCellFontSize(1,74,1,8);
    word.setCellString(1,74,1,tr("电流模式"));

    word.setCellFontBold(1,74,2,false);   //设置是否粗体
    word.setRowAlignment(1,74,0);
    word.setCellFontSize(1,74,2,8);
    word.setCellString(1,74,2,tr("序号"));

    word.setCellFontBold(1,74,3,false);   //设置是否粗体
    word.setRowAlignment(1,74,0);
    word.setCellFontSize(1,74,3,8);
    word.setCellString(1,74,3,tr("实测电流"));

    word.setCellFontBold(1,74,4,false);   //设置是否粗体
    word.setRowAlignment(1,74,0);
    word.setCellFontSize(1,74,4,8);
    word.setCellString(1,74,4,tr("指令电流"));

    word.setCellFontBold(1,74,5,false);   //设置是否粗体
    word.setRowAlignment(1,74,0);
    word.setCellFontSize(1,74,5,8);
    word.setCellString(1,74,5,tr("备注"));

    for(int i = 1; i < 13; i++){
        word.setCellFontBold(1,74+i,2,false);   //设置是否粗体
        word.setRowAlignment(1,74+i,0);
        word.setCellFontSize(1,74+i,2,8);
        word.setCellString(1,74+i,2,QString::number(i));
    }

    //速度测试：向表里填数据
    for (int spd_idx =0;spd_idx < m_test_unit_setspd_.size();++spd_idx){
        //写结论
        word.setCellFontBold(1,6+spd_idx,9,false);   //设置是否粗体
        word.setRowAlignment(1,6+spd_idx,0);
        word.setCellFontSize(1,6+spd_idx,9,8);
        if(m_result_spd_.at(spd_idx).at(1) <= 2 && m_result_spd_.at(spd_idx).at(2) <=2){
            word.setCellString(1,6+spd_idx,9,tr("符合"));
        }
        else{
            word.setCellString(1,6+spd_idx,9,tr("不符合"));
        }
        //写设置转速
        word.setCellFontBold(1,6+spd_idx,8,false);   //设置是否粗体
        word.setRowAlignment(1,6+spd_idx,0);
        word.setCellFontSize(1,6+spd_idx,8,8);
        word.setCellString(1,6+spd_idx,8,QString::number(m_test_unit_setspd_.at(spd_idx)));
        //写设置转速
        word.setCellFontBold(1,33+spd_idx,6,false);   //设置是否粗体
        word.setRowAlignment(1,33+spd_idx,0);
        word.setCellFontSize(1,33+spd_idx,6,8);
        word.setCellString(1,33+spd_idx,6,QString::number(m_test_unit_setspd_.at(spd_idx)));
        //写实际转速
        word.setCellFontBold(1,6+spd_idx,5,false);   //设置是否粗体
        word.setRowAlignment(1,6+spd_idx,0);
        word.setCellFontSize(1,6+spd_idx,5,8);
        word.setCellString(1,6+spd_idx,5,QString::number(m_result_spd_.at(spd_idx).at(0),'f',3));

        //写常值偏差
        word.setCellFontBold(1,6+spd_idx,6,false);   //设置是否粗体
        word.setRowAlignment(1,6+spd_idx,0);
        word.setCellFontSize(1,6+spd_idx,6,8);
        word.setCellString(1,6+spd_idx,6,QString::number(m_result_spd_.at(spd_idx).at(1)));
        //写动态偏差
        word.setCellFontBold(1,6+spd_idx,7,false);   //设置是否粗体
        word.setRowAlignment(1,6+spd_idx,0);
        word.setCellFontSize(1,6+spd_idx,7,8);
        word.setCellString(1,6+spd_idx,7,QString::number(m_result_spd_.at(spd_idx).at(2)));
        //写功耗
        word.setCellFontBold(1,33+spd_idx,5,false);   //设置是否粗体
        word.setRowAlignment(1,33+spd_idx,0);
        word.setCellFontSize(1,33+spd_idx,5,8);
        word.setCellString(1,33+spd_idx,5,QString::number(m_result_spd_.at(spd_idx).at(3)));


    }
    //力矩

    for (int tor_idx =0;tor_idx < m_test_unit_settor_.size();++tor_idx){
        //写指令力矩

        word.setCellFontBold(1,62+tor_idx,4,false);   //设置是否粗体
        word.setRowAlignment(1,62+tor_idx,0);
        word.setCellFontSize(1,62+tor_idx,4,8);
        word.setCellString(1,62+tor_idx,4,QString::number(m_test_unit_settor_.at(tor_idx)));

        //写实测力矩 填的是取了平均值得力矩
        word.setCellFontBold(1,62+tor_idx,3,false);   //设置是否粗体
        word.setRowAlignment(1,62+tor_idx,0);
        word.setCellFontSize(1,62+tor_idx,3,8);
        word.setCellString(1,62+tor_idx,3,QString::number(m_result_tor_.at(tor_idx).at(0),'f',3));//将QString::number(m_result_tor_.at(tor_idx).at(0))该成tr("0")是可以的

    }

    //电流
    for(int i = 0; i < m_test_unit_setcur_.size(); i++){
        //指令电流
        word.setCellFontBold(1,75+i,4,false);   //设置是否粗体
        word.setRowAlignment(1,75+i,0);
        word.setCellFontSize(1,75+i,4,8);
        word.setCellString(1,75+i,4,QString::number(m_test_unit_setcur_.at(i)));
        //串口电流
        word.setCellFontBold(1,75+i,3,false);   //设置是否粗体
        word.setRowAlignment(1,75+i,0);
        word.setCellFontSize(1,75+i,3,8);
        word.setCellString(1,75+i,3,QString::number(m_result_cur_.at(i).at(0),'f',3));//将QString::number(m_result_tor_.at(i).at(0))该成tr("0")是可以的

    }

    //  save and close file
    word.setVisible(false);
    word.saveAs();
    word.close();
    m_result_tor_.clear();
    m_result_spd_.clear();
    m_result_cur_.clear();
    m_result_wate_.clear();

    emit logMsg(tr("生成报告：测试报表已生成！(100%)，存储地址：%1").arg(save_path));
}

void QMotorReport::getDataFromSql(const QVector<QVector<QString> > &res)
{
    if (res.empty()) return;
    //clear history data
    this->flywheel_vol_.clear();
    this->flywheel_cur_.clear();
    this->flywheel_setSpd_.clear();
    this->flywheel_spd_.clear();
    this->flywheel_setTor_.clear();
    this->flywheel_tor_.clear();
    this->flywheel_hopetor_.clear();
    this->flywheel_wate_.clear();
    this->flywheel_JDL_.clear();
    this->flywheel_maxJDL_constPC_.clear();
    this->flywheel_maxJDL_dynamicPC_.clear();
    this->flywheel_mode_.clear();
    this->flywheel_act_cur_.clear();
    this->flywheel_setcur_.clear();
    setExpInfomation(*res.begin());
    //提取数据库中每一列的数据
    for (int i =1;i < res.size();++i){//i为啥要从1开始
        //这里是把报表要计算的数据赛选出来
        //
        const QVector<QString>& data = res.at(i);//提取res第i行的所以数据
        this->flywheel_vol_.push_back(data[7].toDouble());
        this->flywheel_cur_.push_back(data[8].toDouble());
        this->flywheel_setSpd_.push_back(data[9].toDouble());
        this->flywheel_spd_.push_back(data[10].toDouble());
        this->flywheel_setTor_.push_back(data[11].toDouble());
        this->flywheel_tor_.push_back(data[12].toDouble());
        this->flywheel_hopetor_.push_back(data[13].toDouble());//
        this->flywheel_wate_.push_back( data[14].toDouble());
        this->flywheel_JDL_.push_back( data[15].toDouble());
        this->flywheel_mode_.push_back(data[18].toInt());
        this->flywheel_maxJDL_constPC_.push_back(data[16].toDouble());
        this->flywheel_maxJDL_dynamicPC_.push_back(data[17].toDouble());
        this->flywheel_act_cur_.push_back(data[20].toDouble());
        this->flywheel_setcur_.push_back(data[21].toDouble());
    }
    emit logMsg(tr("生成报告：数据查询完毕(20%)"));


    initExpData();
    calExpData();
    //先初始化实验数据然后再计算实验数据
    //最后输出报表
    createWordReport();
    //输出excel
    //createExcel(res);
    emit reportCreated();//这个信号好像没有绑定槽函数
}

//初始化实验数据
void QMotorReport::initExpData()
{
    //提取setspd
    QSet<double> t_setspd;
    int idx =0;
    for (double &setspd:flywheel_setSpd_){
        if (flywheel_mode_.at(idx++) == 0)
            t_setspd.insert(setspd);
    }

    m_test_unit_setspd_.clear();
    for (auto &set_spd:t_setspd){
        m_test_unit_setspd_.push_back(set_spd);
    }

    //输出结果排序 这个可能要删掉
    std::sort(m_test_unit_setspd_.begin(),m_test_unit_setspd_.end(),[](const double &a,const double &b){
        return qAbs(a) < qAbs(b) || (qAbs(a) == qAbs(b) && a >= 0) ;
    });





    //提取settor 因为插入的设置力矩有很多重复的
    QSet<double> t_settor;
    int t_idx =0;
    for (double &settor:flywheel_setTor_){
        if(flywheel_mode_.at(t_idx++) == 3 ){
            if(settor == 0)//恒速的时候设置成的0
                continue;
            t_settor.insert(settor);
        }
    }

    m_test_unit_settor_.clear();
    for (auto &set_tor:t_settor){
        m_test_unit_settor_.push_back(set_tor);
    }

    //输出结果排序 这个可能要删掉
    std::sort(m_test_unit_settor_.begin(),m_test_unit_settor_.end(),[](const double &a,const double &b){
        return qAbs(a) < qAbs(b) || (qAbs(a) == qAbs(b) && a >= 0) ;
    });


    //提取setspd
    QSet<double> t_setcur;
    int c_idx =0;
    for (double &setcur:flywheel_setcur_){
        if (flywheel_mode_.at(c_idx++) == 7)
            t_setcur.insert(setcur);
    }

    m_test_unit_setcur_.clear();
    for (auto &set_cur:t_setcur){
        if(set_cur != 0)//防止把0放入设置电流的序列，导致报表崩坏
            m_test_unit_setcur_.push_back(set_cur);
    }

    //输出结果排序 这个可能要删掉
    std::sort(m_test_unit_setcur_.begin(),m_test_unit_setcur_.end(),[](const double &a,const double &b){
        return qAbs(a) < qAbs(b) || (qAbs(a) == qAbs(b) && a >= 0) ;
    });

}

void QMotorReport::init()
{
    CoInitializeEx(NULL,COINIT_MULTITHREADED);//
    HRESULT r = OleInitialize(0);
    if(r != S_OK && r != S_FALSE){
        qDebug("Qt: Could not initialize OLE (error %x)",(unsigned int)r);
    }
}


/*
//计算从数据库里提取的数据
void QMotorReport::calExpData()
{
    m_result_spd_.clear();
    m_result_tor_.clear();

    m_result_max_jdl_=0;
    m_result_max_wate_=0;
    m_result_max_tor_=0;
    //计算速度实验数据



    for (int idx = 0;idx<m_test_unit_setspd_.size();++idx){
        double current_set_spd = m_test_unit_setspd_.at(idx);

        for (int i =0;i < flywheel_setSpd_.size();++i){
//            if(m_result_max_tor_ < flywheel_hopetor_.at(i) && flywheel_mode_.at(i) == 1){
//            m_result_max_tor_ = flywheel_hopetor_.at(i);
//            }
//            if(m_result_max_wate_ < flywheel_wate_.at(i) && flywheel_mode_.at(i) == 1){
//            m_result_max_wate_ = flywheel_wate_.at(i);
//            }
            if(m_result_max_jdl_ < flywheel_JDL_.at(i) && flywheel_mode_.at(i) == 0){
            m_result_max_jdl_= flywheel_JDL_.at(i);
            }
            }
        //每一次提取当前需要测试的数据 还要加稳速功耗 最大角动量
        QVector<double> need_test_spd;//这个放的是飞轮的实际速度
        QVector<double> maxjdl_constPC;
        QVector<double> maxjdl_dynamicPC;
        QVector<double> steadyspd_wate;//稳速功耗
        QVector<double> maxjdl;//最大角动量
        for (int i =0;i < flywheel_setSpd_.size();++i){
            if (flywheel_setSpd_.at(i) == current_set_spd && flywheel_mode_.at(i) == 0){
                need_test_spd.push_back(flywheel_spd_.at(i));
                maxjdl_constPC.push_back(flywheel_maxJDL_constPC_.at(i));
                maxjdl_dynamicPC.push_back(flywheel_maxJDL_dynamicPC_.at(i));
                steadyspd_wate.push_back(flywheel_wate_.at(i));//稳速功耗
                maxjdl.push_back(flywheel_JDL_.at(i));//最大角动量
            }
        }

        //处理需要测试的数据
        //第一个处理速度 角动量常值偏差 动态偏差 稳速功耗 最大角动量
        int start_test = 0;
        double avg_spd = 0;
        double avg_maxjdl_constPC = 0;
        double avg_maxjdl_dynamicPC = 0;
        double avg_steadyspd_wate = 0;
        double avg_maxjdl = 0;


        uint data_num = 0;
        for (int i = 0;i < need_test_spd.size();++i){
            double spd = need_test_spd.at(i);

            //计算角动量常值偏差和动态偏差要在速度基本到达设定速度后延迟一段时间再开始采集他们的值
            if(start_test <10 ){
                if(qAbs(qAbs(spd) - qAbs(current_set_spd)) > (0.6)){
                    //                        start_test =0;
                    continue;
                }
                else{
                    start_test++;
                }
            }
            else{
                avg_spd += spd;
                avg_maxjdl_constPC += qAbs(maxjdl_constPC.at(i));
                avg_maxjdl_dynamicPC += qAbs(maxjdl_dynamicPC.at(i));
                avg_steadyspd_wate += steadyspd_wate.at(i);
                avg_maxjdl += maxjdl.at(i);
                data_num++;
            }
        }
        QVector<double> tmp_res;
        tmp_res.push_back(avg_spd / data_num);
        tmp_res.push_back(avg_maxjdl_constPC / data_num);
        tmp_res.push_back(avg_maxjdl_dynamicPC / data_num);
        tmp_res.push_back(avg_steadyspd_wate/data_num);
        tmp_res.push_back(avg_maxjdl/data_num);
        m_result_spd_.push_back(tmp_res);
    }

    //斜坡模式数据计算
    for (int i = 0;i< flywheel_tor_.size();i++)
    {
                    if(m_result_max_tor_ < flywheel_tor_.at(i) && flywheel_mode_.at(i) == 1){
                    m_result_max_tor_ = flywheel_tor_.at(i);
                    }
                    if(m_result_max_wate_ < flywheel_wate_.at(i) && flywheel_mode_.at(i) == 1){
                    m_result_max_wate_ = flywheel_wate_.at(i);
                    }
    }



    //计算力矩实验数据
    for (int idx = 0;idx<m_test_unit_settor_.size();++idx){
        double current_set_tor = m_test_unit_settor_.at(idx);
        QVector<double> spd_in_tor;   //力矩模式下的速度
        //每一次提取当前需要测试的数据
        QVector<double> need_test_tor;//存放的是实际力矩
        for (int i =0;i < flywheel_setTor_.size();++i){
            if (flywheel_setTor_.at(i) == current_set_tor && flywheel_mode_.at(i) == 3){
                need_test_tor.push_back(flywheel_tor_.at(i));//把flywheel_tor_改成期望平均力矩flywheel_hopetor_
                  spd_in_tor.push_back(flywheel_spd_.at(i));
            }
//                 if(m_result_max_tor_ < flywheel_hopetor_.at(i) && flywheel_mode_.at(i) == 1){
//                    m_result_max_tor_ = flywheel_hopetor_.at(i);
//                 }
//                  if(m_result_max_wate_ < flywheel_wate_.at(i) && flywheel_mode_.at(i) == 1){
//                  m_result_max_wate_ = flywheel_wate_.at(i);
//                  }



        }

        //处理需要测试的数据 只有实际力矩
        //QSet<double> tor_array;
        //tor_array.clear();
        uint start_test = 0;
        double avg_tor = 0;
        uint data_num = 0;
        for (int i = 0;i < need_test_tor.size();++i){//need_test_tor存放的实际力矩
            //old
//            double tor = need_test_tor.at(i);//tor是实测力矩
//            if (start_test < 10 && qAbs(((tor) - (current_set_tor))/tor) > 0.3){//这个0.8是指实测力矩与设定力矩的偏差
//                start_test++;
//                continue;
//            }
//            else{
//                start_test = 11;
//            }
//            //tor_array.insert(tor);
//            if(spd_in_tor.at(i)*tor >= 0){
//            avg_tor += tor;
//            data_num++;
//            }
//            else{
//                continue;
//            }
                        double tor = need_test_tor.at(i);//tor是实测力矩
                        if ( qAbs(((tor) - (current_set_tor))/current_set_tor) > 0.2){
                            if(qAbs(((tor) - (current_set_tor))/current_set_tor) > 0.7){
                                tor=0.5*tor;
                            }
                            else{
                                continue;
                            }
                        }
                        //tor_array.insert(tor);
                        if(spd_in_tor.at(i)*tor >= 0){
                        avg_tor += tor;

                        }
                        else{
                            continue;
                        }
                        data_num++;
        }
        QVector<double> tmp_res;
        tmp_res.push_back(avg_tor / data_num);
        //auto max_tor = *(std::max_element(tor_array.begin(),tor_array.end()));
        //tmp_res.push_back(max_tor);
        m_result_tor_.push_back(tmp_res);
    }






    emit logMsg(tr("生成报告：数据计算完毕(50%)"));
}
*/

//计算从数据库里提取的数据
void QMotorReport::calExpData()
{
    m_result_spd_.clear();
    m_result_tor_.clear();

    m_result_max_jdl_=0;
    m_result_max_wate_=0;
    m_result_max_tor_=0;
    m_is_rasing = true;


    QVector<double> ave_tor_data;             //0 - -6000升速时力矩数据
    //计算速度实验数据
    for (int idx = 0;idx<m_test_unit_setspd_.size();++idx){
        double current_set_spd = m_test_unit_setspd_.at(idx);

        for (int i =0;i < flywheel_setSpd_.size();++i){
            //计算0 - -6000时期的最大力矩、功耗、评价力矩
            if(flywheel_setSpd_.at(i) > -6000.01 && flywheel_setSpd_.at(i) < -5999.99 &&  flywheel_setSpd_.at(i)*flywheel_spd_.at(i) > 0){
                if(flywheel_mode_.at(i) == 0){
                    if(flywheel_setSpd_.at(i) - flywheel_spd_.at(i) > 10){
                        m_is_rasing = false;
                    }
                    //剔除实际力矩超过平均值太多的值
                    if(qAbs(flywheel_tor_.at(i)) - qAbs(flywheel_hopetor_.at(i)) < 2.5){
                        if(m_result_max_tor_ < qAbs(flywheel_tor_.at(i))){
                            m_result_max_tor_ = qAbs(flywheel_tor_.at(i));
                        }
                        if(m_is_rasing){
                            ave_tor_data.push_back(qAbs(flywheel_tor_.at(i)));//计算处理过数据后的实际力矩的平均值
                        }
                    }
                    if(m_result_max_wate_ < flywheel_wate_.at(i)){
                        m_result_max_wate_ = flywheel_wate_.at(i);
                    }
                }
            }
            if(m_result_max_jdl_ < flywheel_JDL_.at(i) && flywheel_mode_.at(i) == 0){
                m_result_max_jdl_= flywheel_JDL_.at(i);
            }
        }

        //处理加速时的平均力矩
        m_result_ave_tor_0_6000 = 0;
        for(int i = 0 ; i < ave_tor_data.size(); i++){
            m_result_ave_tor_0_6000 += ave_tor_data[i];
        }

        m_result_ave_tor_0_6000 /= ave_tor_data.size();

        //每一次提取当前需要测试的数据 还要加稳速功耗 最大角动量
        QVector<double> need_test_spd;//这个放的是飞轮的实际速度
        QVector<double> maxjdl_constPC;
        QVector<double> maxjdl_dynamicPC;
        QVector<double> steadyspd_wate;//稳速功耗
        QVector<double> maxjdl;//最大角动量

        for (int i =0;i < flywheel_setSpd_.size();++i){
            //计算速度模式下的其他数据
            if (flywheel_setSpd_.at(i) == current_set_spd && flywheel_mode_.at(i) == 0){
                need_test_spd.push_back(flywheel_spd_.at(i));
                maxjdl_constPC.push_back(flywheel_maxJDL_constPC_.at(i));
                maxjdl_dynamicPC.push_back(flywheel_maxJDL_dynamicPC_.at(i));
                steadyspd_wate.push_back(flywheel_wate_.at(i));//稳速功耗
                maxjdl.push_back(flywheel_JDL_.at(i));//最大角动量
            }
        }
        //处理需要测试的数据
        //第一个处理速度 角动量常值偏差 动态偏差 稳速功耗 最大角动量
        bool start_test = false;
        //int start_test = 0;     old
        double avg_spd = 0;
        double avg_maxjdl_constPC = 0;
        double avg_maxjdl_dynamicPC = 0;
        double avg_steadyspd_wate = 0;
        double avg_maxjdl = 0;


        uint data_numPC = 0;




        for (int i = 0;i < need_test_spd.size();++i)
        {
            double spd = need_test_spd.at(i);
            //计算角动量常值偏差和动态偏差要在速度基本到达设定速度后延迟一段时间再开始采集他们的值
            //if (start_test == false &&(maxjdl_constPC.at(i) > 2 && maxjdl_dynamicPC.at(i) > 2)&& (qAbs(spd - current_set_spd) > 1))
            if (start_test == false &&(maxjdl_constPC.at(i) > 2 || maxjdl_dynamicPC.at(i) > 2))
            {
                continue;
            }
            else
            {
                start_test = true;
                avg_maxjdl_constPC += qAbs(maxjdl_constPC.at(i));
                avg_maxjdl_dynamicPC += qAbs(maxjdl_dynamicPC.at(i));
                data_numPC++;
            }
            start_test = false;
        }

        uint data_num1 = 0;
        for (int i = 0;i < need_test_spd.size();++i)
        {
            double spd = need_test_spd.at(i);
            //计算角动量常值偏差和动态偏差要在速度基本到达设定速度后延迟一段时间再开始采集他们的值
            if (start_test == false && qAbs(spd - (current_set_spd)) > 0.6)
            {
                continue;
            }
            else{
                start_test = true;
            }

            avg_steadyspd_wate += steadyspd_wate.at(i);
            avg_maxjdl += maxjdl.at(i);
            avg_spd += spd;
            data_num1++;
        }
        //        for (int i = 0;i < need_test_spd.size();++i){
        //            double spd = need_test_spd.at(i);
        //old
        //            //计算角动量常值偏差和动态偏差要在速度基本到达设定速度后延迟一段时间再开始采集他们的值
        //            if(start_test <15 ){
        //                if(qAbs(qAbs(spd) - qAbs(current_set_spd)) > (0.9)){
        //                    //                        start_test =0;
        //                    continue;
        //                }
        //                else{
        //                    start_test++;
        //                }
        //            }
        //            else{
        //                avg_spd += spd;
        //                avg_maxjdl_constPC += qAbs(maxjdl_constPC.at(i));
        //                avg_maxjdl_dynamicPC += qAbs(maxjdl_dynamicPC.at(i));
        //                avg_steadyspd_wate += steadyspd_wate.at(i);
        //                avg_maxjdl += maxjdl.at(i);
        //                data_num++;
        //            }
        //oldend
        //731old
        //            if (start_test == false &&qAbs(qAbs(spd) - qAbs(current_set_spd)) > 0.3)
        //            {
        //                continue;
        //            }
        //            else
        //            {
        //                start_test = true;
        //            }
        //                            avg_spd += spd;
        //                            avg_maxjdl_constPC += qAbs(maxjdl_constPC.at(i));
        //                            avg_maxjdl_dynamicPC += qAbs(maxjdl_dynamicPC.at(i));
        //                            avg_steadyspd_wate += steadyspd_wate.at(i);
        //                            avg_maxjdl += maxjdl.at(i);
        //                            data_num++;
        //731oldend
        //        }
        QVector<double> tmp_res;
        tmp_res.push_back(avg_spd / data_num1);
        tmp_res.push_back(avg_maxjdl_constPC / data_numPC);
        tmp_res.push_back(avg_maxjdl_dynamicPC / data_numPC);
        tmp_res.push_back(avg_steadyspd_wate/data_num1);
        tmp_res.push_back(avg_maxjdl/data_num1);
        m_result_spd_.push_back(tmp_res);
        data_num1 = 0;
        data_numPC = 0;

    }

    //斜坡模式数据计算300飞轮不需要斜坡模式
    //    for (int i = 0;i< flywheel_tor_.size();i++)
    //    {
    //        if(m_result_max_tor_ < flywheel_tor_.at(i) && flywheel_mode_.at(i) == 1){
    //            m_result_max_tor_ = flywheel_tor_.at(i);
    //        }
    //        if(m_result_max_wate_ < flywheel_wate_.at(i) && flywheel_mode_.at(i) == 1){
    //            m_result_max_wate_ = flywheel_wate_.at(i);
    //        }
    //    }


    //计算力矩实验数据
    for (int idx = 0;idx<m_test_unit_settor_.size();++idx){
        double current_set_tor = m_test_unit_settor_.at(idx);
        QVector<double> spd_in_tor;   //力矩模式下的速度
        //每一次提取当前需要测试的数据
        QVector<double> need_test_tor;//存放的是实际力矩
        for (int i =0;i < flywheel_setTor_.size();++i){
            if (flywheel_setTor_.at(i) == current_set_tor && flywheel_mode_.at(i) == 3){
                need_test_tor.push_back(flywheel_tor_.at(i));//把flywheel_tor_改成期望平均力矩flywheel_hopetor_
                spd_in_tor.push_back(flywheel_spd_.at(i));
            }
        }

        //处理需要测试的数据 只有实际力矩
        //QSet<double> tor_array;
        //tor_array.clear();
        //        uint start_test = 0;
        double avg_tor = 0;
        uint data_num = 0;
        for (int i = 0;i < need_test_tor.size();++i){//need_test_tor存放的实际力矩
            //old
            //            double tor = need_test_tor.at(i);//tor是实测力矩
            //            if (start_test < 10 && qAbs(((tor) - (current_set_tor))/tor) > 0.3){//这个0.8是指实测力矩与设定力矩的偏差
            //                start_test++;
            //                continue;
            //            }
            //            else{
            //                start_test = 11;
            //            }
            //            //tor_array.insert(tor);
            //            if(spd_in_tor.at(i)*tor >= 0){
            //            avg_tor += tor;
            //            data_num++;
            //            }
            //            else{
            //                continue;
            //            }
            double tor = need_test_tor.at(i);//tor是实测力矩
            ///////////注释byzzy
            if ( qAbs(((tor) - (current_set_tor))/current_set_tor) > 0.2){
                if(qAbs(((tor) - (current_set_tor))/current_set_tor) > 0.7){
                    tor=0.5*tor;
                }
                else{
                    continue;
                }
            }
            ///////////注释byzzy

            //tor_array.insert(tor);
            if(spd_in_tor.at(i)*current_set_tor >= 0){
                avg_tor += tor;

            }
            else{
                continue;
            }
            data_num++;
        }
        QVector<double> tmp_res;
        tmp_res.push_back(avg_tor / data_num);
        //auto max_tor = *(std::max_element(tor_array.begin(),tor_array.end()));
        //tmp_res.push_back(max_tor);
        m_result_tor_.push_back(tmp_res);
    }

    //计算电流实验数据 zzy
    for(int idx = 0; idx < m_test_unit_setcur_.size(); idx++){
        double current_set_cur = m_test_unit_setcur_.at(idx);
        QVector<double> test_cur;
        for(int i = 0; i < flywheel_setcur_.size(); i++){
            if(flywheel_setcur_.at(i) == current_set_cur &&flywheel_mode_.at(i) == 7){
                //                if(current_set_cur * flywheel_cur_.at(i) > 0)
                //                    test_cur.push_back(flywheel_cur_.at(i));
                if((current_set_cur > 0)){
                    if(flywheel_spd_.at(i) > 0)
                        test_cur.push_back(flywheel_cur_.at(i));
                }else{
                    if(flywheel_spd_.at(i) < 0)
                        test_cur.push_back(flywheel_cur_.at(i));
                }

            }
        }
        double avg_cur = 0;
        uint data_num = 0;
        for(int i = 0; i < test_cur.size(); i++){
            avg_cur += test_cur.at(i);
            data_num++;
        }
        QVector<double> tmp_res;
        tmp_res.push_back(avg_cur / data_num);
        m_result_cur_.push_back(tmp_res);

    }

    emit logMsg(tr("生成报告：数据计算完毕(50%)"));
}

void QMotorReport::createExcel(const QVector<QVector<QString> > &res)
{
    if (res.empty()){
        //QMessageBox::warning(this,"警告","请检查查询时间是否正确！");
        logMsg(tr("未查询到数据！"));
        return;
    }
    emit processUpdate(0);
    setExpInfomation(*res.begin());
    pApplication=NULL;
    pWorkBooks=NULL;
    pWorkBook=NULL;
    pSheets=NULL;
    pSheet=NULL;
    //    save_path = "E:/15交付验收测试表";
    QString save_path = path + "oldcan交付验收测试表";
    save_path += this->exp_id_;
    save_path += this->flywheel_id_;//这里的飞轮编号怎么加

    //save_path += res.at(0).at(2);
    QString fileName = save_path ;
    pApplication = new QAxObject();
    pApplication->setControl("Ket.Application"); //连接Excel控件 Ket=wps  Excel
    pApplication->dynamicCall("SetVisible(bool)", false); //false不显示窗体
    pApplication->setProperty("DisplayAlerts", false); //不显示任何警告信息。
    pWorkBooks = pApplication->querySubObject("Workbooks");
    QFile file(fileName);
    if(file.exists()){
        pWorkBook = pWorkBooks->querySubObject("Open(const QString &)", fileName);
    }
    else{
        pWorkBooks->dynamicCall("Add");
        pWorkBook = pApplication->querySubObject("ActiveWorkBook");
    }
    //默认有一个sheet
    pSheets = pWorkBook->querySubObject("Worksheets");   //获取工作表集合 // Sheets
    pSheet = pSheets->querySubObject("Item(int)", 1);   //获取工作表集合的工作表1，即sheet1

    QAxObject * range = pSheet->querySubObject("UsedRange");
    //注释时间1.6日
    range->dynamicCall("Clear");

    //写入数据'
    QAxObject *cellA,*cellB,*cellC,*cellD,*cellE,*cellF,*cellG,*cellH,*cellI,*cellJ,*cellK,*cellL,*cellM,*cellN,*cellO,*cellP,*cellQ,*cellR,*cellS,*cellT,*cellU,*cellV,*cellW;
    int cellrow=1;
    QString A="A"+QString::number(cellrow);//设置要操作的单元格，如A1
    QString B="B"+QString::number(cellrow);
    QString C="C"+QString::number(cellrow);
    QString D="D"+QString::number(cellrow);
    QString E="E"+QString::number(cellrow);
    QString F="F"+QString::number(cellrow);
    QString G="G"+QString::number(cellrow);
    QString H="H"+QString::number(cellrow);
    QString I="I"+QString::number(cellrow);
    QString J="J"+QString::number(cellrow);
    QString K="K"+QString::number(cellrow);
    QString L="L"+QString::number(cellrow);
    QString M="M"+QString::number(cellrow);
    QString N="N"+QString::number(cellrow);
    QString O="O"+QString::number(cellrow);
    QString P="P"+QString::number(cellrow);
    QString Q="Q"+QString::number(cellrow);
    QString R="R"+QString::number(cellrow);
    QString S="S"+QString::number(cellrow);
    QString T="T"+QString::number(cellrow);
    QString U="U"+QString::number(cellrow);
    QString V="V"+QString::number(cellrow);
    QString W="W"+QString::number(cellrow);

    cellA = pSheet->querySubObject("Range(QVariant, QVariant)",A);//获取单元格
    cellB = pSheet->querySubObject("Range(QVariant, QVariant)",B);
    cellC = pSheet->querySubObject("Range(QVariant, QVariant)",C);
    cellD = pSheet->querySubObject("Range(QVariant, QVariant)",D);
    cellE = pSheet->querySubObject("Range(QVariant, QVariant)",E);
    cellF = pSheet->querySubObject("Range(QVariant, QVariant)",F);
    cellG = pSheet->querySubObject("Range(QVariant, QVariant)",G);
    cellH = pSheet->querySubObject("Range(QVariant, QVariant)",H);
    cellI = pSheet->querySubObject("Range(QVariant, QVariant)",I);
    cellJ = pSheet->querySubObject("Range(QVariant, QVariant)",J);
    cellK = pSheet->querySubObject("Range(QVariant, QVariant)",K);
    cellL = pSheet->querySubObject("Range(QVariant, QVariant)",L);
    cellM = pSheet->querySubObject("Range(QVariant, QVariant)",M);
    cellN = pSheet->querySubObject("Range(QVariant, QVariant)",N);
    cellO = pSheet->querySubObject("Range(QVariant, QVariant)",O);
    cellP = pSheet->querySubObject("Range(QVariant, QVariant)",P);
    cellQ = pSheet->querySubObject("Range(QVariant, QVariant)",Q);
    cellR = pSheet->querySubObject("Range(QVariant, QVariant)",R);
    cellS = pSheet->querySubObject("Range(QVariant, QVariant)",S);
    cellT = pSheet->querySubObject("Range(QVariant, QVariant)",T);
    cellU = pSheet->querySubObject("Range(QVariant, QVariant)",U);
    cellV = pSheet->querySubObject("Range(QVariant, QVariant)",V);
    cellW = pSheet->querySubObject("Range(QVariant, QVariant)",W);
    cellA->dynamicCall("SetValue(const QVariant&)",QVariant("实验编号"));//设置单元格的值
    cellB->dynamicCall("SetValue(const QVariant&)",QVariant("产品编号"));
    cellC->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮编号"));
    cellD->dynamicCall("SetValue(const QVariant&)",QVariant("测试目的"));
    cellE->dynamicCall("SetValue(const QVariant&)",QVariant("测试时间"));
    cellF->dynamicCall("SetValue(const QVariant&)",QVariant("测试场景"));
    cellG->dynamicCall("SetValue(const QVariant&)",QVariant("测试地点"));
    cellH->dynamicCall("SetValue(const QVariant&)",QVariant("电机电压"));
    cellI->dynamicCall("SetValue(const QVariant&)",QVariant("电机电流(mA)"));
    cellJ->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮设定速度"));
    cellK->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮实际速度"));
    cellL->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮设定力矩"));
    cellM->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮实际力矩"));
    cellN->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮期望力矩"));
    cellO->dynamicCall("SetValue(const QVariant&)",QVariant("平均功耗"));
    cellP->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮角动量"));
    cellQ->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮角动量最大常值偏差"));
    cellR->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮角动量最大动态偏差"));
    cellS->dynamicCall("SetValue(const QVariant&)",QVariant("飞轮模式"));
    cellT->dynamicCall("SetValue(const QVariant&)",QVariant("时间常数"));
    cellU->dynamicCall("SetValue(const QVariant&)",QVariant("采集电流(A)"));
    cellV->dynamicCall("SetValue(const QVariant&)",QVariant("设置电流"));
    cellW->dynamicCall("SetValue(const QVariant&)",QVariant("运行时间"));

    //cellV->setProperty("yyyy-MM-d HH:mm:ss");
    cellW->setProperty("NumberFormatLocal", "yyyy-MM-d HH:mm:ss");
    cellrow++;
    // range->setProperty("HorizontalAlignment",-4108);
    int rows=res.size();
    for(int i=0;i<rows;i++){
        //在这里写个槽函数让跳出for循环的标志位stop=true,这个槽由下面的信号触发
        //if(stop)
        //break;
        emit processUpdate(double(i)/double(rows)*100);
        QString A="A"+QString::number(cellrow);//设置要操作的单元格，如A1
        QString B="B"+QString::number(cellrow);
        QString C="C"+QString::number(cellrow);
        QString D="D"+QString::number(cellrow);
        QString E="E"+QString::number(cellrow);
        QString F="F"+QString::number(cellrow);
        QString G="G"+QString::number(cellrow);
        QString H="H"+QString::number(cellrow);
        QString I="I"+QString::number(cellrow);
        QString J="J"+QString::number(cellrow);
        QString K="K"+QString::number(cellrow);
        QString L="L"+QString::number(cellrow);
        QString M="M"+QString::number(cellrow);
        QString N="N"+QString::number(cellrow);
        QString O="O"+QString::number(cellrow);
        QString P="P"+QString::number(cellrow);
        QString Q="Q"+QString::number(cellrow);
        QString R="R"+QString::number(cellrow);
        QString S="S"+QString::number(cellrow);
        QString T="T"+QString::number(cellrow);
        QString U="U"+QString::number(cellrow);
        QString V="V"+QString::number(cellrow);
        QString W="W"+QString::number(cellrow);

        cellA = pSheet->querySubObject("Range(QVariant, QVariant)",A);//获取单元格
        cellB = pSheet->querySubObject("Range(QVariant, QVariant)",B);
        cellC = pSheet->querySubObject("Range(QVariant, QVariant)",C);
        cellD = pSheet->querySubObject("Range(QVariant, QVariant)",D);
        cellE = pSheet->querySubObject("Range(QVariant, QVariant)",E);
        cellF = pSheet->querySubObject("Range(QVariant, QVariant)",F);
        cellG = pSheet->querySubObject("Range(QVariant, QVariant)",G);
        cellH = pSheet->querySubObject("Range(QVariant, QVariant)",H);
        cellI = pSheet->querySubObject("Range(QVariant, QVariant)",I);
        cellJ = pSheet->querySubObject("Range(QVariant, QVariant)",J);
        cellK = pSheet->querySubObject("Range(QVariant, QVariant)",K);
        cellL = pSheet->querySubObject("Range(QVariant, QVariant)",L);
        cellM = pSheet->querySubObject("Range(QVariant, QVariant)",M);
        cellN = pSheet->querySubObject("Range(QVariant, QVariant)",N);
        cellO = pSheet->querySubObject("Range(QVariant, QVariant)",O);
        cellP = pSheet->querySubObject("Range(QVariant, QVariant)",P);
        cellQ = pSheet->querySubObject("Range(QVariant, QVariant)",Q);
        cellR = pSheet->querySubObject("Range(QVariant, QVariant)",R);
        cellS = pSheet->querySubObject("Range(QVariant, QVariant)",S);
        cellT = pSheet->querySubObject("Range(QVariant, QVariant)",T);
        cellU = pSheet->querySubObject("Range(QVariant, QVariant)",U);
        cellV = pSheet->querySubObject("Range(QVariant, QVariant)",V);
        cellW = pSheet->querySubObject("Range(QVariant, QVariant)",W);
        cellA->dynamicCall("SetValue(const QVariant&)",QVariant(res.at(i).at(0)));//设置单元格的值
        cellB->dynamicCall("SetValue(const QVariant&)",QVariant(res.at(i).at(1)));
        cellC->dynamicCall("SetValue(const QVariant&)",QVariant(res.at(i).at(2)));
        cellD->dynamicCall("SetValue(const QVariant&)",QVariant(res.at(i).at(3)));
        cellE->dynamicCall("SetValue(const QVariant&)",QVariant(res.at(i).at(4)));
        cellF->dynamicCall("SetValue(const QVariant&)",QVariant(res.at(i).at(5)));
        cellG->dynamicCall("SetValue(const QVariant&)",QVariant(res.at(i).at(6)));
        cellH->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(7).toDouble(),'f',2)));
        cellI->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(8).toDouble(),'f',3)));
        cellJ->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(9).toDouble(),'f',2)));
        cellK->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(10).toDouble(),'f',2)));
        cellL->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(11).toDouble(),'f',2)));
        cellM->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(12).toDouble(),'f',2)));
        cellN->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(13).toDouble(),'f',2)));
        cellO->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(14).toDouble(),'f',2)));
        cellP->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(15).toDouble(),'f',4)));
        cellQ->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(16).toDouble(),'f',4)));
        cellR->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(17).toDouble(),'f',4)));
        cellS->dynamicCall("SetValue(const QVariant&)",QVariant(res.at(i).at(18)));
        cellT->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(19).toDouble(),'f',3)));
        cellU->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(20).toDouble(),'f',3)));
        cellV->dynamicCall("SetValue(const QVariant&)",QVariant(QString::number(res.at(i).at(21).toDouble(),'f',3)));
        cellW->dynamicCall("SetValue(const QVariant&)",QVariant(res.at(i).at(22)));

        cellrow++;
    }


    range = pSheet->querySubObject("UsedRange");
    QAxObject * cells = range->querySubObject("Columns");
    cells->dynamicCall("AutoFit");
    cells->setProperty("WrapText", true);
    //NumberFormat = "yyyy-MM-d HH:mm:ss";

    range->setProperty("HorizontalAlignment",-4108);
    range->setProperty("VerticalAlignment",-4108);
    pWorkBook->dynamicCall("SaveAs(const QString &)",
                           QDir::toNativeSeparators(fileName));
    if (pApplication != NULL){
        pApplication->dynamicCall("Quit()");
        delete pApplication;
        pApplication = NULL;
    }
    emit processUpdate(100);
    emit logMsg(tr("生成报告：测试报表excel已生成！(100%)，存储地址：%1").arg(save_path));
}

void QMotorReport::close()

{
    pApplication->dynamicCall("Quit()");

    delete pSheet;
    delete pSheets;
    delete pWorkBook;
    delete pWorkBooks;
    delete pApplication;

    pApplication = 0;
    pWorkBooks = 0;
    pWorkBook = 0;
    pSheets = 0;
    pSheet = 0;

}

void QMotorReport::setSavePath(QString path)
{
    this->path = path;
}
