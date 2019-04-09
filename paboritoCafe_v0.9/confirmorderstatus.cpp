#include "confirmorderstatus.h"
#include "ui_confirmorderstatus.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <io.h>
#include <sstream>
#include <stdlib.h>
#include <omp.h>
#include <iomanip>

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

confirmOrderStatus::confirmOrderStatus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::confirmOrderStatus)
{
    ui->setupUi(this);

    ui->label_Total->setStyleSheet("QLabel {font-weight: bold; color: red}");   
    hidePromptManager();
}

confirmOrderStatus::~confirmOrderStatus()
{
    delete ui;
}

void confirmOrderStatus::hidePromptManager()
{
    ui->label->setStyleSheet("QLabel {font-weight: bold; color: red}");
    ui->label_ManagerPasswordStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");
    ui->label->hide();
    ui->lineEdit_ManagerPassword->hide();
    ui->label_ManagerPasswordStatus->hide();
}

void confirmOrderStatus::showPromptManager()
{
    ui->label->setStyleSheet("QLabel {font-weight: bold; color: red}");
    ui->label_ManagerPasswordStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");
    ui->label->show();
    ui->lineEdit_ManagerPassword->show();
    ui->label_ManagerPasswordStatus->show();

    ui->label_ManagerPasswordStatus->setText("Enter any manager's password . . .");
}

void confirmOrderStatus::modifyOrderFunct()
{
    QSqlQuery qry;
    qry.prepare("INSERT into orderlist (`Order Type`, `Order ID`, `Customer`, `Date`, `Time`, `Employee`, `Food Name`, `Description`, `Price`, `Qty`, `Total`, `Payment Status`, `Status`) select `Order Type`, `Order ID`, `Customer`, `Date`, `Time`, `Employee`, `Food Name`, `Description`, `Price`, `Qty`, `Total`, `Payment Status`, `Status` from orderstatus where `Order ID` = '"+orderID+"'; ");
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
                qry.prepare("select max(id) from orderstatus where `Order ID` = '"+orderID+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Delete max id from Order Status";

                    QString maxID;

                    while(qry.next())
                    {
                        maxID = qry.value(0).toString();
                    }

                    QSqlQuery qry;
                    qry.prepare("DELETE from orderstatus where `ID` = '"+maxID+"'; ");
                    if(qry.exec())
                    {
                        QSqlQuery qry;
                        qry.prepare("DELETE from orderstatus where `Order ID` = '"+orderID+"'; ");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from Order Status";
                            QSqlQuery qry;
                            qry.prepare("select max(id) from orderlist where `Order ID` = '"+orderID+"'; ");
                            if(qry.exec())
                            {
                                QString maxID2;

                                while(qry.next())
                                {
                                    maxID2 = qry.value(0).toString();
                                }

                                QSqlQuery qry;
                                qry.prepare("DELETE from orderlist where `ID` = '"+maxID2+"';");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Modify"), ("Proceed to Orderlist Tab (Tab 1)"));
                                    this->close();
                                }
                            }
                        }
                        else
                        {
                            QMessageBox::critical(this, tr("Error::Delete from Order status"), qry.lastError().text());
                        }
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("Error::Delete Max ID"), qry.lastError().text());
                    }
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

void confirmOrderStatus::confirmOrderFunct()
{
    QSqlQuery qry;
    qry.prepare("select replace(max(distinct `Order ID`), 'S', '') from `paboritocafe_v1`.`orderid`;");
    if(qry.exec())
    {
        qDebug() << "\n Selecting Final Order ID";
        QString newOrderID;
        while(qry.next())
        {
            newOrderID = qry.value(0).toString();
        }

        std::ostringstream ss;
        ss << std::setw(8) << std::setfill('0') << newOrderID.toInt() + 1;
        std::string s = ss.str();

        QString num;
        num = QString::fromStdString(s);

        QString finalOrderID = "S" + num;

        QSqlQuery qry;
        qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+orderID+"' and Customer = '"+customerName+"' and Time = '"+time+"'; ");
        if(qry.exec())
        {
            qDebug() << "Update Reports";

            QSqlQuery qry;
            qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+orderID+"' and Customer = '"+customerName+"' and Time = '"+time+"'; ");
            if(qry.exec())
            {
                qDebug() << "Update Reports Summary";

                QSqlQuery qry;
                qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+orderID+"'; ");
                if(qry.exec())
                {
                    QString food, des, cat, qt;

                    while(qry.next())
                    {
                        food = qry.value(0).toString();
                        des = qry.value(1).toString();
                        cat = qry.value(2).toString();
                        qt = qry.value(3).toString();

                        QSqlQuery qry;
                        qry.prepare("UPDATE menu SET `Sold` = Sold + '"+qt+"' where `Food Name` = '"+food+"' and Description = '"+des+"' and Category = '"+cat+"'; ");
                        if(qry.exec())
                        {
                            qDebug() << "Updating Sold Items";
                        }
                    }

                    QSqlQuery qry;
                    qry.prepare("DELETE from orderstatus where `Order ID` = '"+orderID+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from Order Status";

                        QSqlQuery qry;
                        qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                        if(qry.exec())
                        {
                            QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                            qDebug() << "Order Confirmed";
                            this->close();
                        }
                        else
                        {
                            QMessageBox::critical(this, tr(""), qry.lastError().text());
                        }
                    }
                    else
                    {
                        QMessageBox::critical(this, tr(""), qry.lastError().text());
                    }
                }
            }
            else
            {
                QMessageBox::critical(this, tr(""), qry.lastError().text());
            }
        }
        else
        {
            QMessageBox::critical(this, tr(""), qry.lastError().text());
        }
    }
    else
    {
        QMessageBox::critical(this, tr(""), qry.lastError().text());
    }
}

