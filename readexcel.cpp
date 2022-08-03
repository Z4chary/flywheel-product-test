#include "ReadExcel.h"

ReadExcel::ReadExcel(QObject *parent) : QObject(parent)
{
    this->moveToThread(&mThread);
    mThread.start();
    connect(this, &ReadExcel::sendReadData, this, &ReadExcel::readDataFromExcel);
}

ReadExcel::~ReadExcel()
{
    disconnect(this, &ReadExcel::sendReadData, this, &ReadExcel::readDataFromExcel);
    mThread.quit();
    mThread.wait();
}

void ReadExcel::readDataFromExcel(QString file)
{
    if(file.isEmpty())
        return;
    HRESULT result = OleInitialize(0);
    if (result != S_OK && result != S_FALSE)
    {
        qDebug() << "Could not initialize OLE";
    }
    QAxObject *excel = new QAxObject(this);//建立excel操作对象
    if(!excel->setControl("ket.Application"))//连接Excel控件
        excel->setControl("Excel.Application");
    excel->setProperty("Visible", false);//显示窗体看效果,选择ture将会看到excel表格被打开
    excel->setProperty("DisplayAlerts", true);//显示警告看效果
    QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿(excel文件)集合
//    QString str = QFileDialog::getOpenFileName(this,"打开",
//                                               QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),"Excel 文件(*.xls *.xlsx)");
    workbooks->dynamicCall("Open(const QString&)", file);//打开刚才选定的excel
    QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
    QAxObject *worksheet = workbook->querySubObject("WorkSheets(int)",1);
    QAxObject *usedRange = worksheet->querySubObject("UsedRange");//获取表格中的数据范围

    QVariant var = usedRange->dynamicCall("Value");//将所有的数据读取刀QVariant容器中保存
    //    QList<QList<QVariant>> excel_list;//用于将QVariant转换为Qlist的二维数组
    QVariantList varRaws = var.toList();

    if(!mSpeedsList.isEmpty())
        mSpeedsList.clear();
    for(int i = 0; i < varRaws.size(); i++){
        mSpeedsList.push_back(varRaws[i].toList().front().toDouble());//https://www.cnblogs.com/leocc325/p/12916777.html
    }

    workbook->dynamicCall( "Close(Boolean)", false );
    excel->dynamicCall( "Quit(void)" );
    OleUninitialize();
    delete excel;//因为前面选择了不显示excel窗口，如果这里不删除excel的话，excel依然是打开的，只是我们看不见（因为被隐藏起来了）
    emit readDataFinished();
}
