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
    ui->telefon->setPlaceholderText("Wpisz nr telefonu");
    ui->email->setPlaceholderText("Wpisz adres e-mail");
    ui->login->setPlaceholderText("Wpisz login");
    ui->haslo->setPlaceholderText("Wpisz haslo");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()          // przycisk 'zarezerwuj' na page_3
{

    int wrongData = 0;
    QString imie = ui->imie->text();
    if (imie.size() != 0)
    {
        for (int i = 0; i < imie.size();i++)
        {
            if (!((imie[i] >= 'a' && imie[i] <= 'z') || (imie[i] >= 'A' && imie[i] <= 'Z')))
            {
                QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz ponownie swoje imię");
                wrongData = 1;
                ui->imie->clear();
                break;
            }
        }
    }
    else
    {
        QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz imię");
        wrongData = 1;
    }
    QString nazwisko = ui->nazwisko->text();
    if (nazwisko.size() != 0)
    {
        for (int i = 0; i < nazwisko.size();i++)
        {
            if (!((nazwisko[i] >= 'a' && nazwisko[i] <= 'z') || (nazwisko[i] >= 'A' && nazwisko[i] <= 'Z')))
            {
                QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz ponownie swoje nazwisko");
                wrongData = 1;
                ui->nazwisko->clear();
                break;
            }
        }
    }
    else
    {
        QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz nazwisko");
        wrongData = 1;
    }
    QString telefon = ui->telefon->text();
    if (telefon.size() == 9)
    {
        for (int i = 0; i < telefon.size(); i++)
        {
            if (!(telefon[i] >= '0' && telefon[i] <= '9'))
            {
                QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz ponownie nr telefonu");
                wrongData = 1;
                ui->telefon->clear();
                break;
            }
        }
    }
    else if (telefon.size() < 9)
    {
        QMessageBox::information(this, "Błędnie wpisane dane", "Za krótki nr telefonu");
        wrongData = 1;
        ui->telefon->clear();
    }
    else
    {
        QMessageBox::information(this, "Błędnie wpisane dane", "Za długi nr telefonu");
        wrongData = 1;
        ui->telefon->clear();
    }

    QString email = ui->email->text();

    QDate from = ui->data1->date();
    //QString data1 = from.toString();

    QDate to = ui->data2->date();
    //QString data2 = to.toString();

    int roomNumber = ui->spinBox->value();
    ui->spinBox_6->clear();
    if (wrongData == 0)
    {
        // Łączenie z bazą danych
        database = QSqlDatabase::addDatabase("QMYSQL");
        database.setHostName("localhost");
        database.setUserName("root");
        database.setPassword("");
        database.setDatabaseName("hotel");

        if (database.open())
        {
            QSqlQuery  qryCheck;
            qryCheck.prepare("SELECT od, do FROM rezerwacja_pokoj WHERE id_pokoju = :roomNumber"
            );
            qryCheck.bindValue(":roomNumber", roomNumber);

            bool isAvailable = true;  // zmienna pomocnicza, sprawdza czy dany pokój jest dostępny w podanym terminie

            if (qryCheck.exec())
            {
                //            QMessageBox::information(this,"Powodzenie","Połączono");
                while (qryCheck.next())
                {
                    QDate fromDB = qryCheck.value(0).toDate();  //juz istniejace w DB
                    QDate toDB = qryCheck.value(1).toDate();
                    qDebug() << fromDB << " - " << toDB <<endl;
                    if((from > fromDB && from < toDB) || (to > fromDB && to < toDB ) || (from < fromDB && to > toDB) ||(from > to)) // sprawdzamy czy pokoj jest dostepny w wybranym terminie
                    {
                        isAvailable = false;
                        break;
                    }
                }

            }
            else {
                QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
                qDebug() << qryCheck.lastError().text();
            }

            QSqlQuery  qrySecondCheck;
            qrySecondCheck.prepare("SELECT od, do FROM klient_pokoj WHERE id_pokoju = :roomNumber"
            );
            qrySecondCheck.bindValue(":roomNumber", roomNumber);

            if (qrySecondCheck.exec())
            {
                //            QMessageBox::information(this,"Powodzenie","Połączono");
                while (qrySecondCheck.next())
                {
                    QDate fromDB = qrySecondCheck.value(0).toDate();  //juz istniejace w DB
                    QDate toDB = qrySecondCheck.value(1).toDate();
                    qDebug() << fromDB << " - " << toDB <<endl;
                    if((from > fromDB && from < toDB) || (to > fromDB && to < toDB ) || (from < fromDB && to > toDB) ||(from > to)) // sprawdzamy czy pokoj jest dostepny w wybranym terminie
                    {
                        isAvailable = false;
                        break;
                    }
                }
            }
            else {
                QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
                qDebug() << qrySecondCheck.lastError().text();
            }

            qDebug()<<"availability: "<< isAvailable<<endl;          //POMOCNICZE================================

            if(isAvailable == true )
            {

                QSqlQuery qry;

                qry.prepare("INSERT INTO klient (imie, nazwisko, telefon, email)"
                    "VALUES (:imie, :nazwisko, :telefon, :email);"

                    "INSERT INTO rezerwacja(id_klienta) SELECT MAX(id_klienta) FROM klient WHERE 1;"
                    "UPDATE rezerwacja SET status = 'oczekiwanie', cena = ((SELECT cena FROM pokoj WHERE id_pokoju = :roomNumber) * ABS(DATEDIFF(:od, :do)))"

                    "WHERE id_klienta = (SELECT max(id_klienta) FROM (SELECT * FROM rezerwacja) as kopia WHERE 1);"

                    "INSERT INTO rezerwacja_pokoj (id_rezerwacji, id_pokoju, od, do) SELECT MAX(id_rezerwacji), :roomNumber, :od, :do FROM rezerwacja WHERE 1"
                );

                qry.bindValue(":imie", imie);
                qry.bindValue(":nazwisko", nazwisko);
                qry.bindValue(":telefon", telefon);
                qry.bindValue(":email", email);
                qry.bindValue(":od", from);
                qry.bindValue(":do", to);
                qry.bindValue(":roomNumber", roomNumber);

                if (qry.exec())
                {
                    QMessageBox::information(this, "Powodzenie", "Dane zostały wpisane do bazy danych");
                    ui->stackedWidget->setCurrentIndex(9); // przechodzimy do strony z potwierdzeniem rezerwacji

                    QSqlQuery * qry = new QSqlQuery;
                    QSqlQueryModel * modal = new QSqlQueryModel;

                    qry->prepare("SELECT rp.id_rezerwacji, rp.id_pokoju, rp.od, rp.do, rezerwacja.cena  FROM rezerwacja_pokoj AS rp "
                                 "LEFT JOIN rezerwacja USING(id_rezerwacji) "
                                 "WHERE id_rezerwacji = (SELECT MAX(id_rezerwacji) FROM rezerwacja_pokoj) "

                    );
                    qry->exec();
                    modal->setQuery(*qry);
                    ui->tableView_2->setModel(modal);



                    foreach(QLineEdit* le, findChildren<QLineEdit*>())
                    {
                        le->clear();
                    }
                }
                else
                {
                    QMessageBox::information(this, "Niepowodzenie", "Dane nie zostały wpisane do bazy danych");
                    qDebug() << qry.lastError().text();
                }

            }
            else
            {
                QMessageBox::information(this, "Niepowodzenie", "Wybrany pokój jęst zajęty we wskazanym terminie");
            }
        }
        else
        {
            QMessageBox::information(this, "Brak połączenia", "Nie uzyskano połączenia z bazą danych");
        }
    }
}


