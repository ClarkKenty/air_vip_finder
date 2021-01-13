#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QtDebug>
#include<QFile>
#include<QVector>
#include<QString>
#include<QTextBrowser>
#include<QTableWidget>
#include<QLineEdit>
#include<QInputDialog>
#include<QScrollBar>
#include<QHeaderView>
#include<QMessageBox>
#include<QFormLayout>
#include<QDialogButtonBox>
int tablesize = 200;//表长
QVector<QString> idnum;//身份证号
QVector<QString> name;//名字
QVector<QString> airId;//航班号
QVector<QString> date;//日期
QVector<int> miles;//里程数
QVector<int> hash_table(tablesize,-1);
int mode=1;
QVector<QVector<int>> hash_table_chain(tablesize,QVector<int>(0));
int doublecount = 1;
int (*collision_solve)(int a);
int collision_count = 0;//冲突次数
int locate_again = 0;//再散列次数
QVector<int> locatecount;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->table->setColumnWidth(0,180);
    ui->table->QTableView::setSortingEnabled(false);
    ui->vip_table->QTableView::setSortingEnabled(false);
}

int HashFunction(QString seed)//调用STL中的HASH函数
{
    std::hash<QString> hash_str;
    return hash_str(seed)%(tablesize-1);
}

int collision_method1(int seed)//线性探测法
{
    locate_again++;
    return (seed+1)%(tablesize-1);
}

int collision_method2(int seed)//再散列法
{
    locate_again++;
    std::hash<QString> hash_str;
    return hash_str(QString::number(seed))%(tablesize-1);
}

