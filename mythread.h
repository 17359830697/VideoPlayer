#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include<QThread>
class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    void run() override;//线程处理函数
signals:
    void isDone();

};

#endif // MYTHREAD_H
