#include "managerpass.h"
#include "ui_managerpass.h"

managerPass::managerPass(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::managerPass)
{
    ui->setupUi(this);
}

managerPass::~managerPass()
{
    delete ui;
}

void managerPass::on_pushButton_OK_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Confirm", "Are you sure to MODIFY this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString pass = ui->lineEdit_Password->text();

        QSqlQuery qry;
        qry.prepare("select Password from accounts where Type = 'Manager' and Password = '"+pass+"';");
        if(qry.exec())
        {
            QString getPass;

            while(qry.next())
            {
                getPass = qry.value(0).toString();
            }

            if(pass == "")
            {
                QMessageBox::information(this, tr("Password"), tr("Please enter correct manager's password"));
            }
            else if(pass == getPass)
            {
              modifyOrderFunct();
            }
            else if(pass != getPass)
            {
                QMessageBox::information(this, tr("Password"), tr("Please enter correct manager's password"));
            }
        }
        else
        {
            QMessageBox::information(this, tr("Password"), qry.lastError().text());
        }
    }
}

void managerPass::modifyOrderFunct()
{
    QSqlQuery qry;
        qry.prepare("INSERT into orderlist (`Order Type`, `Order ID`, `Customer`, `Date`, `Time`, `Employee`, `Food Name`, `Description`, `Category`, `Price`, `Qty`, `Total`, `Payment Status`, `Status`) select `Order Type`, `Order ID`, `Customer`, `Date`, `Time`, `Employee`, `Food Name`, `Description`, `Category`, `Price`, `Qty`, `Total`, `Payment Status`, `Status` from orderstatus where `Order ID` = '"+orderID+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            qDebug() << "Transfer to Orderlist from Orderstatus";

            QSqlQuery qry;
            qry.prepare("Delete from reports where `Order ID` = '"+orderID+"'; ");
            if(qry.exec())
            {
                qDebug() << "Delete from Reports";

                QSqlQuery qry;
                qry.prepare("Delete from reports_summary where `Order ID` = '"+orderID+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Delete from Reports Summary";

                    QSqlQuery qry;
                    qry.prepare("DELETE from orderstatus where `Order ID` = '"+orderID+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from Order Status";
                        QMessageBox::information(this, tr("Modify"), ("Proceed to Orderlist Tab (Tab 1)"));
                        this->close();

//                        QString maxID;

//                        while(qry.next())
//                        {
//                            maxID = qry.value(0).toString();
//                        }

//                        QSqlQuery qry;
//                        qry.prepare("DELETE from orderstatus where `ID` = '"+maxID+"'; ");
//                        if(qry.exec())
//                        {
//                            QSqlQuery qry;
//                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+orderID+"'  and Customer = '"+cust+"'; ");
//                            if(qry.exec())
//                            {
//                                qDebug() << "Delete from Order Status";
//                                QSqlQuery qry;
//                                qry.prepare("select max(id) from orderlist where `Order ID` = '"+orderID+"'; ");
//                                if(qry.exec())
//                                {
//                                    QString maxID2;

//                                    while(qry.next())
//                                    {
//                                        maxID2 = qry.value(0).toString();
//                                    }

//                                    QSqlQuery qry;
//                                    qry.prepare("DELETE from orderlist where `ID` = '"+maxID2+"';");
//                                    if(qry.exec())
//                                    {
//                                        QSqlQuery qry;
//                                        qry.prepare("DELETE from reports where `Order ID` like '%S%' and Customer = ''; ");
//                                        if(qry.exec())
//                                        {

//                                        }
//                                        else
//                                        {
//                                            QMessageBox::critical(this, tr("Error::Delete unwanted data"), qry.lastError().text());
//                                        }
//                                    }
//                                }
//                            }
//                            else
//                            {
//                                QMessageBox::critical(this, tr("Error::Delete from Order status"), qry.lastError().text());
//                            }
//                        }
//                        else
//                        {
//                            QMessageBox::critical(this, tr("Error::Delete Max ID"), qry.lastError().text());
//                        }
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("Error::Select max ID"), qry.lastError().text());
                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("Error::Delete from reports_summary"), qry.lastError().text());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("Error::Delete from reports"), qry.lastError().text());
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::transfer to orderlist from orderstatus"), qry.lastError().text());
        }
}

void managerPass::receiveModifyOrder(QStringList mod)
{
    orderID = mod[0];
    cust = mod[1];
    time = mod[2];
}

void managerPass::on_pushButton_Cancel_clicked()
{
    this->close();
}
