#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma execution_character_set("utf-8")
#include <QMainWindow>
#include<QFileDialog>
#include<QFile>
#include<QMediaPlayer> //播放者
#include<QMediaPlaylist> //播放列表
#include<QVideoWidget> //播放窗口
#include<QPropertyAnimation> //动画
#include<QGraphicsOpacityEffect>//透明属性
#include<QKeyEvent>
#include<QMessageBox>
#include<QAction> //菜单动作
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QFileIconProvider> //文件图标提供者
#include<QDateTime>
#include<opencv2/opencv.hpp>
#include<string>
#include<QFileInfo>//文件信息
#include<QDesktopServices>//打开外部Url
#include<QDir>
#include"urlplayer.h"
#include"mythread.h"
#include"mythread1.h"
#include"current_playlist.h"
#include"favorite.h"
#include"jumpto.h"
#include"videomessage.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
using namespace cv;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class Current_Playlist;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool eventFilter(QObject *watched, QEvent *event); //事件过滤函数

    void Init();//初始化

    void Signal_Slot();//信号槽

    void ControlsChange();//控件状态更改

    void AddFileToRecently(QString name,QString path);//添加文件到最近列表

    void AddFileToFavorite();//添加文件到收藏夹的动作

    void AddCurrentPlay();//添加当前媒体

    void Init_Playlist();//清空数据库

    void AddFileToPlaylist(QString path);

    void AddFileToPlaylist(QStringList pathlist);

    void updatemessage();//更新视频属性信息



 signals:
    void Update_recently();

private slots:
    void UrlPlay(QString urlpath);//URL播放

    void InitRecentlyFile();//初始化最近文件列表
    void RecentlyFile(QAction *action);//最近的文件槽函数
    void FavoriteFile(QAction *action);//收藏夹槽函数


private slots:
    void on_action_F_triggered();

    void on_action_F_2_triggered();

    void on_actionURL_U_triggered();

    void on_actionURL_U_2_triggered();

    void on_btn_start_clicked();

    void on_btn_stop_clicked();

    void on_slider_volume_sliderMoved(int position);

    void on_btn_volume_clicked();

    void on_slider_volume_valueChanged(int value);

    void on_btn_setbig_clicked();


    void on_slider_time_sliderMoved(int position);


    void on_slider_time_sliderReleased();


    void on_btn_go_clicked();

    void on_btn_back_clicked();

    void on_action_L_triggered();

    void on_action_P_triggered();

    void on_action_P_2_triggered();


    void on_action_i_triggered();

    void on_action_L_3_triggered();

    void on_action_V_triggered();

    void on_action_N_triggered();

    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_A_2_triggered();


    void on_action_L_2_triggered();

    void on_action_P_3_triggered();

    void on_action_S_2_triggered();

    void on_action_10_triggered();

    void on_action_11_triggered();

    void on_action_1_triggered();

    void on_action_4_triggered();

    void on_action_12_triggered();

    void on_action_13_triggered();

    void on_action_1_0_triggered();

    void on_action_0_75_triggered();

    void on_action_1_1_triggered();

    void on_action_1_25_triggered();

    void on_action_1_5_triggered();

    void on_action_2_0_triggered();

    void on_action_3_0_triggered();

    void on_action_A_A_triggered();

    void on_action_B_B_triggered();

    void on_action_A_B_C_triggered();

    void on_action_R_triggered();

    void on_action_J_triggered();

    void on_action_3_triggered();

    void on_action_S_triggered();

    void on_action_I_triggered();

    void on_action_I_2_triggered();

    void on_action_Y_triggered();

    void on_action_Y_2_triggered();

    void on_action_5_triggered();

    void on_action_F_3_triggered();


    void on_action_m_triggered();

private:
    Ui::MainWindow *ui;
    QMediaPlayer* player;
    QMediaPlaylist* playerlist;
    QVideoWidget* videowidget;
    urlplayer *url; 
    Current_Playlist *cplist;
    favorite* fav;
    jumpto* jp;
    videomessage* vm;
    int prevolume;//记录上一次的音量
    MyThread thread; //停止文字消失线程
    MyThread1 thread1; //音量文字消失线程
    QList<QString> recentlyfile;//存放最近文件的容器
    QString now_play;//记录当前播放视频名称
    QSqlDatabase db;
    QSqlDatabase db1;
    QSqlDatabase db4;
    QString nowspeed;//当前倍速
    QString A_sign;//A标记
    QString B_sign;//B标记
    QString B_sign_1;//少B的B标记
    qint64 A_sign_time;//A标记时间
    qint64 B_sign_time;//B标记时间
    bool repetition_flag;//是否重复标志
    static QString video_length;
    static int fps;

};
#endif // MAINWINDOW_H
