#include "mythread.h"

MyThread::MyThread(QObject *parent) : QThread(parent)
{

}

void MyThread::run()
{
    QThread::sleep(2);//睡眠2s
    emit isDone();
}
