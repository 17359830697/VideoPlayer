#include "mainwindow.h"
#include "ui_mainwindow.h"
QString MainWindow::video_length = NULL;
int MainWindow::fps = 0;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),nowspeed("×1.0"),repetition_flag(false),
      A_sign_time(0),B_sign_time(0)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("视频播放器");
    ui->toolBar->setIconSize(QSize(38,38));//工具栏图标大小
    this->Init(); //初始化
    this->Init_Playlist();//清空数据库
    Signal_Slot();//信号槽
    InitRecentlyFile();//初始化最近文件列表

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)//事件过滤函数
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyevent = dynamic_cast<QKeyEvent*>(event);
        if(keyevent->key() == Qt::Key_Escape)//Esc
        {
            //退出全屏
            //首先恢复成子窗口
            videowidget->setWindowFlags(Qt::SubWindow);
            //退出全屏
            videowidget->showNormal();
            //videowidget在布局中,跟主窗口没啥关系，所以退出全屏后,不会在原来的位置,需要手动放置回去

            //设置父对象 因为对于窗口来说,有父对象就会显示在父对象上
            videowidget->setParent(this);
            //加入到布局管理器
            ui->videolayout->addWidget(videowidget); //如果this自带了布局，则会有两层布局，会干扰，导致回不去，this必须手动布局
        }
    }
    return QWidget::eventFilter(watched,event);
}

void MainWindow::Init()
{
    //指针对象实例化
    player = new QMediaPlayer(this);
    playerlist = new QMediaPlaylist(this);
    videowidget = new QVideoWidget(this);
    videowidget->installEventFilter(this);//安装事件过滤器,以外部窗口作为观察者
    ui->slider_volume->setRange(0,100); //设置声音滑轮条的范围

    url = new urlplayer; //给url窗口分配空间
    cplist = new Current_Playlist;//分配空间
    fav = new favorite;//收藏夹窗口分配空间
    jp = new jumpto;//跳转到窗口分配空间
    vm = new videomessage;//属性窗口分配空间
    prevolume = ui->slider_volume->value(); //初始化上一次的音量
    ui->slider_volume->setEnabled(false);
    ui->slider_time->setEnabled(false);


    //设置播放列表的播放顺序
    playerlist->setPlaybackMode(QMediaPlaylist::Loop);//默认是顺序播放,且最后一个项目播放完毕自动从第一个开始播放
    //设置播放者的播放列表
    player->setPlaylist(playerlist);
    //播放窗口加入布局管理器
    ui->videolayout->addWidget(videowidget);
    //设置播放器的显示窗口
    player->setVideoOutput(videowidget);



    //连接数据库
//    if(QSqlDatabase::contains("qt_sql_default_connection"))
//    db = QSqlDatabase::database("qt_sql_default_connection");
//    else
    db = QSqlDatabase::addDatabase("QSQLITE","db");
    db.setDatabaseName("../VideoPlayer/recentlyfile.db");
    if(!db.open())
    {
        qDebug()<<__LINE__<<" db open failed!";
    }

    db1 = QSqlDatabase::addDatabase("QSQLITE","db1");
    db1.setDatabaseName("../VideoPlayer/favoritefile.db");
    if(!db1.open())
    {
        qDebug()<<__LINE__<<" db1 open failed!";
    }
    db4 = QSqlDatabase::addDatabase("QSQLITE","db4");
    db4.setDatabaseName("../VideoPlayer/currentfile.db");
    if(!db4.open())
    {
        qDebug()<<__LINE__<<" db3 open failed!";
    }
   AddFileToFavorite();//更新菜单栏的收藏夹的内容

}

