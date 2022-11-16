#ifndef URLPLAYER_H
#define URLPLAYER_H

#include <QWidget>

namespace Ui {
class urlplayer;
}

class urlplayer : public QWidget
{
    Q_OBJECT

public:
    explicit urlplayer(QWidget *parent = nullptr);
    ~urlplayer();
signals:
    void sendurl(QString path);

private slots:
    void on_btn_ok_clicked();

    void on_btn_cancel_clicked();

    void on_btn_clear_clicked();

private:
    Ui::urlplayer *ui;
};

#endif // URLPLAYER_H