void MainWindow::on_pushButton_4_clicked()   // powrót z page_3 na page
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_7_clicked()  // przycisk 'klient' na page_1
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_6_clicked()  // przycisk 'powrót' na page_2
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_11_clicked()  // przycisk 'wynajmij pokój' na page
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_pushButton_2_clicked()  // przycisk 'powrót' na page
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_12_clicked()  // przycisk 'pokaż wolne pokoje' na page
{
    QDate from = ui->data1_3->date();
    QDate to = ui->data2_3->date();
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setUserName("root");
    database.setPassword("");
    database.setDatabaseName("hotel");

    if (database.open())
    {

        QSqlQuery * qry = new QSqlQuery;
        QSqlQueryModel * modal = new QSqlQueryModel;

        qry->prepare("SELECT id_pokoju as numer_pokoju, standard, liczba_lozek, (pokoj.cena * ABS(DATEDIFF(:od, :do))) AS cena_za_pobyt FROM pokoj "
            "LEFT JOIN rezerwacja_pokoj AS rp USING(id_pokoju) "
            "LEFT JOIN klient_pokoj AS kp USING(id_pokoju) "
            "WHERE ((rp.od <= :od AND rp.do <= :od) OR (rp.od >= :do AND rp.do >= :do) "
            "OR (rp.od IS NULL AND rp.do IS NULL)) "
            "AND ((kp.od <= :od AND kp.do <= :od) OR (kp.od >= :do AND kp.do >= :do) "
            "OR (kp.od IS NULL AND kp.do IS NULL)) "
            "GROUP BY id_pokoju"

        );
        qry->bindValue(":od", from);
        qry->bindValue(":do", to);
        qry->exec();
        modal->setQuery(*qry);
        ui->tableView->setModel(modal);


        if (qry->exec()) {
            QMessageBox::information(this, "Powodzenie", "Połączono");

            foreach(QLineEdit* le, findChildren<QLineEdit*>()) {
                le->clear();
            }
        }
        else {
            QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
            qDebug() << qry->lastError().text();
        }
    }
}