void MainWindow::Signal_Slot()//信号槽
{
    connect(url,SIGNAL(sendurl(QString)),this,SLOT(UrlPlay(QString)));//url播放的信号槽连接

    connect(&thread,&MyThread::isDone,[=]  //停止后文字延迟2s消失
    {
        ui->label_state->setText(" ");
    });
    connect(&thread1,&MyThread1::Is_Volume,[=]  //调整音量后文字延迟2s消失
    {
        ui->label_state->setText(" ");
    });
    connect(player,&QMediaPlayer::positionChanged,[=](qint64 position) //视频进度条更新时间槽函数
    {

        qint64 Video_duration =  player->duration(); //视频的总时长 ms为单位
        ui->slider_time->setRange(0,Video_duration); //设置进度条范围
        ui->slider_time->setValue(position); //更新进度条当前值

        qint64 H = Video_duration/1000/60/60;
        qint64 M = Video_duration/1000/60%60;
        qint64 s = Video_duration/1000%60;
        QString str = QString("%1:%2:%3").arg(H).arg(M).arg(s);
        video_length = str;

        qint64 now_time = position; //视频当前的时长 ms为单位
        qint64 now_H = now_time/1000/60/60;
        qint64 now_M = now_time/1000/60%60;
        qint64 now_s = now_time/1000%60;

        QString now_duartion = QString("%1:%2:%3/%4").arg(now_H).arg(now_M).arg(now_s).arg(str);
        QString now_duartion1 = now_duartion.section('/',0,0);

        ui->label_time->setText(nowspeed +"  "+ now_duartion);
        ui->label_sign->setText(A_sign + " " + B_sign);

        if(repetition_flag&&now_duartion1==B_sign_1)//重复播放，则循环A-B标记之间
        {
              player->setPosition(A_sign_time);
              ui->slider_time->setValue(A_sign_time);
        }


    });

    connect(ui->menu_R,SIGNAL(triggered(QAction*)),this,SLOT(RecentlyFile(QAction*)));//最近列表触发事件

    connect(this,SIGNAL(Update_recently()),this,SLOT(InitRecentlyFile()));//每次动作会更新最近列表

    connect(ui->menu_A_2,SIGNAL(triggered(QAction*)),this,SLOT(FavoriteFile(QAction*)));//收藏夹触发事件

    connect(fav,&favorite::Update,this,&MainWindow::AddFileToFavorite);//收藏夹编辑后更新菜单栏的动作

    connect(fav,&favorite::Path_Play,[=](QString name,QString path)
    {
        this->setWindowTitle(name + "-视频播放器");
        AddFileToPlaylist(path); //添加到当前播放列表
        ControlsChange();//控件状态更改
    });

    connect(cplist,&Current_Playlist::playlist_play,[=](QString path) //双击播放
    {
        player->setMedia(QUrl(path));
        player->play();
    });

    connect(cplist,&Current_Playlist::playbackmode,[=](QMediaPlaylist::PlaybackMode backmode) //播放模式槽函数
    {
        playerlist->setPlaybackMode(backmode);
    });
    connect(jp,&jumpto::sign_time,[=](int h,int m,int s) //跳转到槽函数
    {
        qint64 ms = h*60*60*1000 + m*60*1000 + s*1000;
        player->setPosition(ms);
    });

}

void MainWindow::ControlsChange()//控件状态更改
{
    if(player->state() == QMediaPlayer::PlayingState)//视频播放状态
    {
        ui->btn_start->setStyleSheet("QPushButton#btn_start{border-image:url(:/new/prefix1/src/zanting.png)}");
        ui->btn_back->setEnabled(true);
        ui->btn_go->setEnabled(true);
        ui->btn_stop->setEnabled(true);

    }
    else if(player->state() == QMediaPlayer::PausedState)//视频暂停状态
    {
        ui->btn_start->setStyleSheet("QPushButton#btn_start{border-image:url(:/new/prefix1/src/kaishi.png)}");

    }
    else if(player->state() == QMediaPlayer::StoppedState)//视频停止状态
    {
        ui->btn_start->setStyleSheet("QPushButton#btn_start{border-image:url(:/new/prefix1/src/kaishi.png)}");
        ui->btn_back->setEnabled(false);
        ui->btn_go->setEnabled(false);
        ui->btn_stop->setEnabled(false);

    }
    if(player->state() == QMediaPlayer::StoppedState)//视频停止状态 音量滑动条不允许使用
    {
        ui->slider_volume->setEnabled(false);
        ui->slider_time->setEnabled(false);
    }
    else
    {
        ui->slider_volume->setEnabled(true);
        ui->slider_time->setEnabled(true);
    }
    updatemessage();
}

