#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include "createaccount.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

loginWindow::loginWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginWindow)
{
    ui->setupUi(this);

    connClose();
    connOpen();

    QApplication::setStyle("fusion"); //fusion //windows //macintosh //windowsxp

    //Count existing accounts
    QSqlQuery qry0;
    qry0.prepare("select count(id) from paboritocafe_v1.accounts;");
    if(qry0.exec())
    {
        int count = 0;

        while(qry0.next())
        {
            count = qry0.value(0).toInt();
        }

        if(count == 0)
        {
            QMessageBox::information(this, tr("Welcome"), tr("Welcome to Paborito Cafe app.\n\nThere is no existing account in your database.\n\n(1)Create new account."));
        }
    }
    else
    {
        runBatchFiles();
    }
}

loginWindow::~loginWindow()
{
    delete ui;
}

void loginWindow::connOpen()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("paboritocafe_v1");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("password");
    db.setPort(3306);

    if(db.open())
    {
        qDebug()<<"Connected!";
        ui->label_Status->setText("Connected!!!");
    }
    else
    {
        ui->label_Status->setText("Failed to connect!!!");
        qDebug()<<"Failed to connect!!!";
    }
}

void loginWindow::connClose()
{
    db.close();
    db.removeDatabase(QSqlDatabase::defaultConnection);
}

void loginWindow::runBatchFiles()
{
    //Run batch file or vbs fil

    QMessageBox::information(this, ("Welcome to MotorCab App"), ("Please wait . . . ."));
    //system("C:/Users/Public/Paborito_Cafe_Dep/dir/bat/exportDB.bat");

    system("C:/PabCafe/Paborito/Paborito_Cafe_Dep/dir/bat/importDB.bat");
    system("C:/PabCafe/Paborito/Paborito_Cafe_Dep/dir/bat/createUSERonMySQL.bat");

    connOpen();
}

void loginWindow::on_pushButton_Login_clicked()
{
    qDebug() << "Log in button clicked!!!";

    QString username, password;
    username = ui->lineEdit_Username->text();
    password = ui->lineEdit_Password->text();

    //Count existing accounts
    QSqlQuery qry0;
    qry0.prepare("select count(id) from paboritocafe_v1.accounts;");
    if(qry0.exec())
    {
        int count = 0;

        while(qry0.next())
        {
            count = qry0.value(0).toInt();
        }

        if(count == 0)
        {
            QMessageBox::information(this, tr("Account"), tr("No accounts are recorded!!!"));
            QMessageBox::StandardButton confirm;
            confirm = QMessageBox::question(this, "Confirm", "Do you wish to create a new account???", QMessageBox::Yes|QMessageBox::No);
            if(confirm == QMessageBox::Yes)
            {
                createAccount *c = new createAccount();
                c->show();
            }
        }
        else if(count != 0)
        {
            //Check account to login
            QSqlQuery qry0;
            qry0.prepare("select Username, Password from paboritocafe_v1.accounts where Username = '"+username+"' and Password = '"+password+"'; ");
            if(qry0.exec())
            {
                QString u, p;

                while(qry0.next())
                {
                    u = qry0.value(0).toString();
                    p = qry0.value(1).toString();
                }

                if(username == "" && password == "")
                {
                    QMessageBox::critical(this, tr("Log in"), tr("Please enter your account."));
                }
                else if(username == u && password == p)
                {
                    MainWindow *w = new MainWindow();
                    w->setMinimumSize(1150, 690);
                    w->showMaximized();
                    this->close();

                    connect(this, SIGNAL(sendDataAccount(QStringList)), w, SLOT(receiveDataAccountFrom(QStringList)));

                    QStringList pass;
                    pass.append(username);
                    pass.append(password);

                    emit sendDataAccount(pass);
                }
                else
                {
                    QMessageBox::critical(this, tr("Ooops!!!"), tr("Possible problem/s:\n(1)Wrong Username or Password.\n(2)Account doesn't exist."));
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Ooops!!!"), qry0.lastError().text());
            }
        }
    }
}

void loginWindow::on_pushButton_Cancel_clicked()
{
    this->close();
}

void loginWindow::on_checkBox_ShowPassword_clicked(bool checked)
{
    if(checked)
    {
        ui->lineEdit_Password->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        ui->lineEdit_Password->setEchoMode(QLineEdit::Password);
    }
}

void loginWindow::on_pushButton_CreateNewAccount_clicked()
{
    createAccount c;
    c.setModal(true);
    c.exec();
}
