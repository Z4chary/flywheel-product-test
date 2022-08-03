#ifndef ReadExcel_H
#define ReadExcel_H

#include <QObject>
#include <QVector>
#include <QAxObject>
#include <QThread>
#include <windows.h>
#include <QDebug>
class ReadExcel : public QObject
{
    Q_OBJECT
public:
    explicit ReadExcel(QObject *parent = nullptr);
    ~ReadExcel();
    const QList<double> getSpeedSequence() const{ return mSpeedsList; }
    void setPath(const QString& path){emit sendReadData(path);}
signals:
    void readDataFinished();
    void sendReadData(QString path);
public slots:
    void readDataFromExcel(QString file);
private:
    //QList<QList<QVariant>> mSpeedsList;
    QList<double> mSpeedsList;
    QString mPath;
    QThread mThread;
};

#endif // ReadExcel_H
