#include "csqldatabase.h"

SqlDataBase::SqlDataBase(QObject *parent) : QObject(parent)
{
    sqlInit();
    qRegisterMetaType<QSqlQuery>("QSqlQuery");
    qRegisterMetaType<QVector<QString>>("QVector<QString>");
    qRegisterMetaType<QVector<QVector<QString> >>("QVector<QVector<QString> >");


    connect(this,&SqlDataBase::sendQueryRes,this,&SqlDataBase::analysisSqlForDocRes);
}

bool SqlDataBase::sqlInit()
{
    //qDebug() << QSqlDatabase::drivers();查看当前支持的SQL驱动
    //读取当前文件下面的所有.db文件
//    QDir dir(path);
//    QStringList nameFilters;
//    nameFilters << "*.jpg" << "*.png";
//    files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);


    if (QSqlDatabase::contains("qt_sql_default_connection")) //判断是否存在这个连接
    {
        m_data_base_ = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        m_data_base_ = QSqlDatabase::addDatabase("QSQLITE"); //是SQLite对应的驱动名，不能改 第二个参数被省略了，
        m_data_base_.setUserName("HDU");        //第二个参数的默认参数就是上面提到的Qt默认连接名称qt_sql_default_connection。
        m_data_base_.setDatabaseName("motionDataBase.db");//如果需要使用自定义的连接名称
                                                   //（如果程序需要处理多个数据库文件的话就会这样），则应该加入第二个参数

        m_data_base_.setPassword("123456");
    }
    if (!m_data_base_.open()){
        qDebug()<<" sql init error!";
        emit sendErrorText(" sql init error!");
        return false;
    }
    p_sql_query_ = new QSqlQuery(m_data_base_);
    return true;
}

void SqlDataBase::doSqlQuery(QString query_str, int dst)
{
    if (!p_sql_query_->exec(query_str))
    {
        qDebug() << p_sql_query_->lastError().text();
        emit sendErrorText(p_sql_query_->lastError().text());
    }
    else{
        if (dst == 1){//在界面查询数据的标志位
            emit sendQueryRes(*p_sql_query_,1);//发送的*p_sql_query_是指针
        }
        else if (dst == 2){//生成报表的标志位
            emit sendQueryRes(*p_sql_query_,2);
        }
        else if (dst == 3){
            emit sendQueryRes(*p_sql_query_,3);//生成excel
        }
        else{
            return;
        }
    }
}

QString SqlDataBase::makeSaveString(QString product_no, QString fw_id, QString exp_aim,
                                    QString exp_time, QString exp_environment, QString exp_address,QVector<QString> motor)

