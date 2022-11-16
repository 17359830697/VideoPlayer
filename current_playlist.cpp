#include "current_playlist.h"
#include "ui_current_playlist.h"
#include"mainwindow.h"

Current_Playlist::Current_Playlist(QWidget *parent) :
    QMainWindow(parent),selectrow(0),
    ui(new Ui::Current_Playlist)
{
    ui->setupUi(this);
    this->setWindowTitle("播放列表 - 顺序播放");
    ui->toolBar->setIconSize(QSize(35,35));
    this->Init_Database();
    this->Init_Table(selectrow);
    PlayMode();//播放模式
    connect(this,&Current_Playlist::update_table,[=](int row)
    {
        Init_Table(row);
    });
    connect(btn,&QToolButton::triggered,[=](QAction* action)
    {
        if(action->text()=="单曲循环")
        {
            emit playbackmode(QMediaPlaylist::CurrentItemInLoop);
            this->setWindowTitle("播放列表 - " + action->text());
        }
        else if(action->text() == "顺序播放")
        {
             emit playbackmode(QMediaPlaylist::Loop);
             this->setWindowTitle("播放列表 - " + action->text());
        }
        else if(action->text() == "随机播放")
        {
              emit playbackmode(QMediaPlaylist::Random);
             this->setWindowTitle("播放列表 - " + action->text());
        }

    });

   lineedit = new QLineEdit(ui->toolBar);
   lineedit->setGeometry(431,13,95,20);
   connect(lineedit,&QLineEdit::textChanged,[=](const QString &text)
   {
       if(text.isEmpty())
       {
           on_action_7_triggered();
           return;
       }
       QSqlQuery* query = new QSqlQuery(db3);
       QString delsql = "delete from seek";
       query->exec(delsql);
       QString sql = QString("select *from current where filename like '%%1%'").arg(text);
       query->exec(sql);
       while(query->next())
       {
           QString name = query->value(0).toString();
             sql = QString("insert into seek values('%1')").arg(name);
             query->exec(sql);
       }
       model = new QSqlTableModel(this,db3);
       model->setTable("seek");
       ui->tableView->setModel(model);
       model->select();
       model->setHeaderData(0,Qt::Horizontal,"名称");
       ui->tableView->horizontalHeader()->setStretchLastSection(true); //自动调整最后一列的宽度占满整个表格
       ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置不可编辑
       ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选择

   });

}

Current_Playlist::~Current_Playlist()
{
    delete ui;
}

void Current_Playlist::Init_Database()
{
    db3 = QSqlDatabase::addDatabase("QSQLITE","db3");
    db3.setDatabaseName("../VideoPlayer/currentfile.db");
    if(!db3.open())
        qDebug()<<"db3 not open!";
    db5 = QSqlDatabase::addDatabase("QSQLITE","db5");
    db5.setDatabaseName("../VideoPlayer/recentlyfile.db");
    if(!db5.open())
        qDebug()<<"db5 not open!";
    //清空数据库
    QSqlQuery* query = new QSqlQuery(db3);
    QString sql = "delete from current";
    query->exec(sql);
}

void Current_Playlist::Init_Table(int row)
{
    model = new QSqlTableModel(this,db3);
    model->setTable("current");
    ui->tableView->setModel(model);
    model->select();
    model->setHeaderData(0,Qt::Horizontal,"名称");
    ui->tableView->horizontalHeader()->setStretchLastSection(true); //自动调整最后一列的宽度占满整个表格
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置不可编辑
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选择
    selectrow =row;
    ui->tableView->selectRow(selectrow); //设置当前选中的行
}

void Current_Playlist::PlayMode() //播放模式
{
    QMenu* menu = new QMenu(this);
    menu->addAction("单曲循环");
    menu->addAction("顺序播放");
    menu->addAction("随机播放");
  btn = new QToolButton(ui->toolBar);
  btn->setGeometry(265,2,40,40);
  btn->setMenu(menu); //绑定一个菜单
  btn->setPopupMode(QToolButton::InstantPopup); //设置按钮点击弹出菜单（不设置则点击不会出现菜单）
  btn->setToolTip("播放模式"); //鼠标悬浮的工具提示
}

void Current_Playlist::on_action_S_triggered() //保存播放列表
{
   QString savepath =  QFileDialog::getSaveFileName(this,"选择一个文件","/home");
   //C:/Users/86173/Desktop/home
   if(savepath.isEmpty())
   {
       return;
   }
   QFile file(savepath);
   if(!file.open(QIODevice::ReadOnly|QIODevice::WriteOnly))
   {
       QMessageBox::warning(nullptr,"错误","文件打开失败!");
       return;
   }
   else
   {

   }
}

