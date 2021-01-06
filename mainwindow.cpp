#include "mainwindow.h"//github backup
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
QVector<QString> idnum;
QVector<QString> name;
QVector<QString> airId;
QVector<QString> date;
QVector<int> miles;
QVector<int> hash_table(200,-1);
int (*collision_solve)(int a);
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->table->setColumnWidth(0,180);
    ui->table->QTableView::setSortingEnabled(false);
    ui->vip_table->QTableView::setSortingEnabled(false);
}

int HashFunction(int seed)
{
    return seed%200;
}


int collision_method1(int seed)
{
    return (seed+1)%200;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openfile_triggered()
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

void MainWindow::on_pushButton_clicked()
{
    if(ui->hash_method->currentText()=="Linear probing")
    {
        collision_solve=collision_method1;
    }
    for(int i=0;i<idnum.size();i++)
    {
        int index=HashFunction((idnum[i].midRef(7,3)).toInt());//index为哈希值,hash_table[index]为原始数据的序号.
        while(hash_table[index]!=-1)
        {
            index=collision_solve(index);
        }
        hash_table[index]=i;
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
    QVector<QString> idnum_copy=idnum;
    for(int i=0;i<idnum_copy.size();i++)
    {
        int rowcount=0;
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
}

void MainWindow::on_search_by_ID_triggered()
{
    bool ok;
    QString string = QInputDialog::getText(this,tr("查找"),tr("请输入身份证号："),QLineEdit::Normal,tr("身份证号"),&ok);
    if(ok && string.size()==18)
    {
        int index=HashFunction(string.midRef(7,3).toInt());
        int times=0;
        int res=hash_table[index];
        if(res==-1)
        {
            QMessageBox errmsg;
            errmsg.setText("查找失败！");
            return;
        }
        while(idnum[res]!=string)
        {
            index=collision_solve(index);
            res=hash_table[index];
            if(++times==300)
            {
                QMessageBox errmsg;
                errmsg.setText("查找失败！");
                errmsg.setWindowTitle("error");
                errmsg.exec();
                return;
            }
        }
        ui->table->verticalScrollBar()->setSliderPosition(index);
        ui->table->selectRow(index);
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
