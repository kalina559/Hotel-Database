#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>
#include<cctype>
#include<string>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_41_clicked();

    void on_pushButton_42_clicked();

    void on_pushButton_40_clicked();

    void on_pushButton_44_clicked();

    void on_pushButton_43_clicked();

    void on_pushButton_45_clicked();

    void on_pushButton_46_clicked();

    void on_pushButton_76_clicked();

    void on_pushButton_39_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_79_clicked();

    void on_pushButton_80_clicked();

    void on_pushButton_81_clicked();

    void on_pushButton_82_clicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase database;

};

#endif // MAINWINDOW_H