void Current_Playlist::on_action_triggered()//添加
{
    QStringList list = QFileDialog::getOpenFileNames(this,"选择一个文件","/home");
    //"C:/Users/86173/Desktop/video/学生请假系统（完整版）.mp4", "C:/Users/86173/Desktop/video/夜晚城市交通.mp4")
    QSqlQuery* query = new QSqlQuery(db3);
    for(int i = 0;i<list.size();i++)
    {
        QString name = list.at(i).section('/',-1,-1);
        QString sql = QString("insert into current values('%1')").arg(name);
        bool ret = query->exec(sql);
        if(!ret)
            qDebug()<<__LINE__<<"insert failed!";
    }
    emit update_table(selectrow);

}

void Current_Playlist::on_action_2_triggered()//删除
{
    //选中的模型
    QItemSelectionModel* smodel = ui->tableView->selectionModel();
    //取出选中模型的索引
    QModelIndexList list = smodel->selectedRows();
   //删除
    for(int i = 0;i<list.size();i++)
    {
        model->removeRow(list.at(i).row());
    }
    int number = selectrow - list.size();
    if(number<0)
        number = 0;
    emit update_table(number);
}

void Current_Playlist::on_action_3_triggered() //播放
{
    //选中的模型
    QItemSelectionModel* smodel = ui->tableView->selectionModel();
    QModelIndex model1 = smodel->currentIndex();
    QVariant var = model1.data();//QVariant 类似于联合体 QVariant(QString, "学生请假系统（完整版）.mp4")
    QString str = var.toString(); //"学生请假系统（完整版）.mp4"
    QSqlQuery* query = new QSqlQuery(db5);
    QString sql = QString("select *from recentlyF where filename = '%1'").arg(str);
    query->exec(sql);
    query->next();
    QString path = query->value(1).toString();
     emit playlist_play(path);
}

void Current_Playlist::on_action_4_triggered()//上一个
{
    //选中的模型
    QItemSelectionModel* smodel = ui->tableView->selectionModel();
    QModelIndex model1 = smodel->currentIndex();
    int num = model1.row();
    if(num == 0)
        return;
    QSqlQuery* query = new QSqlQuery(db3);
    QString sql = QString("select *from current");
    query->exec(sql);
    query->next();
    int flag = 0;
    while((num - 1)!=flag)
    {
        flag++;
        query->next();
    }
    QString path = query->value(0).toString();//夜晚城市交通
    qDebug()<<path;
    QSqlQuery* query1 = new QSqlQuery(db5);
    QString sql1 = QString("select *from recentlyF where filename = '%1'").arg(path);
    query1->exec(sql1);
    query1->next();
    QString paths = query1->value(1).toString();
     qDebug()<<paths;
    emit playlist_play(paths);
     emit update_table(flag);

}

void Current_Playlist::on_action_5_triggered()//下一个
{
    //选中的模型
    QItemSelectionModel* smodel = ui->tableView->selectionModel();
    QModelIndex model1 = smodel->currentIndex();
    int num = model1.row();
    QSqlQuery* query = new QSqlQuery(db3);
    QString sql = QString("select *from current");
    query->exec(sql);
    query->next();
    int flag = 0;
    while((num + 1)!=flag)
    {
        flag++;
        query->next();
    }
    if(query->value(0).toString().isEmpty())
        return;
    QString path = query->value(0).toString(); //夜晚城市交通
    QSqlQuery* query1 = new QSqlQuery(db5);
    QString sql1 = QString("select *from recentlyF where filename = '%1'").arg(path);
    query1->exec(sql1);
    query1->next();
    QString paths = query1->value(1).toString();
    emit playlist_play(paths);
    emit update_table(flag);
}

void Current_Playlist::on_tableView_doubleClicked(const QModelIndex &index)//双击播放
{
    QVariant var = index.data();//QVariant 类似于联合体 QVariant(QString, "学生请假系统（完整版）.mp4")
    QString str = var.toString(); //"学生请假系统（完整版）.mp4"
    QSqlQuery* query = new QSqlQuery(db5);
    QString sql = QString("select *from recentlyF where filename = '%1'").arg(str);
    query->exec(sql);
    query->next();
    QString path = query->value(1).toString();
     emit playlist_play(path);
}

void Current_Playlist::on_action_U_triggered()//向上移动
{
    int flag = --selectrow;
    if(flag<0)
    {
        selectrow = 0;
        return;
    }

    emit update_table(flag);
}

void Current_Playlist::on_action_D_triggered()//向下移动
{
    int temp = selectrow;
    int flag = ++selectrow;
    QSqlQuery* query = new QSqlQuery(db3);
    QString sql = QString("select *from current");
    query->exec(sql);
    query->next();
    int num = 0;
    while(num !=(temp+1))
    {
        num++;
        query->next();
    }
    if(query->value(0).toString().isEmpty())
    {
        selectrow = temp;
        return;
    }

    emit update_table(flag);
}
void Current_Playlist::on_action_7_triggered() //搜索
{
    lineedit->clear();
    Init_Table(0);

}