int collision_methon3(int seed)//平方探测法
{
    locate_again++;
    if(doublecount>hash_table.size()/2) return 0;
    int res= (seed+(doublecount*doublecount))%(tablesize-1);
    doublecount++;
    return res;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openfile_triggered()//选择文件对话框
{
    QString filename = QFileDialog::getOpenFileName(this,tr("文件对话框"),"D://airline_vip",tr("图片文件( * txt"));
    if(filename.length() == 0) {
                    QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    }
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream input(&file);
    input.setCodec("utf-8");
    while(!input.atEnd())
    {
        QString line = input.readLine();
        QStringList lists = line.split(' ');
        idnum.push_back(lists[0].split("：")[1]);
        name.push_back(lists[1].split("：")[1]);
        airId.push_back(lists[2].split("：")[1]);
        date.push_back(lists[3].split("：")[1]);
        miles.push_back((lists[4].split("：")[1]).toUInt());
    }
    input.seek(0);
    ui->outputText->setText(input.readAll());
    file.close();
}

void MainWindow::on_pushButton_clicked()//用户点击HASH按钮
{
    ui->vip_table->setRowCount(0);
    collision_count = 0;//冲突次数
    locate_again = 0;//再散列次数
    locatecount.clear();
    for(int i=0;i<hash_table_chain.size();i++)
    {
        hash_table_chain[i].clear();
    }
    ui->table->setRowCount(0);
    if(ui->hash_method->currentText()=="链地址法")
        mode=0;
    else mode = 1;
    std::fill(hash_table.begin(),hash_table.end(),-1);
    if(mode)//用户选择闭散列方法
    {
        if(ui->hash_method->currentText()=="线性探测法")
        {
            collision_solve=collision_method1;
        }
        else if(ui->hash_method->currentText()=="再散列法")
        {
            collision_solve=collision_method2;
        }
        else if(ui->hash_method->currentText()=="平方探测法")
        {
            collision_solve=collision_methon3;
        }
        for(int i=0;i<idnum.size();i++)//调用散列函数并解决冲突
        {
            doublecount=1;
            locate_again = 0;
            int index=HashFunction(idnum[i]);//index为哈希值,hash_table[index]为原始数据的序号
            if(hash_table[index]!=-1)   collision_count++;
            while(hash_table[index]!=-1)
            {
                index=collision_solve(index);//调用解决冲突函数
            }
            hash_table[index]=i;
            locatecount.push_back(locate_again);
        }
        for(int i =0;i<hash_table.size();i++)
        {
            if(hash_table[i]!=-1)
            {
                ui->table->insertRow(i);
                int j = hash_table[i];
                QTableWidgetItem* newitem = new QTableWidgetItem(QString::number(i));
                ui->table->setVerticalHeaderItem(i,newitem);
                QTableWidgetItem* newitem1 = new QTableWidgetItem(idnum[j]);
                ui->table->setItem(i,0,newitem1);
                QTableWidgetItem* newitem2 = new QTableWidgetItem(name[j]);
                ui->table->setItem(i,1,newitem2);
                QTableWidgetItem* newitem3 = new QTableWidgetItem(airId[j]);
                ui->table->setItem(i,2,newitem3);
                QTableWidgetItem* newitem4 = new QTableWidgetItem(date[j]);
                ui->table->setItem(i,3,newitem4);
                QTableWidgetItem* newitem5 = new QTableWidgetItem(QString::number(miles[j]));
                ui->table->setItem(i,4,newitem5);
            }
            else
            {
                ui->table->insertRow(i);
                QTableWidgetItem* newitem = new QTableWidgetItem(QString::number(i));
                ui->table->setVerticalHeaderItem(i,newitem);
            }
        }


    }
    else{//用户选择闭散列方法
        for(int i=0;i<idnum.size();i++)
        {
            int index=HashFunction(idnum[i]);//index为哈希值,hash_table[index]为原始数据的序号
            if(hash_table[index]!=-1)
            {
                hash_table_chain[index].push_back(i);
            }
            else hash_table[index]=i;
        }
        int rows=0;
        for(int i =0;i<hash_table.size();i++)
        {
            if(hash_table[i]!=-1)
            {
                ui->table->insertRow(rows);
                int j = hash_table[i];
                QTableWidgetItem* newitem = new QTableWidgetItem(QString::number(i));
                ui->table->setVerticalHeaderItem(rows,newitem);
                QTableWidgetItem* newitem1 = new QTableWidgetItem(idnum[j]);
                ui->table->setItem(rows,0,newitem1);
                QTableWidgetItem* newitem2 = new QTableWidgetItem(name[j]);
                ui->table->setItem(rows,1,newitem2);
                QTableWidgetItem* newitem3 = new QTableWidgetItem(airId[j]);
                ui->table->setItem(rows,2,newitem3);
                QTableWidgetItem* newitem4 = new QTableWidgetItem(date[j]);
                ui->table->setItem(rows,3,newitem4);
                QTableWidgetItem* newitem5 = new QTableWidgetItem(QString::number(miles[j]));
                ui->table->setItem(rows,4,newitem5);
                rows++;
                int counts = 1;
                for(auto k=hash_table_chain[i].begin();k!=hash_table_chain[i].end();k++)
                {
                    collision_count++;
                    ui->table->insertRow(rows);
                    int j2 = *k;
                    QTableWidgetItem* newitem5 = new QTableWidgetItem(QString::number(i)+"--"+QString::number(counts++));
                    newitem5->setForeground(QColor(0,0,255));
                    ui->table->setVerticalHeaderItem(rows,newitem5);
                    QTableWidgetItem* newitem6 = new QTableWidgetItem(idnum[j2]);
                    ui->table->setItem(rows,0,newitem6);
                    QTableWidgetItem* newitem7 = new QTableWidgetItem(name[j2]);
                    ui->table->setItem(rows,1,newitem7);
                    QTableWidgetItem* newitem8 = new QTableWidgetItem(airId[j2]);
                    ui->table->setItem(rows,2,newitem8);
                    QTableWidgetItem* newitem9 = new QTableWidgetItem(date[j2]);
                    ui->table->setItem(rows,3,newitem9);
                    QTableWidgetItem* newitem0 = new QTableWidgetItem(QString::number(miles[j2]));
                    ui->table->setItem(rows,4,newitem0);
                    rows++;
                }
            }
            else
            {
                ui->table->insertRow(rows);
                QTableWidgetItem* newitem = new QTableWidgetItem(QString::number(i));
                ui->table->setVerticalHeaderItem(rows,newitem);
                rows++;
            }
        }
    }
    QVector<QString> idnum_copy=idnum;//vip
    int rowcount=0;
    for(int i=0;i<idnum_copy.size();i++)
    {
        if(idnum_copy[i]==" ") continue;
        int count=1;
        int miles_sum=miles[i];
        for(int j = i+1;j<idnum_copy.size();j++)
        {
            if(idnum_copy[j]==idnum_copy[i] && idnum_copy[j]!=" ")
            {
                count++;
                miles_sum+=miles[j];
                idnum_copy[j]=" ";
            }
        }
        if(count>1)
        {
            ui->vip_table->insertRow(rowcount);
            QTableWidgetItem* newitem1 = new QTableWidgetItem(idnum[i]);
            ui->vip_table->setItem(rowcount,0,newitem1);
            QTableWidgetItem* newitem2 = new QTableWidgetItem(name[i]);
            ui->vip_table->setItem(rowcount,1,newitem2);
            QTableWidgetItem* newitem3 = new QTableWidgetItem(QString::number(count));
            ui->vip_table->setItem(rowcount,2,newitem3);
            QTableWidgetItem* newitem4 = new QTableWidgetItem(QString::number(miles_sum));
            ui->vip_table->setItem(rowcount,3,newitem4);
            rowcount++;
        }
    }
    ui->vip_table->sortByColumn(2,Qt::DescendingOrder);
    QMessageBox sucmsg;
    QString mes;
    if(mode==1)
    {
        int cc = 0;
        for(int i =0;i<locatecount.size();i++)
        {
            if(locatecount[i] == 0) continue;
            cc++;
            mes+=QString::number(locatecount[i]) + " ";
            if(cc%25==0&&i!=0)
                mes+="\n";
        }
        sucmsg.setText("散列成功！总冲突次数为："+QString::number(collision_count) +", 重定位次数分别为：\n" + mes);
    }
    else
        sucmsg.setText("散列成功！总冲突次数为："+QString::number(collision_count));
    sucmsg.setWindowTitle("success");
    sucmsg.exec();
}

void MainWindow::on_search_by_ID_triggered()//用户使用搜索功能
{
    bool ok;
    QString string = QInputDialog::getText(this,tr("查找"),tr("请输入身份证号："),QLineEdit::Normal,tr("身份证号"),&ok);
    if(mode==1)//闭散列
    {
        if(ok && string.size()==18)
        {
            int index=HashFunction(string);
            int times=0;
            int res=hash_table[index];
            if(res==-1)
            {
                QMessageBox errmsg;
                errmsg.setText("查找失败！");
                errmsg.setWindowTitle("error");
                errmsg.exec();
                return;
            }
            doublecount=1;
            while(idnum[res]!=string)
            {
                index=collision_solve(index);
                res=hash_table[index];
                if(++times==300 || res==-1)
                {
                    QMessageBox errmsg;
                    errmsg.setText("查找失败！");
                    errmsg.setWindowTitle("error");
                    errmsg.exec();
                    return;
                }
            }
            QMessageBox sucmsg;
            sucmsg.setText("查找成功！散列值为："+QString::number(index));
            sucmsg.setWindowTitle("success");
            sucmsg.exec();
            ui->table->verticalScrollBar()->setSliderPosition(index);
            ui->table->selectRow(index);
        }
    }
    else{//开散列
        if(ok && string.size()==18)
        {
            int index=HashFunction(string);
            bool suc=false;
            int pos=0;
            int res=hash_table[index];
            if(res==-1)
            {
                QMessageBox errmsg;
                errmsg.setText("查找失败！");
                errmsg.setWindowTitle("error");
                errmsg.exec();
                return;
            }
            if(idnum[res]!=string)
            {
                for(auto m=hash_table_chain[index].begin();m!=hash_table_chain[index].end();m++)
                {
                    if(string==idnum[*m])
                    {
                        suc=true;
                        pos=m-hash_table_chain[index].begin()+1;
                        break;
                    }
                }
            }
            else{
                suc=true;
            }
            if(suc)
            {
                int fore=0;
                for(int n = 0;n<index;n++)
                {
                    fore+=hash_table_chain[n].size();
                }
                ui->table->verticalScrollBar()->setSliderPosition(index+pos+fore);
                ui->table->selectRow(index+pos+fore);
                QMessageBox sucmsg;
                sucmsg.setText("查找成功！散列值为："+QString::number(index));
                sucmsg.setWindowTitle("success");
                sucmsg.exec();
            }
        }
    }
}

void MainWindow::on_actionquit_triggered()
{
    close();
}

void MainWindow::on_file_open_buttom_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("文件对话框"),"D://airline_vip",tr("图片文件( * txt"));
    if(filename.length() == 0) {
                    QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
                    return;
    }
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream input(&file);
    input.setCodec("utf-8");
    while(!input.atEnd())
    {
        QString line = input.readLine();
        QStringList lists = line.split(' ');
        idnum.push_back(lists[0].split("：")[1]);
        name.push_back(lists[1].split("：")[1]);
        airId.push_back(lists[2].split("：")[1]);
        date.push_back(lists[3].split("：")[1]);
        miles.push_back((lists[4].split("：")[1]).toUInt());
    }
    input.seek(0);
    ui->outputText->setText(input.readAll());
    file.close();
}

