#ifndef JUMPTO_H
#define JUMPTO_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include<QValidator> //验证类(限制LineEdit输入文本范围及类型)
#include<QTimeEdit>
namespace Ui {
class jumpto;
}

class jumpto : public QWidget
{
    Q_OBJECT

public:
    explicit jumpto(QWidget *parent = nullptr);
    ~jumpto();
signals:
    void sign_time(int h,int m,int s);

private slots:
    void on_pushButton_2_clicked();


    void on_pushButton_clicked();

private:
    Ui::jumpto *ui;
    QTimeEdit* timeedit;
};

#endif // JUMPTO_H
