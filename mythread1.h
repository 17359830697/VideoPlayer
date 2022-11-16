#ifndef MYTHREAD1_H
#define MYTHREAD1_H

#include <QObject>
#include<QThread>
class MyThread1 : public QThread
{
    Q_OBJECT
public:
    explicit MyThread1(QObject *parent = nullptr);
    void run() override;

signals:
    void Is_Volume();

};

#endif // MYTHREAD1_H