void MainWindow::on_hash_settings_clicked()//用户自定义散列表长度
{
    QDialog diag;
    diag.setWindowTitle("自定义");
    diag.resize(200,20);
    QFormLayout form(&diag);
    form.setVerticalSpacing(15);
    form.addRow(new QLabel("设置："));
    QLineEdit* newsize = new QLineEdit(&diag);
    form.addRow("输入表长",newsize);
    QDialogButtonBox button(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &diag);
    connect(&button,SIGNAL(accepted()),&diag,SLOT(accept()));
    connect(&button,SIGNAL(rejected()),&diag,SLOT(reject()));
    form.addRow(&button);
    if(diag.exec()==QDialog::Accepted)
    {
        if(newsize->text()=="")
        {
            QMessageBox* warning = new QMessageBox();
            warning->setWindowTitle("Warning");
            warning->setText("输入为空！");
            warning->show();
            return;
        }
        if(newsize->text().toInt()<idnum.size()/0.6)
        {
            QMessageBox* warning = new QMessageBox();
            warning->setWindowTitle("Warning");
            warning->setText("表长过短！");
            warning->show();
            return;
        }
        if(newsize->text().toInt()>10000)
        {
            QMessageBox* warning = new QMessageBox();
            warning->setWindowTitle("Warning");
            warning->setText("表长太大！");
            warning->show();
            return;
        }
        tablesize = newsize->text().toUInt();
        hash_table.resize(tablesize);
        hash_table_chain.resize(tablesize);
        ui->label_4->setText("表长：" + newsize->text());
        diag.close();
    }
}
