#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);


    ui->imie->setPlaceholderText("Wpisz imię");
    ui->nazwisko->setPlaceholderText("Wpisz nazwisko");
    ui->dane->setPlaceholderText("Wpisz nr dowodu/telefonu lub maila");
    ui->pokoj->setPlaceholderText("Wpisz nr pokoju");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{

    int blad=0;
    QString imie = ui->imie->text();
    if(imie.size() != 0){
    for(int i = 0; i < imie.size();i++){
        if(!((imie[i]>='a' && imie[i]<='z') || (imie[i]>='A' && imie[i]<='Z'))){
            QMessageBox::information(this,"Błędnie wpisane dane","Wpisz ponownie swoje imię");
            blad = 1;
            ui->imie->clear();
            break;
        }
    }
    }else {
        QMessageBox::information(this,"Błędnie wpisane dane","Wpisz imię");
        blad = 1;
    }
    QString nazwisko = ui->nazwisko->text();
    if(nazwisko.size() != 0){
    for(int i = 0; i < nazwisko.size();i++){
        if(!((nazwisko[i]>='a' && nazwisko[i]<='z') || (nazwisko[i]>='A' && nazwisko[i]<='Z'))){
            QMessageBox::information(this,"Błędnie wpisane dane","Wpisz ponownie swoje nazwisko");
            blad = 1;
            ui->nazwisko->clear();
            break;
        }
    }
    }else {
        QMessageBox::information(this,"Błędnie wpisane dane","Wpisz nazwisko");
        blad = 1;
    }
    QString dane = ui->dane->text();
    if(dane.size() == 9){
        for(int i = 0; i < 3; i++){
            if(!(((dane[i]>='a' && dane[i]<='z') || (dane[i]>='A' && dane[i]<='Z')) || (dane[i]>='0' && dane[i]<='9'))){
                QMessageBox::information(this,"Błędnie wpisane dane","Wpisz ponownie nr dowodu/telefonu");
                blad = 1;
                ui->dane->clear();
                break;
            }
        }
        for(int i = 3; i < dane.size(); i++){
            if(!(dane[i]>='0' && dane[i]<='9')){
                QMessageBox::information(this,"Błędnie wpisane dane","Wpisz ponownie nr dowodu/telefonu");
                blad = 1;
                ui->dane->clear();
                break;
            }
        }

    }else if(dane.size()<9){
        QMessageBox::information(this,"Błędnie wpisane dane","Za krótki nr dowodu/telefonu");
        blad = 1;
        ui->dane->clear();
    }else {
        QMessageBox::information(this,"Błędnie wpisane dane","Za długi nr dowodu/telefonu");
        blad = 1;
        ui->dane->clear();

    }

    QString pokoj = ui->pokoj->text();

    QDate od = ui->data1->date();
    QString data1 = od.toString();

    QDate do_2 = ui->data2->date();
    QString data2 = do_2.toString();


    if(blad == 0){

    // Łączenie z bazą danych
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setUserName("root");
    database.setPassword("");
    database.setDatabaseName("qt_app");

    if(database.open()){
        // pobieramy wpisane dane

        // dodajemy do do bazy danych

        QSqlQuery qry;

        qry.prepare("INSERT INTO rezerwacje (imie, nazwisko, dowod_telefon, pokoj, od, do)"
                    "VALUES (:imie, :nazwisko, :dane, :pokoj, :od, :do)");

        qry.bindValue(":imie", imie);
        qry.bindValue(":nazwisko", nazwisko);
        qry.bindValue(":dane", dane);
        qry.bindValue(":pokoj", pokoj);
        qry.bindValue(":od", data1);
        qry.bindValue(":do", data2);

        if(qry.exec()){
            QMessageBox::information(this,"Powodzenie","Dane zostały wpisane do bazy danych");

            foreach(QLineEdit* le, findChildren<QLineEdit*>()) {
               le->clear();
            }
        } else {
            QMessageBox::information(this,"Niepowodzenie","Dane nie zostały wpisane do bazy danych");
            qDebug() << qry.lastError().text();
        }


    }else {

        QMessageBox::information(this, "Brak połączenia", "Nie uzyskano połączenia z bazą danych");
    }



}
}

void MainWindow::on_pushButton_1_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
