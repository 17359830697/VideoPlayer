#include "videomessage.h"
#include "ui_videomessage.h"
#include<QDebug>
videomessage::videomessage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::videomessage)
{
    ui->setupUi(this);
    this->setWindowTitle("文件属性");
   connect(this,SIGNAL(message(QString,QString,QString,QString,QString,int,int,qint64)),this,
           SLOT(update(QString,QString,QString,QString,QString,int,int,qint64)));
}

videomessage::~videomessage()
{
    delete ui;
}

void videomessage::update(QString filename, QString filepath, QString format, QString len, QString fen, int sum, int fps,qint64 size)
{
    ui->label_filename->setText(filename);
    ui->lb_name->setText(filepath);
    ui->lb_format->setText(format);
    ui->lb_length->setText(len);
    ui->lb_fen->setText(fen);
    ui->lb_count->setText(QString::number(sum));
    ui->lb_rate->setText(QString::number(fps));
    int mb = size/1024/1024 ;
    QString str_mb = QString("(%1MB)").arg(mb);
    QString str = QString("%1kb%2").arg(size/1024).arg(str_mb);
    ui->lb_size->setText(str);
}