void MainWindow::on_pushButton_5_clicked()  // przycisk 'sprawdź dostępność pokojów' na page_2
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_pushButton_8_clicked()  // przycisk 'pracownicy' na page_1
{
    ui->stackedWidget->setCurrentIndex(4);
}



void MainWindow::on_pushButton_9_clicked()  //przycisk 'powrót' na page_4
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_clicked()  //przycisk 'zaloguj' na page_4
{
    QString login = ui->login->text();
    QString haslo = ui->haslo->text();
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setUserName("root");
    database.setPassword("");
    database.setDatabaseName("hotel");

    if (database.open()) {

        QSqlQuery  qry;
        qry.prepare("SELECT login, haslo,typ FROM pracownicy WHERE login = :login AND haslo = :haslo"
        );
        qry.bindValue(":login", login);
        qry.bindValue(":haslo", haslo);

        if (qry.exec())
        {
            //            QMessageBox::information(this,"Powodzenie","Połączono");

            foreach(QLineEdit* le, findChildren<QLineEdit*>())
            {
                le->clear();

                while (qry.next())
                {
                    QString loginDB = qry.value(0).toString();
                    QString hasloDB = qry.value(1).toString();
                    qDebug() << loginDB << " " << hasloDB;

                    if (loginDB == login && hasloDB == haslo)
                    {
                        QMessageBox::information(this, "Powodzenie", "Zalogowano");
                        QString typDB = qry.value(2).toString();
                        if (typDB == "recepcja")
                            ui->stackedWidget->setCurrentIndex(5);
                        if (typDB == "menadzer")
                            ui->stackedWidget->setCurrentIndex(6);
                        if (typDB == "sprzatanie")
                            ui->stackedWidget->setCurrentIndex(8);
                    }
                    else
                    {
                        QMessageBox::information(this, "Niepowodzenie", "Nie uzyskano połączenia");
                    }
                }
            }
        }
        else {
            QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
            qDebug() << qry.lastError().text();
        }
    }
}

void MainWindow::on_pushButton_10_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_pushButton_13_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_pushButton_14_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_pushButton_16_clicked()      // przycisk 'potwierdzam' na page_7
{
    QMessageBox::information(this, "Potwierdzanie rezerwacji", "Dziękujemy za złożenie rezerwacji w naszym hotelu");
    ui->stackedWidget->setCurrentIndex(1);

}

