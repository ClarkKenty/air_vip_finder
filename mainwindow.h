#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openfile_triggered();


    void on_pushButton_clicked();

    void on_search_by_ID_triggered();

    void on_actionquit_triggered();

    void on_file_open_buttom_clicked();

    void on_hash_settings_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
