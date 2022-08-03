#ifndef CSQLDATABASE_H
#define CSQLDATABASE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <motor.h>
#include <QMetaType>
#include <QSharedPointer>
#include <QDateTime>
#include <QThread>
#include <QFile>
#include <QDir>

#include "motor.h"

class SqlDataBase : public QObject
{
    Q_OBJECT
public:
    explicit SqlDataBase(QObject *parent = 0);
    bool sqlInit();
    ~SqlDataBase(){
        delete p_sql_query_;
        m_data_base_.close();
    }

    void doSqlQuery(QString query_str, int dst);
    QString makeSaveString(QString product_no, QString fw_id, QString exp_aim,
                           QString exp_time, QString exp_environment, QString exp_address,QVector<QString> motor);
    QString getLastExpId(QString motor_id);
    void getExpDataFromSqlDB(QString motor_id,QString exp_id,QString motor_mode,QString,QString);
    void getExpDataFromSqlDB2(QString motor_id,QString exp_id,QString motor_mode,QString,QString);
signals:
    void emitExpData(QVector<QVector<QString> >);
    void emitExpDataforexcel(QVector<QVector<QString> > );
    void emitLastExpData(QVector<QVector<QString> >);
    void sendQueryRes(QSqlQuery,int);
    void sendErrorText(QString);
public slots:

    void insertIntoDB(QString product_no, QString fw_id, QString exp_aim,
                      QString exp_time, QString exp_environment, QString exp_address,QVector<QString> motor);
    void analysisSqlForDocRes(QSqlQuery, int dst);
    void getLastExpData(QString exp_id,QString motor_id,QString motor_mode);
private:
    QSqlDatabase m_data_base_;
    QSqlQuery* p_sql_query_;
};

#endif // CSQLDATABASE_H