void MainWindow::InitRecentlyFile()//初始化最近文件列表
{
    recentlyfile.clear(); //容器清空
    ui->menu_R->clear(); //菜单清空
    QSqlQuery* query = new QSqlQuery(db);
    QString addsql = QString("select *from recentlyF");
    query->exec(addsql);
    while(query->next())
    {
       recentlyfile.push_front(query->value(0).toString());
    }
    for(const auto &k:recentlyfile)
    {
      QAction* action = new QAction(ui->menu_R);
      action->setText(k);
      ui->menu_R->addAction(action);
    }
    //添加分隔线
    ui->menu_R->addSeparator();

    //添加清除动作
    QAction* action = new QAction(ui->menu_R);
    action->setText("清除");
    ui->menu_R->addAction(action);
    action->setIcon(QIcon(":/new/prefix1/src/clearrece.png"));

    //设置播放时显示当前播放的视频名称
    QString title = QString("%1-视频播放器").arg(now_play);
    this->setWindowTitle(title);
}

void MainWindow::AddFileToRecently(QString name,QString path)//添加文件到最近列表
{
    if(name==NULL) //文件名为空,直接返回
     {
        return;
    }
    QSqlQuery* query = new QSqlQuery(db);
    QString selectsql = QString("select *from recentlyF");
    query->exec(selectsql);
    bool Is_New = true;//否是新的视频文件标志
    while(query->next())
    {   //遍历判断是否是新的视频文件
        if(query->value(0).toString() == name && query->value(1).toString() == path)
        {
            Is_New = false;
            break;
        }
    }

    if(Is_New) //如果是新的视频文件,则添加
    {
        QString sql = QString("insert into recentlyF values('%1','%2')").arg(name).arg(path);
        bool ret = query->exec(sql);
        if(!ret)
            qDebug()<<__LINE__<<"添加失败";
    }
    else  //不是新的视频文件
    {
        //将其更新到最近的位置 (先删除,再重新插入,移动)
        QString delsql = QString("delete from recentlyF where filename = '%1' and filepath = '%2'").arg(name).arg(path);
        qDebug()<<query->exec(delsql);
        QString sql = QString("insert into recentlyF values('%1','%2')").arg(name).arg(path);
        query->exec(sql);
    }
    emit Update_recently();
}

void MainWindow::AddFileToFavorite()//添加文件到收藏夹的动作
{
    ui->menu_A_2->clear();

    QAction* action_edit = new QAction(ui->menu_A_2);
    action_edit->setText("编辑(E)...");
    ui->menu_A_2->addAction(action_edit);

    QAction* action_add = new QAction(ui->menu_A_2);
    action_add->setText("添加当前媒体");
    ui->menu_A_2->addAction(action_add);

    ui->menu_A_2->addSeparator();//添加分割线

    QSqlQuery* query = new QSqlQuery(db1);
    QString sql = "select *from favorite";
    query->exec(sql);
    while(query->next())
    {
        QAction* action = new QAction(ui->menu_A_2);
        action->setText(query->value(1).toString());
        ui->menu_A_2->addAction(action);
    }
}

void MainWindow::AddCurrentPlay()//添加当前媒体
{
    QString filepath = playerlist->currentMedia().request().url().toString(); //获取当前播放媒体的路径
    if(filepath.isEmpty())
        return;
    QString filename = filepath.section('/',-1,-1);
    QSqlQuery* query2 = new QSqlQuery(db1);
    QString selectsql = QString("select *from favorite");
    query2->exec(selectsql);
    bool Is_New = true;//否是新的视频文件标志
    while(query2->next())
    {   //遍历判断是否是新的视频文件
        if(query2->value(1).toString() == filename && query2->value(2).toString() == filepath)
        {
            Is_New = false;
            break;
        }
    }

    if(Is_New) //如果是新的视频文件,则添加
    {
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd"); //获取当前系统时间并格式化时间 yyyy-MM-dd hh:mm:ss
        QSqlQuery* query = new QSqlQuery(db1);
        QString sql = QString("insert into favorite values('%1','%2','%3')").arg(time).arg(filename).arg(filepath);
        bool ret = query->exec(sql);
        if(!ret)
            qDebug()<<"add failed!";
    }
    else  //不是新的视频文件
    {
        QMessageBox::information(this,"提示","该媒体在收藏夹已存在!");
        return;
    }
    emit fav->Add_current_player(); //发送信号
    AddFileToFavorite();//更新菜单栏的收藏夹的内容
    ControlsChange();//控件状态更改
}

