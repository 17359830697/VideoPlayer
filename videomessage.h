#ifndef VIDEOMESSAGE_H
#define VIDEOMESSAGE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include<opencv2/opencv.hpp>
using namespace cv;
namespace Ui {
class videomessage;
}

class videomessage : public QWidget
{
    Q_OBJECT

public:
    explicit videomessage(QWidget *parent = nullptr);
    ~videomessage();
signals:
    void message(QString filename,QString filepath,QString format,QString len,QString fen,int sum,int fps,qint64 size);

public slots:
    void update(QString filename,QString filepath,QString format,QString len,QString fen,int sum,int fps,qint64 size);

private:
    Ui::videomessage *ui;
};

#endif // VIDEOMESSAGE_H