void confirmOrderStatus::cancelOrderFunct()
{
    QSqlQuery qry;
    qry.prepare("DELETE from orderstatus where `Order ID` = '"+orderID+"';");
    if(qry.exec())
    {
        qDebug() << "Delete from orderstatus";

        QSqlQuery qry;
        qry.prepare("select max(id) from reports where `Order ID` = '"+orderID+"'; ");
        if(qry.exec())
        {
            QString maxID;

            while(qry.next())
            {
                maxID = qry.value(0).toString();
            }

            QSqlQuery qry;
            qry.prepare("DELETE from reports where `ID` = '"+maxID+"';");
            if(qry.exec())
            {
                QSqlQuery qry;
                qry.prepare("DELETE from reports where `Order ID` = '"+orderID+"' and Time = '"+time+"' and Customer = '"+customerName+"';");
                if(qry.exec())
                {
                    qDebug() << "Delete from reports";
                    QSqlQuery qry;
                    qry.prepare("DELETE from reports_summary where `Order ID` = '"+orderID+"' and Time = '"+time+"' and Customer = '"+customerName+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports_summary";
                        this->close();
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("reports_summary"), qry.lastError().text());
                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("reports"), qry.lastError().text());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("delete max id"), qry.lastError().text());
            }
        }
        else
        {
            QMessageBox::critical(this, tr("reports Max id"), qry.lastError().text());
        }
    }
    else
    {
        QMessageBox::critical(this, tr("orderstatus"), qry.lastError().text());
    }
}

void confirmOrderStatus::connOpen()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("paboritocafe_v1");
    db.setHostName("localhost");
    db.setUserName("paborito");
    db.setPassword("paborito");
    db.setPort(3306);

    if(db.open())
    {
        qDebug()<<"Connected!";
        //ui->label_Status->setText("Connected!!!");
    }
    else
    {
        //ui->label_Status->setText("Failed to connect!!!");
        qDebug()<<"Failed to connect!!!";
    }
}

void confirmOrderStatus::connClose()
{
    QSqlDatabase db;
    db.close();
    db.removeDatabase(QSqlDatabase::defaultConnection);
}

void confirmOrderStatus::receiveOrderStatus(QStringList orderStatus)
{
    orderType = orderStatus[0];
    orderID = orderStatus[1];
    time = orderStatus[2];
    customerName = orderStatus[3];

    qDebug() << "Received: " + orderType + " " + orderID;

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("select *from orderstatus where `Order ID` = '"+orderID+"' ORDER BY ID asc; ");
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView->setModel(model);
        ui->tableView->setColumnHidden(0, true);

        ui->tableView->setVisible(false);
        ui->tableView->resizeColumnsToContents();
        ui->tableView->resizeRowsToContents();
        ui->tableView->setVisible(true);

        ui->label_OrderID->setText(orderID);

        QSqlQuery qry;
        qry.prepare("select sum(Total) from orderstatus where `Order ID` = '"+orderID+"'; ");
        if(qry.exec())
        {
            QString textTotal, total;
            textTotal = ui->label_Total->text();

            while(qry.next())
            {
                total = qry.value(0).toString();
            }

            ui->label_Total->setText(textTotal + total);
        }
    }
}

void confirmOrderStatus::on_pushButton_ModifyOrder_clicked()
{
    //Modify Order
    qDebug() << "\nModify Order";



    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Confirm", "Are you sure to MODIFY this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        showPromptManager();

        QString pass = ui->lineEdit_ManagerPassword->text();

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
              hidePromptManager();
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

void confirmOrderStatus::on_pushButton_ConfirmOrder_clicked()
{
    hidePromptManager();
    //Confirm Order

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Confirm", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        confirmOrderFunct();
    }
}

void confirmOrderStatus::on_pushButton_CancelOrder_clicked()
{
    hidePromptManager();
    //Cancel Order
    qDebug() << "\nCancel Order";

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Confirm", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        cancelOrderFunct();
    }
}

void confirmOrderStatus::on_pushButton_Close_clicked()
{
    //Close Order
    qDebug() << "Close Order";
    this->close();
}

void confirmOrderStatus::on_lineEdit_ManagerPassword_textChanged(const QString &arg1)
{
    QString pass = ui->lineEdit_ManagerPassword->text();

    QSqlQuery qry;
    qry.prepare("select Password from accounts where Type = 'Manager' and Password = '"+pass+"'; ");
    if(qry.exec())
    {
        qDebug() << "Select Managers Password . . .";

        QString p;
        while(qry.next())
        {
            p = qry.value(0).toString();
        }

        if(pass == "")
        {
            ui->label_ManagerPasswordStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");
            ui->label_ManagerPasswordStatus->setText("Wrong Password ....");
        }
        else if(p == pass)
        {
            ui->label_ManagerPasswordStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");
            ui->label_ManagerPasswordStatus->setText("Correct Password....  Click 'Modify Order' button again....");
        }
        else
        {
            ui->label_ManagerPasswordStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");
            ui->label_ManagerPasswordStatus->setText("Wrong Password ....");
        }
    }
}