void MainWindow::Init_Playlist()
{
    QSqlQuery* query = new QSqlQuery(db4);
    QString sql = "delete from current";
    query->exec(sql);
}

void MainWindow::AddFileToPlaylist(QString path) //添加文件到当前播放列表
{
    playerlist->addMedia(QUrl(path));
    int index = playerlist->currentIndex();
    playerlist->setCurrentIndex(++index);
    player->play();
    QString name = path.section('/',-1,-1);
    QSqlQuery* query = new QSqlQuery(db4);
    QString sql = QString("insert into current values('%1')").arg(name);
    query->exec(sql);
    emit cplist->update_table(index);//发送信号,更新表格
}

void MainWindow::AddFileToPlaylist(QStringList pathlist)//添加文件到当前播放列表
{
    QSqlQuery* query = new QSqlQuery(db4);
    for(int i = 0;i<pathlist.size();i++)
    {
        playerlist->addMedia(QUrl(pathlist.at(i)));

        QString name = pathlist.at(i).section('/',-1,-1);
        QString sql = QString("insert into current values('%1')").arg(name);
        query->exec(sql);
    }
    int index = playerlist->currentIndex();
    playerlist->setCurrentIndex(++index);
    player->play();
    emit cplist->update_table(index); //发送信号,更新表格
}

void MainWindow::updatemessage()//更新视频属性信息
{
    QString path = player->currentMedia().request().url().toString();
    QString name = path.section('/',-1,-1);
     String cstr = String((const char *)path.toLocal8Bit()); //QString转String 解决乱码
    VideoCapture cap(cstr); //QString 转成String
    if(!cap.isOpened())
        qDebug()<<"not open";
     int sum = cap.get(CAP_PROP_FRAME_COUNT);//总帧数
     fps = cap.get(CAP_PROP_FPS);//帧率
     int width = cap.get(CAP_PROP_FRAME_WIDTH);//帧宽度
     int  height = cap.get(CAP_PROP_FRAME_HEIGHT);//帧高度
     QString format = name.section('.',-1,-1); //视频格式
     QString fen = QString("%1 × %2").arg(width).arg(height);//视频分辨率
     QFileInfo info(path);
     qint64 size = info.size();
     emit vm->message(name,path,format,video_length,fen,sum,fps,size);
}

void MainWindow::RecentlyFile(QAction *action)//最近的文件槽函数
{

   QString filename = action->text();
   now_play = filename; //更新当前播放视频名称
   QSqlQuery* query = new QSqlQuery(db);
   if(filename == "清除")
   {
       QString delsql = QString("delete from recentlyF");
        query->exec(delsql);
        ui->menu_R->clear(); //菜单清空
        return;
   }
   QString selectsql = QString("select *from recentlyF where filename = '%1'").arg(filename);
   query->exec(selectsql);
   query->next();
   QString path = query->value(1).toString();
   AddFileToPlaylist(path); //添加到当前播放列表
   ControlsChange();//控件状态更改
   AddFileToRecently(filename,path);
   emit Update_recently();
}

