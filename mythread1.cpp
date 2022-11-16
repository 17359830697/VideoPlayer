#include "mythread1.h"

MyThread1::MyThread1(QObject *parent) : QThread(parent)
{

}

void MyThread1::run()
{
    QThread::sleep(2);
    emit Is_Volume();
}