void MainWindow::on_pushButton_17_clicked()
{
    QMessageBox::information(this, "Potwierdzanie meldunku", "Dane klienta zostały wpisane do bazy danych");
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_18_clicked()
{
    ui->stackedWidget->setCurrentIndex(11);
}

void MainWindow::on_pushButton_41_clicked()
{
    ui->stackedWidget->setCurrentIndex(12);
}

void MainWindow::on_pushButton_42_clicked()   //wynajem na recepcji
{
    int wrongData = 0;
    QString imie = ui->imie_6->text();
    if (imie.size() != 0)
    {
        for (int i = 0; i < imie.size();i++)
        {
            if (!((imie[i] >= 'a' && imie[i] <= 'z') || (imie[i] >= 'A' && imie[i] <= 'Z')))
            {
                QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz ponownie swoje imię");
                wrongData = 1;
                ui->imie_6->clear();
                break;
            }
        }
    }
    else
    {
        QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz imię");
        wrongData = 1;
    }
    QString nazwisko = ui->nazwisko_6->text();
    if (nazwisko.size() != 0)
    {
        for (int i = 0; i < nazwisko.size();i++)
        {
            if (!((nazwisko[i] >= 'a' && nazwisko[i] <= 'z') || (nazwisko[i] >= 'A' && nazwisko[i] <= 'Z')))
            {
                QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz ponownie swoje nazwisko");
                wrongData = 1;
                ui->nazwisko_6->clear();
                break;
            }
        }
    }
    else
    {
        QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz nazwisko");
        wrongData = 1;
    }
    QString telefon = ui->telefon_6->text();
    if (telefon.size() == 9)
    {
        for (int i = 0; i < telefon.size(); i++)
        {
            if (!(telefon[i] >= '0' && telefon[i] <= '9'))
            {
                QMessageBox::information(this, "Błędnie wpisane dane", "Wpisz ponownie nr telefonu");
                wrongData = 1;
                ui->telefon_6->clear();
                break;
            }
        }
    }
    else if (telefon.size() < 9)
    {
        QMessageBox::information(this, "Błędnie wpisane dane", "Za krótki nr telefonu");
        wrongData = 1;
        ui->telefon_6->clear();
    }
    else
    {
        QMessageBox::information(this, "Błędnie wpisane dane", "Za długi nr telefonu");
        wrongData = 1;
        ui->telefon_6->clear();
    }

    QString email = ui->email_6->text();

    QDate to = ui->data2_8->date();
    QDate currentDate = QDate::currentDate();
    int roomNumber = ui->spinBox_6->value();
    ui->spinBox_6->clear();


    if (wrongData == 0)
    {
        database = QSqlDatabase::addDatabase("QMYSQL");
        database.setHostName("localhost");
        database.setUserName("root");
        database.setPassword("");
        database.setDatabaseName("hotel");

        if (database.open())
        {
            QSqlQuery  qryCheck;
            qryCheck.prepare("SELECT od, do, status FROM rezerwacja_pokoj LEFT JOIN pokoj USING(id_pokoju) WHERE id_pokoju = :roomNumber"
            );
            qryCheck.bindValue(":roomNumber", roomNumber);

            bool isAvailable = true;  // zmienna pomocnicza, sprawdza czy dany pokój jest dostępny w podanym terminie

            if (qryCheck.exec())
            {
                foreach(QLineEdit* le, findChildren<QLineEdit*>())
                {
                    le->clear();
                }
                if (qryCheck.next())
                {
                    QDate fromDB = qryCheck.value(0).toDate();  //juz istniejace w DB
                    QDate toDB = qryCheck.value(1).toDate();
                    if((currentDate >= fromDB && currentDate < toDB) || (to >= fromDB && to < toDB ) || (currentDate <= fromDB && currentDate >= toDB) || (currentDate >= to)) // sprawdzamy czy pokoj jest dostepny w wybranym terminie
                    {
                        qDebug()<<"checking "<< isAvailable <<endl;
                        isAvailable = false;
                        //break;
                    }
                    else
                    {
                        QMessageBox::information(this, "Błąd", "W tym terminie wybrany pokój jest zajęty");
                        qDebug() << qryCheck.lastError().text();
                    }
                }

                QSqlQuery  qryStatusCheck;
                qryStatusCheck.prepare("SELECT status FROM pokoj WHERE id_pokoju = :roomNumber");
                qryStatusCheck.bindValue(":roomNumber", roomNumber);

                QString status;
                qryStatusCheck.exec();

                if(qryStatusCheck.next())
                {
                    status = qryStatusCheck.value(0).toString();
                }


                if(isAvailable == true && status == "wolny")
                {
                    QSqlQuery qry;

                    qry.prepare("INSERT INTO klient (imie, nazwisko, telefon, email)"
                        "VALUES (:imie, :nazwisko, :telefon, :email);"
                        "INSERT INTO klient_pokoj (id_klienta, id_pokoju, od, do) SELECT MAX(id_klienta), :roomNumber, :od, :do  FROM klient WHERE 1;"
                        "UPDATE klient_pokoj SET cena = (SELECT cena FROM pokoj WHERE id_pokoju = :roomNumber) * ABS(DATEDIFF(:od, :do)) "
                        "WHERE id_klienta = (SELECT MAX(id_klienta) FROM klient_pokoj);"
                        "UPDATE klient_pokoj LEFT JOIN pokoj USING(id_pokoju) SET status = 'zajety' WHERE id_klienta = (SELECT MAX(id_klienta) FROM klient_pokoj);"
                    );

                    qry.bindValue(":imie", imie);
                    qry.bindValue(":nazwisko", nazwisko);
                    qry.bindValue(":telefon", telefon);
                    qry.bindValue(":email", email);
                    qry.bindValue(":od", currentDate);
                    qry.bindValue(":do", to);
                    qry.bindValue(":roomNumber", roomNumber);


                    if (qry.exec())
                    {
                        QMessageBox::information(this, "Powodzenie", "Dane zostały wpisane do bazy danych");

                        ui->stackedWidget->setCurrentIndex(10); // przechodzimy do strony z potwierdzeniem meldunku

                        QSqlQuery * qry1 = new QSqlQuery;
                        QSqlQueryModel * modal = new QSqlQueryModel;

                        qry1->prepare("SELECT * FROM klient_pokoj LEFT JOIN klient USING(id_klienta)"
                                     "WHERE id_klienta = (SELECT MAX(id_klienta) FROM klient_pokoj) "

                        );
                        qry1->exec();
                        modal->setQuery(*qry1);
                        ui->tableView_3->setModel(modal);

                        foreach(QLineEdit* le, findChildren<QLineEdit*>())
                        {
                            le->clear();
                        }
                    }
                    else
                    {
                        QMessageBox::information(this, "Niepowodzenie", "Dane nie zostały wpisane do bazy danych");
                        qDebug() << qry.lastError().text();
                    }
                }
                else
                {
                    QMessageBox::information(this, "Niepowodzenie", "Wybrany pokój jest obecnie zajęty");
                }
            }
            else
            {
                QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
                qDebug() << qryCheck.lastError().text();
            }
        }
        else
        {
            QMessageBox::information(this, "Brak połączenia", "Nie uzyskano połączenia z bazą danych");
        }
    }
}

void MainWindow::on_pushButton_40_clicked() //zamelduje z rezerwacja na page_10
{
    ui->stackedWidget->setCurrentIndex(13);
}

void MainWindow::on_pushButton_44_clicked()      // powrot na page_23
{
    ui->stackedWidget->setCurrentIndex(11);
}

void MainWindow::on_pushButton_43_clicked()       //przycisk 'wyszukaj' na page_23
{
    QString id_rezerwacji = ui->lineEdit->text();
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setUserName("root");
    database.setPassword("");
    database.setDatabaseName("hotel");

    if (database.open())
    {

        QSqlQuery * qry = new QSqlQuery;
        QSqlQueryModel * modal = new QSqlQueryModel;

        qry->prepare("SELECT * FROM rezerwacja_pokoj LEFT JOIN rezerwacja USING (id_rezerwacji) WHERE id_rezerwacji = :id_rezerwacji"

        );
        qry->bindValue(":id_rezerwacji", id_rezerwacji);


        qry->exec();

        if(qry->next())
        {
            if (qry->exec())
            {

                modal->setQuery(*qry);
                ui->tableView_7->setModel(modal);
            }
            else
            {
                QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
                qDebug() << qry->lastError().text();
            }


        }
        else
        {
            QMessageBox::information(this, "Błąd", "Rezerwacja o podanym ID nie istnieje");
        }

    }
}

void MainWindow::on_pushButton_45_clicked()  // przycisk zamelduj na page_23 (z rezerwacją)
{
    if(ui->tableView_7->rowAt(0) != -1)
    {
        QString id_rezerwacji = ui->lineEdit->text();
        database = QSqlDatabase::addDatabase("QMYSQL");
        database.setHostName("localhost");
        database.setUserName("root");
        database.setPassword("");
        database.setDatabaseName("hotel");

        if (database.open())
        {

            QSqlQuery  qry;
            qry.prepare("INSERT INTO klient_pokoj(id_klienta, id_pokoju, od, do, cena) SELECT id_klienta, id_pokoju, od, do, cena "
                        "FROM rezerwacja_pokoj LEFT JOIN rezerwacja USING(id_rezerwacji) WHERE id_rezerwacji = :id_rezerwacji;"
                        "UPDATE pokoj SET status = 'zajety' WHERE id_pokoju = (SELECT id_pokoju FROM rezerwacja_pokoj WHERE id_rezerwacji = :id_rezerwacji);"
                        "DELETE FROM rezerwacja_pokoj WHERE id_rezerwacji = :id_rezerwacji;"
                        "UPDATE rezerwacja SET status = 'zrealizowana' WHERE id_rezerwacji = :id_rezerwacji;"

            );
            qry.bindValue(":id_rezerwacji", id_rezerwacji);

            if (qry.exec())
            {
                QMessageBox::information(this, "Powodzenie", "Zrealizowano rezerwację");
                qDebug() << qry.lastError().text();

            }
            else
            {
                QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
                qDebug() << qry.lastError().text();
            }
        }
    }
    else
    {
        QMessageBox::information(this, "Niepowodzenie", "Wpisz numer rezerwacji");
    }
}

void MainWindow::on_pushButton_46_clicked()   // przycisk 'wyszukaj wolne pokoje' na page_22
{
    QDate from = QDate::currentDate();
    QDate to = ui->data2_9->date();
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setUserName("root");
    database.setPassword("");
    database.setDatabaseName("hotel");

    if (database.open())
    {

        QSqlQuery * qry = new QSqlQuery;
        QSqlQueryModel * modal = new QSqlQueryModel;

        qry->prepare("SELECT id_pokoju as numer_pokoju, standard, liczba_lozek, (pokoj.cena * ABS(DATEDIFF(:od, :do))) AS cena_za_pobyt FROM pokoj "
            "LEFT JOIN rezerwacja_pokoj AS rp USING(id_pokoju) "
            "WHERE ((rp.od <= :od AND rp.do <= :od) OR (rp.od >= :do AND rp.do >= :do) "
            "OR (rp.od IS NULL AND rp.do IS NULL)) AND status = 'wolny'"
            ""
            "GROUP BY id_pokoju"

        );
        qry->bindValue(":od", from);
        qry->bindValue(":do", to);
        qry->exec();
        modal->setQuery(*qry);
        ui->tableView_8->setModel(modal);


        if (qry->exec()) {
            QMessageBox::information(this, "Powodzenie", "Połączono");

            foreach(QLineEdit* le, findChildren<QLineEdit*>()) {
                le->clear();
            }
        }
        else {
            QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
            qDebug() << qry->lastError().text();
        }
    }
}

void MainWindow::on_pushButton_76_clicked()
{
     ui->stackedWidget->setCurrentIndex(5);
}

void MainWindow::on_pushButton_39_clicked()
{
    ui->stackedWidget->setCurrentIndex(14);
}

void MainWindow::on_pushButton_19_clicked()
{
    ui->stackedWidget->setCurrentIndex(15);
}

void MainWindow::on_pushButton_79_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void MainWindow::on_pushButton_80_clicked()     // przycisk 'wyszukaj' na page_39 (wymeldowanie)
{
    QString id_klienta = ui->lineEdit_4->text();
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setUserName("root");
    database.setPassword("");
    database.setDatabaseName("hotel");

    if (database.open())
    {

        QSqlQuery * qry = new QSqlQuery;
        QSqlQueryModel * modal = new QSqlQueryModel;

        qry->prepare("SELECT * FROM klient_pokoj LEFT JOIN klient USING(id_klienta) WHERE id_klienta = :id_klienta"

        );
        qry->bindValue(":id_klienta", id_klienta);

        qry->exec();
        if(qry->next())
        {
            if (qry->exec())
            {
                modal->setQuery(*qry);
                ui->tableView_15->setModel(modal);
            }
            else
            {
                QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
                qDebug() << qry->lastError().text();
            }
        }
        else
        {
            QMessageBox::information(this, "Błąd", "Klient o podanym ID nie istnieje");
        }

    }
}

void MainWindow::on_pushButton_81_clicked()   //przycisk 'wymelduj' na page_39
{
    if(ui->tableView_15->rowAt(0) != -1)      // sprawdzamy czy tableView nie jest puste
    {
        QString id_klienta = ui->lineEdit_4->text();
        database = QSqlDatabase::addDatabase("QMYSQL");
        database.setHostName("localhost");
        database.setUserName("root");
        database.setPassword("");
        database.setDatabaseName("hotel");

        if (database.open())
        {
            QSqlQuery  qry;
            qry.prepare("UPDATE pokoj SET status = 'wolny' WHERE id_pokoju = (SELECT id_pokoju FROM klient_pokoj WHERE id_klienta = :id_klienta)"

            );
            qry.bindValue(":id_klienta", id_klienta);

            if (qry.exec())
            {
                QMessageBox::information(this, "Powodzenie", "Klient został wymeldowany");
                qDebug() << qry.lastError().text();
            }
            else
            {
                QMessageBox::information(this, "Niepowodzenie", "Nie połączono");
                qDebug() << qry.lastError().text();
            }
        }
    }
}

void MainWindow::on_pushButton_82_clicked()  // powrot na page_10 (zameldowanie)
{
    ui->stackedWidget->setCurrentIndex(5);
}