void MainWindow::FavoriteFile(QAction *action)//收藏夹槽函数
{
    QString text = action->text();
    if(text=="编辑(E)...")
    {
        fav->show();
        return;
    }
    else if(text == "添加当前媒体")
    {
       AddCurrentPlay();//添加当前媒体
        return;
     //   QFileInfo file(filepath);
     //   QFileIconProvider icon_provider;
     //   QIcon icon = icon_provider.icon(file); //获取文件的图标
     //   QString type = icon_provider.type(file);//获取文件类型
    }
    QSqlQuery* query = new QSqlQuery(db1);
    QString sql = QString("select *from favorite where filename = '%1'").arg(text);
    query->exec(sql);
    query->next();
    QString path = query->value(2).toString();
    AddFileToPlaylist(path); //添加到当前播放列表
    ControlsChange();//控件状态更改

}

void MainWindow::UrlPlay(QString urlpath)//url播放槽函数
{
    if(urlpath.isEmpty())
        return;
     AddFileToPlaylist(urlpath);//添加到当前播放列表
     this->close();
      ControlsChange();//控件状态更改
}

void MainWindow::on_action_F_triggered()//打开文件
{
    QString path = QFileDialog::getOpenFileName(this,"选择一个文件","/home",
  tr("视频(*.mp4 *.avi *.mpeg *.vcd *.m4v)\n音频( *.mp3 *.wav *.ogg *.oga)\n播放列表(*.m3u *.m3u8 *.pls)\n所有文件(*.*)"));
    if(path.isEmpty())
        return;

    AddFileToPlaylist(path); //添加到播放列表,并从该曲目开始播放
    ControlsChange();//控件状态更改

    //获取文件名
    QString name = path.section('/',-1,-1);
    AddFileToRecently(name,path);
    now_play = name;
    emit Update_recently();//发送更新最近列表的信号

}

void MainWindow::on_action_F_2_triggered() //打开文件(工具栏)
{
    on_action_F_triggered();
}

void MainWindow::on_actionURL_U_triggered()//URL播放
{
    url->show();
}

void MainWindow::on_actionURL_U_2_triggered()//URL播放(工具栏)
{
    on_actionURL_U_triggered();
}

void MainWindow::on_btn_start_clicked() // 开始/暂停
{
    if(player->state() == QMediaPlayer::PlayingState) //视频处于播放状态
    {
        player->pause();//视频暂停
         ui->label_state->setText("暂停");
    }
    else
    {
        player->play();//视频暂停
        ui->label_state->setText(NULL);
    }
    ControlsChange();//控件状态更改
}

void MainWindow::on_btn_stop_clicked()//停止
{
    player->stop();
    ui->label_state->setText("停止");
    thread.start();//启动线程处理函数
    ControlsChange();//控件状态更改

    //    QPropertyAnimation *animation = new QPropertyAnimation(ui->label_state,"windowOpacity"); //透明度动画属性
    //    animation->setDuration(5000); //动画持续时间2s
    //    animation->setStartValue(1); //初始属性 1 （完全不透明）
    //    animation->setEndValue(0); //结束属性 0(完全透明)
    //    animation->start();
}

void MainWindow::on_slider_volume_sliderMoved(int position)//音量滑动条(滑条移动)
{
    if(position == 0)
    ui->btn_volume->setStyleSheet("QPushButton#btn_volume{border-image:url(:/new/prefix1/src/volumeCross.png)}");
    else if(position>0&&position<=30)
    ui->btn_volume->setStyleSheet("QPushButton#btn_volume{border-image:url(:/new/prefix1/src/volumeLow.png)}");
    else if(position>30&&position<=70)
    ui->btn_volume->setStyleSheet("QPushButton#btn_volume{border-image:url(:/new/prefix1/src/volumeMiddle.png)}");
    else if(position>70)
    ui->btn_volume->setStyleSheet("QPushButton#btn_volume{border-image:url(:/new/prefix1/src/volumeHigh.png)}");
    player->setVolume(position);

    QString text = QString("音量:%1").arg(position);
    if(position>=prevolume)
        ui->label_state->setText(text);
    else if(position<prevolume)
        ui->label_state->setText(text);

    thread1.start();

}

