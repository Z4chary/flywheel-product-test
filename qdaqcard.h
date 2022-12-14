#ifndef QDAQCARD_H
#define QDAQCARD_H

#include <QObject>
#include <QString>
#include <QVector>

#include "D2kDask.h"

using namespace DAQCARD;
class QDaqcard : public QObject
{
    Q_OBJECT
public:
    explicit QDaqcard(QObject *parent = 0);
    ~QDaqcard();
    void init();
    double getVoltageOfChannel(uint channel);
signals:
    void logMsg(QString);
    void sendAllData(QVector<double>);
    void sendSurgeCurrnt(QVector<double>);
public slots:
    void readAllChannel();
    void getSurgeCurrent(U16 channel, QVector<double> &v, int point_num);
    void setDOPort(uint port,bool status);
private:
    bool is_init_ = false;
    DAQCARD::I16 card_;
    uchar m_DO_port_status_ = 0x00;
};

#endif // QDAQCARD_H
