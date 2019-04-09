#include "createaccount.h"
#include "ui_createaccount.h"

#include <QMessageBox>
#include <QSqlQuery>

createAccount::createAccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createAccount)
{
    ui->setupUi(this);
}

createAccount::~createAccount()
{
    delete ui;
}

void createAccount::on_pushButton_Confirm_clicked()
{
    first = ui->lineEdit_FirstName->text();
    middle = ui->lineEdit_MiddelName->text();
    last = ui->lineEdit_LastName->text();
    username = ui->lineEdit_Username->text();
    password = ui->lineEdit_Password->text();
    type = ui->comboBox_accountType->currentText();
    status = "Offline";

    if(first == "" || middle == "" || last == "" || username == "" || password == "" || type == "")
    {
        QMessageBox::critical(this, tr("Create Account"), tr("Please complete the form...."));

        if(first == "")
        {
             ui->label_FirstName->setStyleSheet("QLabel {font-weight: bold; color: red;}");
        }
        else
        {
            ui->label_FirstName->setStyleSheet("QLabel {font-weight: color: black;}");
        }
        if(middle == "")
        {
            ui->label_MiddelName->setStyleSheet("QLabel {font-weight: bold; color: red;}");
        }
        else
        {
            ui->label_MiddelName->setStyleSheet("QLabel {font-weight: color: black;}");
        }
        if(last == "")
        {
            ui->label_LastName->setStyleSheet("QLabel {font-weight: bold; color: red;}");
        }
        else
        {
            ui->label_LastName->setStyleSheet("QLabel {font-weight: color: black;}");
        }
        if(username == "")
        {
            ui->label_Username->setStyleSheet("QLabel {font-weight: bold; color: red;}");
        }
        else
        {
            ui->label_Username->setStyleSheet("QLabel {font-weight: color: black;}");
        }
        if(password == "")
        {
            ui->label_Password->setStyleSheet("QLabel {font-weight: bold; color: red;}");
        }
        else
        {
            ui->label_Password->setStyleSheet("QLabel {font-weight: color: black;}");
        }
        if(type == "")
        {
            ui->label_Type->setStyleSheet("QLabel {font-weight: bold; color: red;}");
        }
        else
        {
            ui->label_Type->setStyleSheet("QLabel {font-weight: color: black;}");
        }
    }
    else
    {
        //Check usernames
        QSqlQuery qry0;
        qry0.prepare("select Username from accounts where Username = '"+username+"'");
        if(qry0.exec())
        {
            QString u;

            while(qry0.next())
            {
                u = qry0.value(0).toString();
            }

            if(u != "")
            {
                QMessageBox::critical(this, tr("Username"), tr("Username already exists"));
            }
            else
            {
                //Save account or create account
                QSqlQuery qry0;
                qry0.prepare("INSERT INTO `paboritocafe_v1`.`accounts` (`First`, `Middle`, `Last`, `Username`, `Password`, `Type`, `Status`) VALUES ('"+first+"', '"+middle+"', '"+last+"', '"+username+"', '"+password+"', '"+type+"', '"+status+"');");
                if(qry0.exec())
                {
                    QMessageBox::information(this, tr("Create Account"), tr("Account is created successfully!!!"));
                    this->close();
                }
            }
        }
    }
}

void createAccount::on_pushButton_Cancel_clicked()
{
    this->close();
}