void MainWindow::on_btn_volume_clicked() //音量按钮
{
    int volume = player->volume();
    if(volume != 0) //当前有声音,点击则静音
    {
        ui->slider_volume->setValue(0);
        on_slider_volume_sliderMoved(0);
    }
    else if(volume == 0) //当前是静音,点击回到上一次的音量状态
    {
        ui->slider_volume->setValue(prevolume);
        on_slider_volume_sliderMoved(prevolume);
    }
}

void MainWindow::on_slider_volume_valueChanged(int value) //音量滑动条(值改变)
{
    player->setVolume(value);
}

void MainWindow::on_btn_setbig_clicked()//最大化
{
    //qt通过子窗口的成员函数Showfullscreen可以实现窗口全屏,但是只允许顶层窗口全屏,
    //子窗口如果想要全屏必须先升级成顶级窗口->调用 setWindowFlags(Qt::window);
    //升级为顶层窗口
    videowidget->setWindowFlags(Qt::Window);
    //全屏
    videowidget->showFullScreen();
}

void MainWindow::on_slider_time_sliderMoved(int position)//滑动进度条
{
    player->setPosition(position);
    qint64 now_H = position/1000/60/60;
    qint64 now_M = position/1000/60%60;
    qint64 now_s = position/1000%60;
    QString now_duartion = QString("%1:%2:%3").arg(now_H).arg(now_M).arg(now_s);
    ui->label_state->setText("跳转到" + now_duartion);

}

void MainWindow::on_slider_time_sliderReleased()//释放滑动条
{
    ui->label_state->setText(" ");
}

void MainWindow::on_btn_go_clicked()//快进10s
{
    qint64 nowtime = player->position();
    player->setPosition(nowtime+10000);
    ui->slider_time->setValue(nowtime+10000);
}

void MainWindow::on_btn_back_clicked()//后退10s
{
    qint64 nowtime = player->position();
    player->setPosition(nowtime-10000);
    ui->slider_time->setValue(nowtime-10000);
}

void MainWindow::on_action_L_triggered()//关闭
{
    this->close();
}

void MainWindow::on_action_P_triggered()//选择播放列表(打开)
{
    QStringList paths = QFileDialog::getOpenFileNames(this,"选择一个文件","/home",tr("视频(*.mp4 *.avi *.mpeg *.vcd *.m4v)\n所有文件(*.*)"));
    if(paths.isEmpty())
       return;
    AddFileToPlaylist(paths);
    for(const auto& k:paths)
    {
        QString name = k.section('/',-1,-1);
        AddFileToRecently(name,k);
    }
    player->play();
    ControlsChange();//控件状态更改
    emit Update_recently();
}

void MainWindow::on_action_P_2_triggered()//查看播放列表(工具栏)
{
    on_action_L_3_triggered();
}

void MainWindow::on_action_i_triggered()//目录
{
   QString contentpath = QFileDialog::getExistingDirectory(nullptr,"选择一个目录","/home");
   if(contentpath.isEmpty())
       return;

  QDir path(contentpath);
  QStringList nameFilter;  //过滤器,可以为空,为空则表示显示所有文件
  //QDir::Dirs 作用：显示文件夹  QDir::Files 作用：显示文件（不是文件夹）
  //QDir::NoDotAndDotDot 作用：去除 "."  ".."   它们分别表示为当前目录、上一级目录
  QStringList clist = path.entryList(QDir::Files);
  QStringList pathlist;
  int num = 0;
  for(const auto& k:clist)
  {
      QString path = contentpath + "/" + k;
      pathlist.insert(num++,path);
      AddFileToRecently(k,path);
  }
  AddFileToPlaylist(pathlist); //添加到当前播放列表
  ControlsChange();//控件状态更改
}

void MainWindow::on_action_L_3_triggered() //查看当前播放列表
{
    cplist->show();
}

void MainWindow::on_action_V_triggered() //上一个(菜单栏)
{
    int currentindex = playerlist->currentIndex();
    playerlist->setCurrentIndex(currentindex-1);
    player->play();
}

void MainWindow::on_action_N_triggered()//下一个(菜单栏)
{
    int currentindex = playerlist->currentIndex();
    playerlist->setCurrentIndex(currentindex+1);
    player->play();
}

