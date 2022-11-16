#ifndef CURRENT_PLAYLIST_H
#define CURRENT_PLAYLIST_H
#pragma execution_character_set("utf-8")
#include <QMainWindow>
#include<QMediaPlaylist>
#include<QNetworkRequest>
#include<QMediaPlayer>
#include<QFileDialog>
#include<QFile>
#include<QItemSelectionModel> //选择的模型
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlTableModel>
#include<QToolBox>
#include<QToolButton>
#include<QLineEdit>
namespace Ui {
class Current_Playlist;
}

class Current_Playlist : public QMainWindow
{
    Q_OBJECT
    friend class MyThread2;

public:
    explicit Current_Playlist(QWidget *parent = nullptr);
    ~Current_Playlist();
   void Init_Database();
   void Init_Table(int row);

   void PlayMode();//播放模式



signals:
   void update_table(int num);
   void playlist_play(QString path);
   void playbackmode(QMediaPlaylist::PlaybackMode backmode);
private slots:
    void on_action_S_triggered();

    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_action_4_triggered();

    void on_action_5_triggered();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_action_U_triggered();

    void on_action_D_triggered();


    void on_action_7_triggered();

private:
    Ui::Current_Playlist *ui;
    int selectrow; //播放列表当前媒体数量
    QSqlDatabase db3;
    QSqlDatabase db5;
    QSqlTableModel* model;
    QToolButton* btn; //播放模式下拉按钮
    QToolButton* seekbtn;//搜索下拉按钮
    QLineEdit* lineedit;//搜索框

};

#endif // CURRENT_PLAYLIST_H