{
//    qDebug()<<"SQL:"<<QThread::currentThreadId();
    QString query_string;
    if (product_no.isEmpty()){
        product_no = "null";
    }
    if (fw_id.isEmpty()){
        fw_id = "null";
    }
    if (exp_aim.isEmpty()){
        exp_aim = "null";
    }
    if (exp_time.isEmpty()){
        exp_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    if (exp_environment.isEmpty()){
        exp_environment = "null";
    }
    if (exp_address.isEmpty()){
        exp_address = "null";
    }

    if(!m_data_base_.tables().contains(motor.at(0)))
    {
        QString tempsql = "CREATE TABLE ";
        tempsql.append(motor.at(0));//这个是表名
        tempsql.append("([EXPID] VARCHAR (50),[PRODUCTNO] VARCHAR (50),[FLYWHEELID] VARCHAR (50),[EXPAIM] VARCHAR (50),[EXPTIME] VARCHAR (50),[EXPENVIRONMENTAL] VARCHAR (50),"
                       "[EXPADDRESS] VARCHAR (50),[VOL] DOUBLE, [CURRENT] DOUBLE, [SETSPEED] DOUBLE, [SPEED] DOUBLE,"
                       "[SETTORQUE] DOUBLE,[ACTTORQUE] DOUBLE,[HOPETORQUE] DOUBLE,[AVEWATE] DOUBLE,[ANGULARMOMENTUM] DOUBLE,"
                       "[ANGULARMOMENTUMJT] DOUBLE,[ANGULARMOMENTUMDT] DOUBLE,[FLYWHEELMODE] VARCHAR (50),[TIMECONSTANT] DOUBLE,"
                       "[ACTCUR] DOUBLE , [SETCUR] DOUBLE, "
                       "[TIME] TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))");

        QSqlQuery sql_query(m_data_base_);
        if (!sql_query.exec(tempsql))
        {
            qDebug() << sql_query.lastError().text();
            emit sendErrorText(sql_query.lastError().text());
        }
    }
    else{
        query_string.append("INSERT INTO ");
        query_string.append(motor.at(0));
        query_string.append("([EXPID],[PRODUCTNO],[FLYWHEELID],[EXPAIM],[EXPTIME],[EXPENVIRONMENTAL],[EXPADDRESS],[VOL],[CURRENT],[SETSPEED],[SPEED]"
                            ",[SETTORQUE],[ACTTORQUE],[HOPETORQUE],[AVEWATE],[ANGULARMOMENTUM],[ANGULARMOMENTUMDT],[ANGULARMOMENTUMJT]"
                            ",[FLYWHEELMODE],[TIMECONSTANT],[ACTCUR],[SETCUR]) VALUES(");
        query_string.append("'"+motor.at(1)+"',");

        query_string.append("'"+product_no+"',");
        query_string.append("'"+fw_id+"',");
        query_string.append("'"+exp_aim+"',");
        query_string.append("'"+exp_time+"',");
        query_string.append("'"+exp_environment+"',");
        query_string.append("'"+exp_address+"',");
        query_string.append("'"+motor.at(2)+"',");
        query_string.append("'"+motor.at(3)+"',");
        query_string.append("'"+motor.at(4)+"',");
        query_string.append("'"+motor.at(5)+"',");
        query_string.append("'"+motor.at(6)+"',");
        query_string.append("'"+motor.at(7)+"',");
        query_string.append("'"+motor.at(8)+"',");
        query_string.append("'"+motor.at(9)+"',");
        query_string.append("'"+motor.at(10)+"',");
        query_string.append("'"+motor.at(11)+"',");
        query_string.append("'"+motor.at(12)+"',");
        query_string.append("'"+motor.at(13)+"',");
        query_string.append("'"+motor.at(14)+"',");
        query_string.append("'"+motor.at(15)+"',");
        query_string.append("'"+motor.at(16)+"'");
        query_string.append(")");
         qDebug()<<query_string;
    }

    return query_string;
}
//EXPID是什么？
//查询最后一次实验的实验ID
//为了测试完成后自动生成报表
QString SqlDataBase::getLastExpId(QString motor_id)
{
    QString tempsql = QString("select * from %1 where EXPID is not \"\" ORDER BY EXPID DESC LIMIT 1 ").arg(motor_id);
    QSqlQuery sql_query(m_data_base_);
    if (!sql_query.exec(tempsql))
    {
        qDebug() << sql_query.lastError().text();
        emit sendErrorText(sql_query.lastError().text());
    }
    else{
        if (sql_query.next()){
            return sql_query.value("EXPID").toString();
        }
    }
    return QString("null");
}

//调用该接口，从数据库中查询数据并返回查询结果
//在某个模式下按时间查询的功能 不能实现
//motor_id是指表名，例如：MOTORi是指第i张表,i=1,2,...,6;
void SqlDataBase::getExpDataFromSqlDB(QString motor_id, QString exp_id, QString motor_mode,QString start_time,QString end_time)
{
//    QString query_str = "SELECT * FROM ";
//    query_str.append(motor_id);
//    if (exp_id.size() != 0){
//        query_str.append(" WHERE EXPID = ");
//        query_str.append(exp_id);
//    }
//    //if (exp_id.size()!=0 && motor_mode.size() != 0){
//    if (motor_mode.size() != 0){
//        if (exp_id.size() != 0) query_str.append(" AND ");
//        else query_str.append(" WHERE ");
//        query_str.append(" FLYWHEELMODE = ");
//        query_str.append(motor_mode);
//    }
//    QString ss = "WHERE";
//    QString ss1 = "FLYWHEELMODE";
//    if (!query_str.contains(ss)){
//        query_str.append(" WHERE ");
//        query_str.append(" TIME >= \"");
//        query_str.append(start_time);
//        query_str.append("\" AND ");
//        query_str.append(" TIME <= \"");
//        query_str.append(end_time);
//        query_str.append("\"");
//    }
//    else if(query_str.contains(ss1)){
//        query_str.append(" AND ");
//        query_str.append(" TIME >= \"");
//        query_str.append(start_time);
//        query_str.append("\" AND ");
//        query_str.append(" TIME <= \"");
//        query_str.append(end_time);
//        query_str.append("\"");
//    }
//    else{
//        query_str.append(" AND ");
//        query_str.append(" TIME >= \"");
//        query_str.append(start_time);
//        query_str.append("\" AND ");
//        query_str.append(" TIME <= \"");
//        query_str.append(end_time);
//        query_str.append("\"");
//        }
    QString query_str = "SELECT * FROM ";
    query_str.append(motor_id);
    if (exp_id.size() != 0){
            query_str.append(" WHERE EXPID = ");
            query_str.append(exp_id);
        }
    else{
    query_str.append(" WHERE ");
    query_str.append(" TIME >= \"");
    query_str.append(start_time);
    query_str.append("\" AND ");
    query_str.append(" TIME <= \"");
    query_str.append(end_time);
    query_str.append("\"");
    if (motor_mode.size() != 0){
        query_str.append(" AND ");
        query_str.append(" FLYWHEELMODE = ");
        query_str.append(motor_mode);
    }
 }

    //query_str.append(" LIMIT 20000");//1000可能多了
    doSqlQuery(query_str,1);
}
void SqlDataBase::getExpDataFromSqlDB2(QString motor_id, QString exp_id, QString motor_mode,QString start_time,QString end_time)
{
    QString query_str = "SELECT * FROM ";
    query_str.append(motor_id);
    if (exp_id.size() != 0){
            query_str.append(" WHERE EXPID = ");
            query_str.append(exp_id);
        }
    else{
    query_str.append(" WHERE ");
    query_str.append(" TIME >= \"");
    query_str.append(start_time);
    query_str.append("\" AND ");
    query_str.append(" TIME <= \"");
    query_str.append(end_time);
    query_str.append("\"");
    if (motor_mode.size() != 0){
        query_str.append(" AND ");
        query_str.append(" FLYWHEELMODE = ");
        query_str.append(motor_mode);
    }
 }
   /* if (exp_id.size() != 0){
        query_str.append(" WHERE EXPID = ");
        query_str.append(exp_id);
    }
    if (exp_id.size()!=0 && motor_mode != 0){
        if (exp_id.size() != 0) query_str.append(" AND ");
        else query_str.append(" WHERE ");
        query_str.append(" FLYWHEELMODE = ");
        query_str.append(motor_mode);
    }
    QString ss = "WHERE";
    if (!query_str.contains(ss)){
        query_str.append(" WHERE ");
        query_str.append(" TIME >= \"");
        query_str.append(start_time);
        query_str.append("\" AND ");
        query_str.append(" TIME <= \"");
        query_str.append(end_time);
        query_str.append("\"");
    }
    else{
        query_str.append(" AND ");
        query_str.append(" TIME >= \"");
        query_str.append(start_time);
        query_str.append("\" AND ");
        query_str.append(" TIME <= \"");
        query_str.append(end_time);
        query_str.append("\"");
    }*/
    //query_str.append(" LIMIT 20000");
    doSqlQuery(query_str,3);
}

void SqlDataBase::insertIntoDB(QString product_no, QString fw_id, QString exp_aim,
                               QString exp_time, QString exp_environment, QString exp_address,QVector<QString> motor)
{
//    qDebug()<<"SQL:"<<QThread::currentThreadId();
    QString query_str = makeSaveString(product_no,fw_id,exp_aim,
                                       exp_time,exp_environment,exp_address,motor);
    doSqlQuery(query_str,0);

}


void SqlDataBase::analysisSqlForDocRes(QSqlQuery query_res,int dst)
{
    QVector<QVector<QString> > res;
    while(query_res.next()){
        QVector<QString> t;
        for (int i =0;i<23;++i){
            t.append(query_res.value(i).toString());
            //t.append(QString::number(query_res.value(i),'f',2));
        }
        res.push_back(std::move(t));//这句是什么意思
    }
    if (dst == 1)//在界面查询数据的标志位
        emit emitExpData(res);
    else if (dst == 2){//生成报表的标志位
        emit emitLastExpData(res);
    }
    else if (dst == 3){//生成excel的标志位

        emit emitExpDataforexcel(res);
    }
}

//得到最后一次实验数据
void SqlDataBase::getLastExpData(QString exp_id,QString motor_id,QString motor_mode)
{
    //可以根据实验ID测试一下生成的报表会不会有重复现象
    QString query_str = "SELECT * FROM ";
    query_str.append(motor_id);
    //query_str.append(" WHERE EXPID = 190923172128");
    query_str.append(" WHERE EXPID = ");
    if(exp_id.isEmpty())
    {
        query_str.append(getLastExpId(motor_id));
    }
    else{
        query_str.append(exp_id);
    }

    /*
    if (motor_mode.size() != 0){
        query_str.append(" OR ");
        query_str.append(" WHERE ");
        query_str.append(" FLYWHEELMODE = ");
        query_str.append(motor_mode);
    }*/
    doSqlQuery(query_str,2);//生成报表的
}