void MainWindow::on_action_triggered()//上一个（工具栏）
{
    on_action_V_triggered();
}

void MainWindow::on_action_2_triggered()//下一个（工具栏）
{
    on_action_N_triggered();
}


void MainWindow::on_action_A_2_triggered()//编辑收藏夹(工具栏)
{
    fav->show();
    emit fav->Add_current_player(); //发送信号
}

void MainWindow::on_action_L_2_triggered()//播放（菜单栏）
{
    if(player->state() == QMediaPlayer::PlayingState) //视频处于播放状态
    {
        return;
    }
    else //视频暂停
    {
        player->play();
        ui->label_state->setText(NULL);
    }
    ControlsChange();//控件状态更改
}

void MainWindow::on_action_P_3_triggered()//暂停(菜单栏)
{
    if(player->state() == QMediaPlayer::PlayingState) //视频处于播放状态
    {
        player->pause();//视频暂停
         ui->label_state->setText("暂停");
    }
    else
    {
       return;
    }
    ControlsChange();//控件状态更改
}

void MainWindow::on_action_S_2_triggered()//停止(菜单栏)
{
    on_btn_stop_clicked();//停止
}

void MainWindow::on_action_10_triggered()// -10秒(菜单栏)
{
   on_btn_back_clicked();
}

void MainWindow::on_action_11_triggered()// +10秒(菜单栏)
{
   on_btn_go_clicked();
}

void MainWindow::on_action_1_triggered()// -1分(菜单栏)
{
   qint64 nowtime = player->position(); //获取当前播放者进度 ms
   //-1分钟即后退 60000ms
   player->setPosition(nowtime - 60000);
   ui->slider_time->setValue(nowtime - 60000);
}

void MainWindow::on_action_4_triggered()// +1分(菜单栏)
{
    qint64 nowtime = player->position(); //获取当前播放者进度 ms
    player->setPosition(nowtime + 60000);
    ui->slider_time->setValue(nowtime + 60000);
}

void MainWindow::on_action_12_triggered()// -10分(菜单栏)
{
    qint64 nowtime = player->position(); //获取当前播放者进度 ms
    //-10分钟即后退 600000ms
    player->setPosition(nowtime - 600000);
    ui->slider_time->setValue(nowtime - 600000);
}

void MainWindow::on_action_13_triggered()// +10分(菜单栏)
{
    qint64 nowtime = player->position(); //获取当前播放者进度 ms
    //-10分钟即后退 600000ms
    player->setPosition(nowtime + 600000);
    ui->slider_time->setValue(nowtime + 600000);
}

void MainWindow::on_action_1_0_triggered()//×0.5
{
    player->setPlaybackRate(0.5);
    nowspeed = "×0.5";
}

void MainWindow::on_action_0_75_triggered()//×0.75
{
    player->setPlaybackRate(0.75);
    nowspeed = "×0.75";
}

void MainWindow::on_action_1_1_triggered()//×1.0
{
    player->setPlaybackRate(1.0);
    nowspeed = "×1.0";
}

void MainWindow::on_action_1_25_triggered()//×1.25
{
    player->setPlaybackRate(1.25);
    nowspeed = "×1.25";
}

void MainWindow::on_action_1_5_triggered()//×1.5
{
    player->setPlaybackRate(1.5);
    nowspeed = "×1.5";
}

void MainWindow::on_action_2_0_triggered()//×2.0
{
    player->setPlaybackRate(2.0);
    nowspeed = "×2.0";
}

void MainWindow::on_action_3_0_triggered()//×3.0
{
    player->setPlaybackRate(3.0);
    nowspeed = "×3.0";
}

void MainWindow::on_action_A_A_triggered() //设置A标记
{
    qint64 nowtime = player->position();
    A_sign_time = nowtime;
    int H = nowtime/1000/60/60;
    int M = nowtime/1000/60%60;
    int s = nowtime/1000%60;
    QString Asign = QString("A:%1:%2:%3").arg(H).arg(M).arg(s);
    A_sign = Asign;
}

