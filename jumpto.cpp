#include "jumpto.h"
#include "ui_jumpto.h"

jumpto::jumpto(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::jumpto)
{
    ui->setupUi(this);
    this->setWindowTitle("视频播放器");
    timeedit = new QTimeEdit(this);
    timeedit->resize(106,39);
    timeedit->setDisplayFormat("HH:mm:ss"); //yyyy-MM-dd 设置timeEdit的显示样式
    ui->horizontalLayout_2->addWidget(timeedit); //将timeEdit加入到布局中

}

jumpto::~jumpto()
{
    delete ui;
}

void jumpto::on_pushButton_2_clicked()//cancel
{
    this->close();
}


void jumpto::on_pushButton_clicked()//OK
{
    int h = timeedit->time().hour();
    int m = timeedit->time().minute();
    int s = timeedit->time().second();
    emit sign_time(h,m,s);
}
