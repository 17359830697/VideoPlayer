#include "favorite.h"
#include "ui_favorite.h"

favorite::favorite(QWidget *parent) :
    QWidget(parent),selectrow(0),
    ui(new Ui::favorite)
{
    ui->setupUi(this);
    this->setWindowTitle("收藏夹编辑器");
     this->Init_Database(); //初始化数据库
    this->Init(); //初始化按钮图标
    this->Init_Table(selectrow); //初始化表格

    connect(this,&favorite::Add_current_player,[=]
    {
        //初始化表格内容
        this->Init_Table(selectrow);
    });

}

favorite::~favorite()
{
    delete ui;
}

void favorite::Init()
{
   ui->btn_Del->setIcon(QIcon(":/new/prefix1/src/clearrece.png"));
   ui->btn_Del->setLayoutDirection(Qt::LeftToRight);//设置按钮内部布局为从左至右

   ui->btn_DelAll->setIcon(QIcon(":/new/prefix1/src/deleteall.png"));
   ui->btn_DelAll->setLayoutDirection(Qt::LeftToRight);//设置按钮内部布局为从左至右

   ui->btn_AddNew->setIcon(QIcon(":/new/prefix1/src/addone.png"));
   ui->btn_AddNew->setLayoutDirection(Qt::LeftToRight);//设置按钮内部布局为从左至右

   ui->btn_Play->setIcon(QIcon(":/new/prefix1/src/kaishi.png"));
   ui->btn_Play->setLayoutDirection(Qt::LeftToRight);//设置按钮内部布局为从左至右

   ui->btn_Up->setIcon(QIcon(":/new/prefix1/src/up.png"));
   ui->btn_Up->setLayoutDirection(Qt::LeftToRight);//设置按钮内部布局为从左至右

   ui->btn_Down->setIcon(QIcon(":/new/prefix1/src/down.png"));
   ui->btn_Down->setLayoutDirection(Qt::LeftToRight);//设置按钮内部布局为从左至右
}

void favorite::Init_Table(int row)//初始化表格内容
{
    if(model!=nullptr)
    {
        delete model;
    }
    model = new QSqlTableModel(this,db2);
    model->setTable("favorite");
    ui->tableView->setModel(model); //model放到view中
    model->select();//显示数据
    ui->tableView->horizontalHeader()->setStretchLastSection(true); //自动调整最后一列的宽度占满整个表格
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选择
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);//设置model的编辑模式为手动修改
    ui->tableView->selectRow(selectrow); //设置当前选中的行
    // 设置行之间的颜色变化,使之更加美观
    ui->tableView->setAlternatingRowColors(true);
    //设置水平表头的显示
    model->setHeaderData(0,Qt::Horizontal,"收藏日期");
    model->setHeaderData(1,Qt::Horizontal,"名称");
    model->setHeaderData(2,Qt::Horizontal,"媒体");
    selectrow =row;
   ui->tableView->selectRow(selectrow); //设置当前选中的行


    // 设置以第二列排序  排序功能时使用
     //ui->tableView->sortByColumn(1,Qt::AscendingOrder);


}
//void favorite::Init_Table()//初始化表格内容
//{
//    //设置水平表头
//    ui->tableWidget->setColumnCount(3); //设置列数
//    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("收藏日期"));
//    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("名称"));
//    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("媒体"));
//    ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //自动调整最后一列的宽度占满整个表格
//    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked); //双击编辑
//    QSqlTableModel* tableModel = new QSqlTableModel(this,db2); //利用QSqlTableModel中的rowCount()获取行数
//    tableModel->setTable("favorite");
//    tableModel->select();
//    int rownumber = tableModel->rowCount();

//    ui->tableWidget->setRowCount(rownumber);//设置TableWidget的行数
//    QSqlQuery* query = new QSqlQuery(db2);
//    QString sql = "select *from favorite";
//    query->exec(sql);
//    int row = 0;
//    while(query->next())
//    {
//        QTableWidgetItem* item1 = new QTableWidgetItem;
//        QString time = query->value(0).toString();
//        item1->setText(time);
//        ui->tableWidget->setItem(row,0,item1);

//        QTableWidgetItem* item2 = new QTableWidgetItem;
//        QString name = query->value(1).toString();
//        item2->setText(name);
//        ui->tableWidget->setItem(row,1,item2);

//        QTableWidgetItem* item3 = new QTableWidgetItem;
//        QString path = query->value(2).toString();
//        item3->setText(path);
//        ui->tableWidget->setItem(row,2,item3);
//        row++;
//    }
//}

void favorite::Init_Database()//初始化数据库
{

//    if(QSqlDatabase::contains("qt_sql_default_connection"))
//    db = QSqlDatabase::database("qt_sql_default_connection");
//    else
    db2 = QSqlDatabase::addDatabase("QSQLITE","db2");
    db2.setDatabaseName("../VideoPlayer/favoritefile.db");
    if(!db2.open())
        qDebug()<<"database not open!!!";
}

void favorite::on_btn_Ok_clicked()//Save
{
    //提交所有动作
   model->submitAll();
   emit Update();
}

void favorite::on_btn_cancel_clicked()//cancel
{
    //取下所有动作
      model->revertAll();
      //提交所有动作
      model->submitAll();
    this->close();
}

void favorite::on_btn_AddNew_clicked()//新建项目
{
    //添加空记录
    QSqlRecord record = model->record();//获取空记录
    //获取行号
    int row = model->rowCount();
    //添加空行
    model->insertRecord(row,record);
}

void favorite::on_btn_Del_clicked()//删除
{
    QItemSelectionModel* smodel = ui->tableView->selectionModel();
    //取出模型中的索引
    QModelIndexList list = smodel->selectedRows();
    //删除所有选中的行
    for(int i = 0;i<list.size();i++)
    {
        model->removeRow(list.at(i).row());
    }

}

void favorite::on_btn_DelAll_clicked()//删除全部
{
    int num = model->rowCount();
    for(int i = 0;i<num;i++)
    {
        model->removeRow(i);
    }
}

void favorite::on_btn_Up_clicked()//向上
{
    selectrow--;
    if(selectrow<0)
        selectrow = 0;
    //初始化表格内容
    this->Init_Table(selectrow);
}

void favorite::on_btn_Down_clicked()//向下
{
    int temp = selectrow;
    int flag = ++selectrow;
    QSqlQuery* query = new QSqlQuery(db2);
    QString sql = QString("select *from favorite");
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
    //初始化表格内容
    this->Init_Table(flag);
}

void favorite::on_btn_Play_clicked()//播放
{
   QItemSelectionModel* smodel = ui->tableView->selectionModel();
   QModelIndex index = smodel->currentIndex(); //选中的数据
   int num = index.row(); //选中数据的行号 0
   int flag = 0;//计数器
  QSqlQuery* query = new QSqlQuery(db2);
  QString sql = "select *from favorite";
  query->exec(sql);
  query->next(); //指向第1条数据（查询结果的第0行）
  while(flag!=num)
  {
      query->next();
      flag++;
  }
  QString path = query->value(2).toString();
  QString name = query->value(1).toString();
  emit Path_Play(name,path);
}

void favorite::on_tableView_clicked(const QModelIndex &index)
{
    selectrow = index.row();
}