void MainWindow::on_action_B_B_triggered() //设置B标记
{
    qint64 nowtime = player->position();
    B_sign_time = nowtime;
    int H = nowtime/1000/60/60;
    int M = nowtime/1000/60%60;
    int s = nowtime/1000%60;
    QString Bsign = QString("B:%1:%2:%3").arg(H).arg(M).arg(s);
    B_sign_1 = Bsign.section(':',1,3);
    B_sign = Bsign;
}

void MainWindow::on_action_A_B_C_triggered() //清除A-B标记
{
     ui->label_sign->clear();
     A_sign.clear();
     B_sign.clear();
     A_sign_time = 0;
     B_sign_time = 0;
}

void MainWindow::on_action_R_triggered() //重复
{
    repetition_flag = !repetition_flag;
    if(repetition_flag)
    {
        player->setPosition(A_sign_time);
        ui->slider_time->setValue(A_sign_time);
    }
}

void MainWindow::on_action_J_triggered() //跳转到
{
    jp->show();
}

void MainWindow::on_action_3_triggered() //倒放
{
    QString path = player->currentMedia().request().url().toString();
    QString file("../VideoPlayer/pic");
    QFile filepath(file);
    QDir dir;
    if(dir.exists(file))
    {
        qDebug()<<"exist";
        dir.setPath(file);
        dir.removeRecursively(); //删除整个文件夹
    }
    dir.mkpath("./pic");//创建文件夹

     String cstr = String((const char *)path.toLocal8Bit()); //QString转String 解决乱码
    VideoCapture cap(cstr); //QString 转成String
    if(!cap.isOpened())
        qDebug()<<"not open";

    int num = 0;

    while(1)
    {
         Mat frame;
        cap>>frame;//取一帧
        if(frame.empty())
            break;
        num++;
        String name = format("../VideoPlayer/pic/%d.png",num);
        imwrite(name,frame);
        waitKey(1);
    }

    while(1)
    {
        if(num==0)
            break;
        String name = format("../VideoPlayer/pic/%d.png",num--);
        Mat frame = imread(name);
        if(frame.empty())
            break;
        imshow("backplay",frame);
        if(waitKey(40)==27) //esc键退出
            break;
    }
   destroyAllWindows();//销毁窗口

}

void MainWindow::on_action_S_triggered() //截图
{
    QString path = QFileDialog::getSaveFileName(this,"保存","/home","*.png");
    //"C:/Users/86173/Desktop/home.png"
//    QDesktopWidget *desk = QApplication::desktop();
//    QScreen* screen = QGuiApplication::primaryScreen();
    QPixmap pixmap = QPixmap::grabWidget(this); //截取窗口内容
    pixmap.save(path); //保存
}

void MainWindow::on_action_I_triggered()//查看视频信息
{
    updatemessage();//更新视频信息
    vm->show();
}

void MainWindow::on_action_I_2_triggered()//查看视频信息(菜单栏)
{
    on_action_I_triggered();
}

void MainWindow::on_action_Y_triggered()//百度
{
    QDesktopServices::openUrl(QUrl("https://www.baidu.com"));
}

void MainWindow::on_action_Y_2_triggered()//百度(菜单栏)
{
    on_action_Y_triggered();
}

void MainWindow::on_action_5_triggered()
{
    QMessageBox::information(this,"帮助说明","该软件外观及功能参考了SMPlayer,只完成了基本功能,许多高级功能还未开发!");
}

void MainWindow::on_action_F_3_triggered()//逐帧步进
{
    //计算一帧所需时间
    int fps_time = 1000/fps;
    quint64 now_time = player->position();//获取当前的位置
    quint64 new_time = now_time+fps_time;
    player->setPosition(new_time);
    qDebug()<<"now:"<<new_time;
     player->pause();

}

void MainWindow::on_action_m_triggered()//逐帧步退
{
    //计算一帧所需时间
    int fps_time = 1000/fps;
    quint64 now_time = player->position();//获取当前的位置
    quint64 new_time = now_time-fps_time;
    player->setPosition(new_time);
    qDebug()<<"now:"<<new_time;
     player->pause();
}
