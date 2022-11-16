#ifndef FAVORITE_H
#define FAVORITE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QDebug>
#include<QSqlTableModel>
#include<QSqlRecord>//数据记录
#include<QItemSelectionModel>
namespace Ui {
class favorite;
}

class favorite : public QWidget
{
    Q_OBJECT

public:
    explicit favorite(QWidget *parent = nullptr);
    ~favorite();

    void Init();//初始化

    void Init_Table(int row);//初始化表格内容

    void Init_Database();//初始化数据库

signals:
    void Add_current_player();//添加当前媒体信号
    void Update();//修改内容信号
    void Path_Play(QString name,QString path);//收藏夹的播放信号

private slots:
    void on_btn_Ok_clicked();

    void on_btn_cancel_clicked();

    void on_btn_AddNew_clicked();

    void on_btn_Del_clicked();

    void on_btn_DelAll_clicked();

    void on_btn_Up_clicked();

    void on_btn_Down_clicked();

    void on_btn_Play_clicked();

    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::favorite *ui;
    QSqlDatabase db2;
    QSqlTableModel* model = nullptr;//数据模型
    int selectrow;
};

#endif // FAVORITE_H
