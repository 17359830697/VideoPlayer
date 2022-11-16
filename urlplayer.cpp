#include "urlplayer.h"
#include "ui_urlplayer.h"

urlplayer::urlplayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::urlplayer)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(545,154));
    this->setWindowTitle(u8"视频播放器 - 输入 URL");
}

urlplayer::~urlplayer()
{
    delete ui;
}

void urlplayer::on_btn_ok_clicked() //OK
{
    QString urlpath = ui->le_url->text();
    emit sendurl(urlpath);
}

void urlplayer::on_btn_cancel_clicked()//cancel
{
    this->close();
}

void urlplayer::on_btn_clear_clicked()//clear
{
    ui->le_url->clear();
}
