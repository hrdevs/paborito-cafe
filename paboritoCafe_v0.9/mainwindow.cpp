#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "loginwindow.h"
#include "newitem.h"
#include "edititem.h"
#include "createaccount.h"
#include "confirmorderstatus.h"
#include "kitchenview.h"
#include "editaccount.h"
#include "adddiscount.h"
#include "addvat.h"

#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QTextDocument>

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlTableModel>

#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <QPixmap>
#include <QToolButton>

#include <algorithm>
#include <string>
#include <cmath>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <io.h>
#include <omp.h>
#include <thread>
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->label_OrderTotal->setStyleSheet("QLabel {font-weight: bold; color: red}");

    HomePage();

    QDate showDate = QDate::currentDate();
    date = locale().toString(showDate, "yyyy-MMM-dd");

    on_pushButton_OrderClear_clicked();
    HomePage();

    ui->comboBox_ReportsSortByDate->setDisabled(true);
    ui->comboBox_ReportsSortByCustomer->setDisabled(true);
    ui->comboBox_ReportsSortByEmp->setDisabled(true);

    ui->pushButton_Export2ExcelMonthlyReports->hide();
    ui->pushButton_ChangeView->hide();

    clearOrders();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::HomePage()
{
    //Home Page

    qDebug() << "\nHome Page";
    ui->stackedWidget->setCurrentWidget(ui->HomePage);

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("SELECT * FROM paboritocafe_v1.menu ORDER BY Sold desc;");
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_MenuPage->setModel(model);

        ui->tableView_MenuPage->setColumnHidden(0, true);
        ui->tableView_MenuPage->setColumnHidden(6, true);

        ui->tableView_MenuPage->setVisible(false);
        ui->tableView_MenuPage->resizeColumnsToContents();
        ui->tableView_MenuPage->resizeRowsToContents();
        ui->tableView_MenuPage->setVisible(true);
    }
    else
    {
        QMessageBox::critical(this, tr("Error::Best Sellers Table"), qry->lastError().text());
    }
}

void MainWindow::OrderPage()
{
    //Order Page    
    qDebug() << "\nOrder Page";
    ui->stackedWidget->setCurrentWidget(ui->OrderPage);
}

void MainWindow::MenuPage()
{
    //Menu Page

    qDebug() << "\nMenu Page";
    ui->stackedWidget->setCurrentWidget(ui->MenuPage);
    on_pushButton_MenuRiceBowls_clicked();
}

void MainWindow::ManageAccountsPage()
{
    //Accounts Page

    qDebug() << "\nManage Accounts Page";
    ui->stackedWidget->setCurrentWidget(ui->ManageAccountsPage);    
}

void MainWindow::ReportsPage()
{
    //Reports Page

    qDebug() << "\nReports Page Button clicked";
    ui->stackedWidget->setCurrentWidget(ui->ReportsPage);
}

void MainWindow::setMenuTable(QString& categ)
{
    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("select ID, `Food Name`, Description, Price, Category, Sold, Image from `paboritocafe_v1`.`menu` where Category = '"+categ+"' ORDER BY `Food Name` asc;");
    if(qry->exec())
    {
        QString id, foodName, description, price, category, image;

        while(qry->next())
        {
            id = qry->value(0).toString();
            foodName = qry->value(1).toString();
            description = qry->value(2).toString();
            price = qry->value(3).toString();
            category = qry->value(4).toString();
            image = qry->value(5).toString();

            qDebug() << id + " " + foodName + " " + description + " " + price + " " + category + " " + image;
        }

        qDebug() << "Set Menu Table by: " + categ;

        QSqlQueryModel *model = new QSqlQueryModel;
        model->setQuery(*qry);

        ui->tableView_Menu->setModel(model);
        ui->tableView_Menu->setColumnHidden(0, true);
        ui->tableView_Menu->setColumnHidden(4, true);
        ui->tableView_Menu->setColumnHidden(6, true);

        ui->tableView_Menu->setVisible(false);
        ui->tableView_Menu->resizeColumnsToContents();
        ui->tableView_Menu->resizeRowsToContents();
        ui->tableView_Menu->setVisible(true);
    }
    else
    {
        QMessageBox::critical(this, tr("Error::Set Menu Table"), qry->lastError().text());
    }
}

void MainWindow::addAccount()
{
    qDebug() << "\nCreate Account";
    createAccount c;
    c.setModal(true);
    c.exec();

    on_pushButton_ManageAccounts_clicked();
}

void MainWindow::delAccount()
{
    qDebug() << "\nDelete Account";

    int loc = ui->tableView_ManageAccounts->currentIndex().row();

    QString id;
    id = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 0)).toString();

    QSqlQuery qry;
    qry.prepare("DELETE FROM `paboritocafe_v1`.`accounts` WHERE `ID`='"+id+"'; ");
    if(qry.exec())
    {
        on_pushButton_ManageAccounts_clicked();
    }
    else
    {
        QMessageBox::critical(this, tr("Error::Delete Account"), qry.lastError().text());
    }
}

void MainWindow::orderList()
{
    QString orderID = ui->lineEdit_OrderID->text();
    QString textTotal = "";

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("select *from `paboritocafe_v1`.`orderlist` ORDER BY IF(`Order ID` RLIKE '^[a-z]', 1, 2), `Order ID`;");
    if(qry->exec())
    {
        qDebug() << "Load Orderlist Data!!!";

        model->setQuery(*qry);
        ui->tableView_OrderList->setModel(model);
        ui->tableView_OrderList->setColumnHidden(0, true); //ID
        //ui->tableView_OrderList->setColumnHidden(1, true); //Order Type
        ui->tableView_OrderList->setColumnHidden(2, true); //Order ID
        ui->tableView_OrderList->setColumnHidden(3, true);
        ui->tableView_OrderList->setColumnHidden(4, true);
        ui->tableView_OrderList->setColumnHidden(5, true);
        ui->tableView_OrderList->setColumnHidden(12, true);

        ui->tableView_OrderList->setVisible(false);
        ui->tableView_OrderList->resizeColumnsToContents();
        ui->tableView_OrderList->resizeRowsToContents();
        ui->tableView_OrderList->setVisible(true);
    }
    else
    {
        QMessageBox::critical(this, tr("Error::Set Order List"), qry->lastError().text());
    }

    QSqlQuery qry2;
    qry2.prepare("select sum(Total) from `paboritocafe_v1`.`orderlist` where `Order ID` = '"+orderID+"'; ");
    if(qry2.exec())
    {
        QString total;
        while(qry2.next())
        {
            total = qry2.value(0).toString();

            ui->lineEdit_OrderTotal->setText(textTotal + total);

            double vat, discount, total, grandTotal;
            vat = ui->comboBox_OrderVAT->currentText().toDouble() / 100;
            discount = ui->comboBox_OrderDiscount->currentText().toDouble() / 100;
            total = ui->lineEdit_OrderTotal->text().toDouble();

            double finalVat, finalDiscount;
            finalVat = ((1 + vat) * total);

            //calculate vat
            ui->doubleSpinBox_GrandTotal->setValue(finalVat);

            finalDiscount = finalVat - ((discount) * finalVat);

            //calculate discount
            ui->doubleSpinBox_GrandTotal->setValue(finalDiscount);
        }
    }
}

void MainWindow::orderStatus()
{
    QSqlQueryModel *model, *model1;
    model = new QSqlQueryModel;
    model1 = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("select *from `paboritocafe_v1`.`orderstatus` ORDER BY IF(`Order ID` RLIKE '^[a-z]', 1, 2), `Order ID`; ");
    if(qry->exec())
    {
        qDebug() << "Order Status (Dine In)";

        model->setQuery(*qry);
        ui->tableView_OrderStatus->setModel(model);
        ui->tableView_OrderStatus->setColumnHidden(0, true);
        //ui->tableView_OrderStatus->setColumnHidden(1, true);
        ui->tableView_OrderStatus->setColumnHidden(9, true);
        ui->tableView_OrderStatus->setVisible(false);
        ui->tableView_OrderStatus->resizeColumnsToContents();
        ui->tableView_OrderStatus->resizeRowsToContents();
        ui->tableView_OrderStatus->setVisible(true);

//        QSqlQuery qry;
//        qry.prepare("select ID from orderstatus where Customer = '' and Date = '';");
//        if(qry.exec())
//        {
//            QString id;

//            while(qry.next())
//            {
//                id = qry.value(0).toString();

//                QSqlQuery qry;
//                qry.prepare("Delete from orderstatus where ID = '"+id+"'; ");
//                if(qry.exec())
//                {
//                    qDebug()<< "Delete unwanted records";
//                }
//                else
//                {
//                    QMessageBox::critical(this, tr("Error::Delete unwanted data"), qry.lastError().text());
//                }
//            }
//        }
//        else
//        {
//            QMessageBox::critical(this, tr("Error::Delete unwanted data"), qry.lastError().text());
//        }
    }
}

void MainWindow::orderStatus_2()
{
    //Get Orders
    clearOrders();

    getOrder[0] = "";
    getOrder[1] = "";
    getOrder[2] = "";
    getOrder[3] = "";
    getOrder[4] = "";
    getOrder[5] = "";
    getOrder[6] = "";
    getOrder[7] = "";
    getOrder[8] = "";
    getOrder[9] = "";

    QSqlQuery qry;
    qry.prepare("select distinct `Order ID` from paboritocafe_v1.orderstatus ORDER BY `Order ID` asc; ");
    if(qry.exec())
    {
        int count = 0;
        //qDebug() << "Fetch Order:";

        while(qry.next())
        {
            getOrder[count] = qry.value(0).toString();
            //qDebug() << getOrder[count];

            count++;
        }

        if(getOrder[0] == "" || getOrder[1] == "" || getOrder[2] == "" || getOrder[3] == "" || getOrder[4] == "" || getOrder[5] == "" || getOrder[6] == "" || getOrder[7] == "" || getOrder[8] == "" || getOrder[9] == "")
        {
            return;
        }
            //qDebug() << "Done fetching Order";
    }
    else
    {
        QMessageBox::critical(this, ("Error::Getting Orders"), qry.lastError().text());
    }
}

void MainWindow::refreshOrderListStatus()
{
    orderList();
    orderStatus();
    setOrders();
}

void MainWindow::loadReportsSalesDiscTotal()
{
    int row = ui->tableView_ReportsSummary->model()->rowCount();
    double salesVal = 0, discVal = 0, vaTtotal = 0, totalVal = 0;

    for(int x = 0; x <= row; x++)
    {
        QString sales = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(x, 7)).toString();
        QString disc = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(x, 8)).toString();
        QString vat = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(x, 9)).toString();
        QString total = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(x, 10)).toString();

        salesVal = salesVal + sales.toDouble();
        discVal = discVal + disc.toDouble();
        vaTtotal = vaTtotal + vat.toDouble();
        totalVal = totalVal + total.toDouble();
    }

    ui->doubleSpinBox_ReportsSales->setValue(salesVal);
    ui->doubleSpinBox_ReportsDiscount->setValue(discVal);
    ui->doubleSpinBox_ReportsVat->setValue(vaTtotal);
    ui->doubleSpinBox_ReportsTotal->setValue(totalVal);
}

void MainWindow::loadVat()
{
    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("SELECT value FROM paboritocafe_v1.vat;");
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->comboBox_OrderVAT->setModel(model);

        ui->comboBox_OrderVAT->setCurrentIndex(0);
    }
}

void MainWindow::loadDiscount()
{
    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("SELECT value FROM paboritocafe_v1.discount_type;");
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->comboBox_OrderDiscount->setModel(model);

        ui->comboBox_OrderDiscount->setCurrentIndex(0);
    }
}

void MainWindow::receiveDataAccountFrom(QStringList pass)
{
    username = pass[0];
    password = pass[1];

    qDebug() << "Username: " + username;
    qDebug() << "Password: " + password;

    QSqlQuery qry0;
    qry0.prepare("select First, Middle, Last, Type from `paboritocafe_v1`.`accounts` where Username = '"+username+"' and Password = '"+password+"';");
    if(qry0.exec())
    {
        while(qry0.next())
        {
            first = qry0.value(0).toString();
            middle = qry0.value(1).toString();
            last = qry0.value(2).toString();
            type = qry0.value(3).toString();
        }

        ui->label_HomePageEmployeeName->setText(first + " " + middle + " " + last);
        ui->label_HomePageDate->setText(date);
        ui->label_HomePageAccountType->setText(type);

        if(type == "Manager")
        {
            ui->pushButton_MenuAdd->show();
            ui->pushButton_MenuDel->show();
        }
        else if(type == "Cashier")
        {
            ui->pushButton_MenuAdd->hide();
            ui->pushButton_MenuDel->hide();
            ui->pushButton_ReportsPage->hide();
            ui->pushButton_ManageAccounts->hide();
            ui->label_Reports->hide();
            ui->label_ManageAccounts->hide();
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error Getting Account Info!!!"), qry0.lastError().text());
    }
}

void MainWindow::on_pushButton_Logout_clicked()
{
    qDebug() << "\nLog Out";

    this->close();

    loginWindow l;
    l.setModal(true);
    l.exec();
}

void MainWindow::on_pushButton_OrderPage_clicked()
{
    //Order Page PushButton
    qDebug() << "\n Order Page PushButton clicked";   

    OrderPage();

    on_pushButton_OrderClear_clicked();

    refreshOrderListStatus();

    loadVat();
    loadDiscount();
}

void MainWindow::on_pushButton_OrderPage_HomePage_clicked()
{    
    int row = ui->tableView_OrderList->model()->rowCount();

    if(row > 0)
    {
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::question(this, "Confirm", "Order on going!!!\n\n(1)IF you click YES, all orders will be cleared.", QMessageBox::Yes|QMessageBox::No);
        if(confirm == QMessageBox::Yes)
        {
            on_pushButton_OrderClear_clicked();
            HomePage();
        }
    }
    else
    {
        on_pushButton_OrderClear_clicked();
        HomePage();
    }
}

void MainWindow::on_pushButton_MenuPage_clicked()
{
    MenuPage();
}

void MainWindow::on_pushButton_MenuHomePage_clicked()
{
    HomePage();
}

void MainWindow::on_pushButton_ReportsPage_clicked()
{
    //Reports PushButton
    ReportsPage();

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("select *from `paboritocafe_v1`.`reports_summary`; ");
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_ReportsSummary->setModel(model);

        ui->tableView_ReportsSummary->setColumnHidden(0, true);

        ui->tableView_ReportsSummary->setVisible(false);
        ui->tableView_ReportsSummary->resizeColumnsToContents();
        ui->tableView_ReportsSummary->resizeRowsToContents();
        ui->tableView_ReportsSummary->setVisible(true);
    }
    else
    {
        QMessageBox::critical(this, tr("Reports::Error::1"), qry->lastError().text());
    }

    loadReportsSalesDiscTotal();
}

void MainWindow::on_pushButton_ReportsHomePage_clicked()
{
    HomePage();
}

void MainWindow::on_pushButton_ManageAccounts_clicked()
{
    //Accounts PushButton
    ManageAccountsPage();

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("select *from `paboritocafe_v1`.`accounts` ORDER BY Type desc; ");
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_ManageAccounts->setModel(model);

        ui->tableView_ManageAccounts->setColumnHidden(0, true);
        ui->tableView_ManageAccounts->setColumnHidden(5, true);
        ui->tableView_ManageAccounts->setColumnHidden(7, true);

        ui->tableView_ManageAccounts->setVisible(false);
        ui->tableView_ManageAccounts->resizeColumnsToContents();
        ui->tableView_ManageAccounts->resizeRowsToContents();
        ui->tableView_ManageAccounts->setVisible(true);
    }
    else
    {
        QMessageBox::critical(this, tr("Accounts::Error::1"), qry->lastError().text());
    }
}

void MainWindow::on_pushButton_ManageAccountsHomePage_clicked()
{
    HomePage();
}

void MainWindow::on_pushButton_MenuAdd_clicked()
{
    newItem x;
    x.setModal(true);
    x.exec();
}

void MainWindow::on_pushButton_MenuRiceBowls_clicked()
{
    ui->label_MenuTable->setText("Rice Bowls");

    QString categ = ui->label_MenuTable->text();

    setMenuTable(categ);
}

void MainWindow::on_pushButton_MenuFingerFoods_clicked()
{
    ui->label_MenuTable->setText("Finger Foods");

    QString categ = ui->label_MenuTable->text();

    setMenuTable(categ);
}

void MainWindow::on_pushButton_MenuPasta_clicked()
{
    ui->label_MenuTable->setText("Pasta");

    QString categ = ui->label_MenuTable->text();

    setMenuTable(categ);
}

void MainWindow::on_pushButton_MenuPizza_clicked()
{
    ui->label_MenuTable->setText("Pizza");

    QString categ = ui->label_MenuTable->text();

    setMenuTable(categ);
}

void MainWindow::on_pushButton_MenuSpecialty_clicked()
{
    ui->label_MenuTable->setText("Specialty");

    QString categ = ui->label_MenuTable->text();

    setMenuTable(categ);
}

void MainWindow::on_pushButton_MenuColdDrinks_clicked()
{
    ui->label_MenuTable->setText("Cold Drinks");

    QString categ = ui->label_MenuTable->text();

    setMenuTable(categ);
}

void MainWindow::on_pushButton_MenuHotDrinls_clicked()
{
    ui->label_MenuTable->setText("Hot Drinks");

    QString categ = ui->label_MenuTable->text();

    setMenuTable(categ);
}

void MainWindow::on_pushButton_MenuOthers_clicked()
{
    ui->label_MenuTable->setText("Others");

    QString categ = ui->label_MenuTable->text();

    setMenuTable(categ);
}

void MainWindow::on_pushButton_MenuDel_clicked()
{
    int loc = ui->tableView_Menu->currentIndex().row();

    QString id, foodName;
    id = ui->tableView_Menu->model()->data(ui->tableView_Menu->model()->index(loc, 0)).toString();
    foodName = ui->tableView_Menu->model()->data(ui->tableView_Menu->model()->index(loc, 1)).toString();

    if(foodName == "")
    {
        QMessageBox::critical(this, tr("No item selected to delete."), tr("No item selected to delete."));
    }
    else if(foodName != "")
    {
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::question(this, "Delete Item", "Click Yes to delete " + foodName + " from the Menu", QMessageBox::Yes|QMessageBox::No);
        if(confirm == QMessageBox::Yes)
        {
            QSqlQuery qry;
            qry.prepare("DELETE FROM `paboritocafe_v1`.`menu` WHERE `ID`='"+id+"'; ");
            if(qry.exec())
            {
                QMessageBox::information(this, tr("Item Deleted"), tr("Item deleted from the menu . . ."));
            }
            else
            {
                QMessageBox::critical(this, tr("Error::Delete Menu"), qry.lastError().text());
            }
        }
    }
}

void MainWindow::on_tableView_Menu_doubleClicked(const QModelIndex &index)
{
    QString type = ui->label_HomePageAccountType->text();

    int loc = ui->tableView_Menu->currentIndex().row();

    QString id, foodName, desc, price, categ, image;
    id = ui->tableView_Menu->model()->data(ui->tableView_Menu->model()->index(loc, 0)).toString();
    foodName = ui->tableView_Menu->model()->data(ui->tableView_Menu->model()->index(loc, 1)).toString();
    desc = ui->tableView_Menu->model()->data(ui->tableView_Menu->model()->index(loc, 2)).toString();
    price = ui->tableView_Menu->model()->data(ui->tableView_Menu->model()->index(loc, 3)).toString();
    categ = ui->tableView_Menu->model()->data(ui->tableView_Menu->model()->index(loc, 4)).toString();
    image = ui->tableView_Menu->model()->data(ui->tableView_Menu->model()->index(loc, 6)).toString();


    qDebug() << "Table Menu Clicked";
    qDebug() << id + " " + foodName + " " + desc + " " + price + " " + categ + " " + image;


    editItem *x = new editItem();

    if(type == "Manager")
    {
        x->show();

        connect(this, SIGNAL(sendEditItem(QStringList)), x, SLOT(receiveEditItem(QStringList)));

        QStringList gfoodName;
        gfoodName.append(id);
        gfoodName.append(foodName);
        gfoodName.append(desc);
        gfoodName.append(price);
        gfoodName.append(categ);
        gfoodName.append(image);

        emit sendEditItem(gfoodName);
    }
}

void MainWindow::on_tableView_Menu_clicked(const QModelIndex &index)
{
    int loc = ui->tableView_Menu->currentIndex().row();

    QString image;
    image = ui->tableView_Menu->model()->data(ui->tableView_Menu->model()->index(loc, 6)).toString();

    QPixmap pic(image);
    ui->label_MenuImage->setPixmap(pic.scaled(250, 300, Qt::KeepAspectRatioByExpanding));
    ui->label_MenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //ui->pushButton_MenuAdd->setIcon(QIcon(pic));
    //ui->pushButton_MenuAdd->setIconSize(QSize(200, 200));
}

void MainWindow::on_pushButton_OrderRiceBowls_clicked()
{  
    QString categ = "Rice Bowls";
    QString qryText = "select ID, `Food Name`, Description, Price, Category from `paboritocafe_v1`.`menu` where Category = '"+categ+"'";

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare(qryText);
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_OrderMenu->setModel(model);
        ui->tableView_OrderMenu->setColumnHidden(0, true);
        ui->tableView_OrderMenu->setColumnHidden(4, true);

        ui->tableView_OrderMenu->setVisible(false);
        ui->tableView_OrderMenu->resizeColumnsToContents();
        ui->tableView_OrderMenu->resizeRowsToContents();
        ui->tableView_OrderMenu->setVisible(true);
    }

    QString image = "C:/Users/Harn Ryan/Documents/Qt/Paborito Cafe Source Code/Default Image.jpg";

    QPixmap pic(image);
    ui->label_OrderMenuImage->setPixmap(pic.scaled(120, 180, Qt::KeepAspectRatioByExpanding));
    ui->label_OrderMenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::on_pushButton_OrderFingerFoods_clicked()
{
    QString categ = "Finger Foods";
    QString qryText = "select ID, `Food Name`, Description, Price, Category from `paboritocafe_v1`.`menu` where Category = '"+categ+"'";

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare(qryText);
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_OrderMenu->setModel(model);
        ui->tableView_OrderMenu->setColumnHidden(0, true);
        ui->tableView_OrderMenu->setColumnHidden(4, true);

        ui->tableView_OrderMenu->setVisible(false);
        ui->tableView_OrderMenu->resizeColumnsToContents();
        ui->tableView_OrderMenu->resizeRowsToContents();
        ui->tableView_OrderMenu->setVisible(true);
    }

    QString image = "C:/Users/Harn Ryan/Documents/Qt/Paborito Cafe Source Code/Default Image.jpg";

    QPixmap pic(image);
    ui->label_OrderMenuImage->setPixmap(pic.scaled(120, 180, Qt::KeepAspectRatioByExpanding));
    ui->label_OrderMenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::on_pushButton_OrderPasta_clicked()
{
    QString categ = "Pasta";
    QString qryText = "select ID, `Food Name`, Description, Price, Category from `paboritocafe_v1`.`menu` where Category = '"+categ+"'";

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare(qryText);
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_OrderMenu->setModel(model);
        ui->tableView_OrderMenu->setColumnHidden(0, true);
        ui->tableView_OrderMenu->setColumnHidden(4, true);

        ui->tableView_OrderMenu->setVisible(false);
        ui->tableView_OrderMenu->resizeColumnsToContents();
        ui->tableView_OrderMenu->resizeRowsToContents();
        ui->tableView_OrderMenu->setVisible(true);
    }

    QString image = "C:/Users/Harn Ryan/Documents/Qt/Paborito Cafe Source Code/Default Image.jpg";

    QPixmap pic(image);
    ui->label_OrderMenuImage->setPixmap(pic.scaled(120, 180, Qt::KeepAspectRatioByExpanding));
    ui->label_OrderMenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::on_pushButton_OrderPizza_clicked()
{
    QString categ = "Pizza";
    QString qryText = "select ID, `Food Name`, Description, Price, Category from `paboritocafe_v1`.`menu` where Category = '"+categ+"'";

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare(qryText);
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_OrderMenu->setModel(model);
        ui->tableView_OrderMenu->setColumnHidden(0, true);
        ui->tableView_OrderMenu->setColumnHidden(4, true);

        ui->tableView_OrderMenu->setVisible(false);
        ui->tableView_OrderMenu->resizeColumnsToContents();
        ui->tableView_OrderMenu->resizeRowsToContents();
        ui->tableView_OrderMenu->setVisible(true);
    }

    QString image = "C:/Users/Harn Ryan/Documents/Qt/Paborito Cafe Source Code/Default Image.jpg";

    QPixmap pic(image);
    ui->label_OrderMenuImage->setPixmap(pic.scaled(120, 180, Qt::KeepAspectRatioByExpanding));
    ui->label_OrderMenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::on_pushButton_OrderSpecialty_clicked()
{
    QString categ = "Specialty";
    QString qryText = "select ID, `Food Name`, Description, Price, Category from `paboritocafe_v1`.`menu` where Category = '"+categ+"'";

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare(qryText);
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_OrderMenu->setModel(model);
        ui->tableView_OrderMenu->setColumnHidden(0, true);
        ui->tableView_OrderMenu->setColumnHidden(4, true);

        ui->tableView_OrderMenu->setVisible(false);
        ui->tableView_OrderMenu->resizeColumnsToContents();
        ui->tableView_OrderMenu->resizeRowsToContents();
        ui->tableView_OrderMenu->setVisible(true);
    }

    QString image = "C:/Users/Harn Ryan/Documents/Qt/Paborito Cafe Source Code/Default Image.jpg";

    QPixmap pic(image);
    ui->label_OrderMenuImage->setPixmap(pic.scaled(120, 180, Qt::KeepAspectRatioByExpanding));
    ui->label_OrderMenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::on_pushButton_OrderColdDrinks_clicked()
{
    QString categ = "Cold Drinks";
    QString qryText = "select ID, `Food Name`, Description, Price, Category from `paboritocafe_v1`.`menu` where Category = '"+categ+"'";

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare(qryText);
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_OrderMenu->setModel(model);
        ui->tableView_OrderMenu->setColumnHidden(0, true);
        ui->tableView_OrderMenu->setColumnHidden(4, true);

        ui->tableView_OrderMenu->setVisible(false);
        ui->tableView_OrderMenu->resizeColumnsToContents();
        ui->tableView_OrderMenu->resizeRowsToContents();
        ui->tableView_OrderMenu->setVisible(true);
    }

    QString image = "C:/Users/Harn Ryan/Documents/Qt/Paborito Cafe Source Code/Default Image.jpg";

    QPixmap pic(image);
    ui->label_OrderMenuImage->setPixmap(pic.scaled(120, 180, Qt::KeepAspectRatioByExpanding));
    ui->label_OrderMenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::on_pushButton_OrderHotDrinks_clicked()
{
    QString categ = "Hot Drinks";
    QString qryText = "select ID, `Food Name`, Description, Price, Category from `paboritocafe_v1`.`menu` where Category = '"+categ+"'";
    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare(qryText);
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_OrderMenu->setModel(model);
        ui->tableView_OrderMenu->setColumnHidden(0, true);
        ui->tableView_OrderMenu->setColumnHidden(4, true);

        ui->tableView_OrderMenu->setVisible(false);
        ui->tableView_OrderMenu->resizeColumnsToContents();
        ui->tableView_OrderMenu->resizeRowsToContents();
        ui->tableView_OrderMenu->setVisible(true);
    }

    QString image = "C:/Users/Harn Ryan/Documents/Qt/Paborito Cafe Source Code/Default Image.jpg";

    QPixmap pic(image);
    ui->label_OrderMenuImage->setPixmap(pic.scaled(120, 180, Qt::KeepAspectRatioByExpanding));
    ui->label_OrderMenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::on_pushButton_OrderOthers_clicked()
{
    QString categ = "Others";
    QString qryText = "select ID, `Food Name`, Description, Price, Category from `paboritocafe_v1`.`menu` where Category = '"+categ+"'";

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare(qryText);
    if(qry->exec())
    {
        model->setQuery(*qry);
        ui->tableView_OrderMenu->setModel(model);
        ui->tableView_OrderMenu->setColumnHidden(0, true);
        ui->tableView_OrderMenu->setColumnHidden(4, true);

        ui->tableView_OrderMenu->setVisible(false);
        ui->tableView_OrderMenu->resizeColumnsToContents();
        ui->tableView_OrderMenu->resizeRowsToContents();
        ui->tableView_OrderMenu->setVisible(true);
    }

    QString image = "C:/Users/Harn Ryan/Documents/Qt/Paborito Cafe Source Code/Default Image.jpg";

    QPixmap pic(image);
    ui->label_OrderMenuImage->setPixmap(pic.scaled(120, 180, Qt::KeepAspectRatioByExpanding));
    ui->label_OrderMenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void MainWindow::on_tableView_OrderMenu_clicked(const QModelIndex &index)
{
    int loc = ui->tableView_OrderMenu->currentIndex().row();

    QString id, foodName, price, image;
    id = ui->tableView_OrderMenu->model()->data(ui->tableView_OrderMenu->model()->index(loc, 0)).toString();
    foodName = ui->tableView_OrderMenu->model()->data(ui->tableView_OrderMenu->model()->index(loc, 1)).toString();
    price = ui->tableView_OrderMenu->model()->data(ui->tableView_OrderMenu->model()->index(loc, 2)).toString();  

    QSqlQuery qry;
    qry.prepare("select Image from menu where ID = '"+id+"';");
    if(qry.exec())
    {
        while(qry.next())
        {
            image = qry.value(0).toString();
        }

        QPixmap pic(image);
        ui->label_OrderMenuImage->setPixmap(pic.scaled(120, 180, Qt::KeepAspectRatioByExpanding));
        ui->label_OrderMenuImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        int loc = ui->tableView_OrderMenu->currentIndex().row();

        QString id, foodName, description, price, categ;
        id = ui->tableView_OrderMenu->model()->data(ui->tableView_OrderMenu->model()->index(loc, 0)).toString();
        foodName = ui->tableView_OrderMenu->model()->data(ui->tableView_OrderMenu->model()->index(loc, 1)).toString();
        description = ui->tableView_OrderMenu->model()->data(ui->tableView_OrderMenu->model()->index(loc, 2)).toString();
        price = ui->tableView_OrderMenu->model()->data(ui->tableView_OrderMenu->model()->index(loc, 3)).toString();
        categ = ui->tableView_OrderMenu->model()->data(ui->tableView_OrderMenu->model()->index(loc, 4)).toString();

        QString orderType, orderID, date, time, customer, qty, paymentStatus, total;
        QString newTotal;

        orderType = ui->comboBox_OrderType->currentText();
        orderID = ui->lineEdit_OrderID->text();
        date = ui->label_HomePageDate->text();
        customer = ui->lineEdit_OrderCustomerName->text();
        paymentStatus = ui->comboBox_OrderPaymentStatus->currentText();


        QString formOrderType, formOrderID, formOrderDate, formOrderCustomer, formOrderEmployee;
        formOrderType = ui->comboBox_OrderType->currentText();
        formOrderID = ui->lineEdit_OrderID->text();
        formOrderDate = ui->lineEdit_OrderDate->text();
        formOrderCustomer = ui->lineEdit_OrderCustomerName->text();
        formOrderEmployee = ui->lineEdit_OrderEmployeeName->text();

        if(formOrderType == "" || formOrderID == "" || formOrderDate == "" || formOrderCustomer == "" || formOrderEmployee == "")
        {
            QMessageBox::information(this, tr("Incomplete Form"), tr("Please fill in the red marks!!!"));

            if(formOrderType == "")
            {
                ui->label_OrderType->setStyleSheet("QLabel {font-weight: bold; color: red;}");
            }
            if(formOrderID == "")
            {
                ui->label_OrderID->setStyleSheet("QLabel {font-weight: bold; color: red;}");
            }
            if(formOrderDate == "")
            {
                ui->label_OrderDate->setStyleSheet("QLabel {font-weight: bold; color: red;}");
            }
            if(formOrderCustomer == "")
            {
                ui->label_OrderCustomer->setStyleSheet("QLabel {font-weight: bold; color: red;}");
            }
            if(formOrderEmployee == "")
            {
                ui->label_OrderEmployee->setStyleSheet("QLabel {font-weight: bold; color: red;}");
            }
        }
        else
        {
            if(orderType == "Dine In")
            {
                ui->label_OrderType->setStyleSheet("QLabel {color: black;}");
                ui->label_OrderID->setStyleSheet("QLabel {color: black;}");
                ui->label_OrderDate->setStyleSheet("QLabel {color: black;}");
                ui->label_OrderCustomer->setStyleSheet("QLabel {color: black;}");
                ui->label_OrderEmployee->setStyleSheet("QLabel {color: black;}");

                QSqlQuery qry;
                qry.prepare("select `Food Name`, description, price from `paboritocafe_v1`.`orderlist` where `Food Name` = '"+foodName+"' and Description = '"+description+"' and Price = '"+price+"' and `Order Type` = 'Dine In'; ");
                if(qry.exec())
                {
                    QString gName, gDesc, gPrice, gQty;

                    while(qry.next())
                    {
                        gName = qry.value(0).toString();
                        gDesc = qry.value(1).toString();
                        gPrice = qry.value(2).toString();
                    }

                    QString gAll = gName + " " + gDesc + " " + gPrice;

                    qDebug() << gAll;

                    if(gAll != "  ")
                    {
                        qDebug() << "Update Qty Status";

                        QSqlQuery qry;
                        qry.prepare("UPDATE `paboritocafe_v1`.`orderlist` SET `Qty`= Qty + 1 WHERE `Food Name` = '"+foodName+"' and Description = '"+description+"' and Price = '"+price+"' and `Order Type` = 'Dine In';  ");
                        if(qry.exec())
                        {
                            //QMessageBox::information(this, tr("Add order to Order List"), tr("Item updated to order list."));

                            QSqlQuery qry;
                            qry.prepare("select Qty from `paboritocafe_v1`.`orderlist` where `Food Name` = '"+foodName+"' and Description = '"+description+"' and Price = '"+price+"' and `Order Type` = 'Dine In';");
                            if(qry.exec())
                            {
                                while(qry.next())
                                {
                                    gQty = qry.value(0).toString();
                                }

                                newTotal = QString::number(gPrice.toDouble() * gQty.toDouble());

                                QSqlQuery qry;
                                qry.prepare("UPDATE `paboritocafe_v1`.`orderlist` SET Total = '"+newTotal+"' WHERE `Food Name` = '"+foodName+"' and Description = '"+description+"' and Price = '"+price+"' and `Order Type` = 'Dine In'; ");
                                if(qry.exec())
                                {
                                   refreshOrderListStatus();
                                }
                            }

                        }
                        else
                        {
                            QMessageBox::critical(this, tr(""), qry.lastError().text());
                        }
                    }
                    else if(gAll == "  ")
                    {
                        newTotal = QString::number(price.toDouble() * 1);

                        qDebug() << "Insert New Item";
                        QSqlQuery qry;
                        qry.prepare("INSERT INTO `paboritocafe_v1`.`orderlist` (`Order Type`, `Order ID`, `Date`, `Time`, `Customer`, `Food Name`, `Description`, `Price`, `Category`, `Qty`, `Total`, `Payment Status`, `Employee`, `Status`) VALUES ('"+orderType+"', '"+orderID+"', '"+date+"', '"+time+"', '"+customer+"', '"+foodName+"', '"+description+"', '"+price+"', '"+categ+"', '1', '"+newTotal+"', '', '"+formOrderEmployee+"', 'Pending'); ");
                        if(qry.exec())
                        {
                            refreshOrderListStatus();
                        }
                        else
                        {
                            QMessageBox::critical(this, tr(""), qry.lastError().text());
                        }
                    }
                }
            }
            else if(orderType == "Take Out")
            {
                ui->label_OrderType->setStyleSheet("QLabel {color: black;}");
                ui->label_OrderID->setStyleSheet("QLabel {color: black;}");
                ui->label_OrderDate->setStyleSheet("QLabel {color: black;}");
                ui->label_OrderCustomer->setStyleSheet("QLabel {color: black;}");
                ui->label_OrderEmployee->setStyleSheet("QLabel {color: black;}");

                QSqlQuery qry;
                qry.prepare("select `Food Name`, description, price from `paboritocafe_v1`.`orderlist` where `Food Name` = '"+foodName+"' and Description = '"+description+"' and Price = '"+price+"' and `Order Type` = 'Take Out'; ");
                if(qry.exec())
                {
                    QString gName, gDesc, gPrice, gQty;

                    while(qry.next())
                    {
                        gName = qry.value(0).toString();
                        gDesc = qry.value(1).toString();
                        gPrice = qry.value(2).toString();
                    }

                    QString gAll = gName + " " + gDesc + " " + gPrice;

                    qDebug() << gAll;

                    if(gAll != "  ")
                    {
                        qDebug() << "Update Qty Status";
                        QSqlQuery qry;
                        qry.prepare("UPDATE `paboritocafe_v1`.`orderlist` SET `Qty`= Qty + 1 WHERE `Food Name` = '"+foodName+"' and Description = '"+description+"' and Price = '"+price+"' and `Order Type` = 'Take Out';  ");
                        if(qry.exec())
                        {
                            //QMessageBox::information(this, tr("Add order to Order List"), tr("Item updated to order list."));

                            QSqlQuery qry;
                            qry.prepare("select Qty from `paboritocafe_v1`.`orderlist` where `Food Name` = '"+foodName+"' and Description = '"+description+"' and Price = '"+price+"' and `Order Type` = 'Take Out';");
                            if(qry.exec())
                            {
                                while(qry.next())
                                {
                                    gQty = qry.value(0).toString();
                                }

                                newTotal = QString::number(gPrice.toDouble() * gQty.toDouble());

                                QSqlQuery qry;
                                qry.prepare("UPDATE `paboritocafe_v1`.`orderlist` SET Total = '"+newTotal+"' WHERE `Food Name` = '"+foodName+"' and Description = '"+description+"' and Price = '"+price+"' and `Order Type` = 'Take Out'; ");
                                if(qry.exec())
                                {
                                   refreshOrderListStatus();
                                }
                            }

                        }
                        else
                        {
                            QMessageBox::critical(this, tr(""), qry.lastError().text());
                        }
                    }
                    else if(gAll == "  ")
                    {
                        newTotal = QString::number(price.toDouble() * 1);

                        qDebug() << "Insert New Item";
                        QSqlQuery qry;
                        qry.prepare("INSERT INTO `paboritocafe_v1`.`orderlist` (`Order Type`, `Order ID`, `Date`, `Time`, `Customer`, `Food Name`, `Description`, `Price`, `Category`, `Qty`, `Total`, `Payment Status`, `Employee`, `Status`) VALUES ('"+orderType+"', '"+orderID+"', '"+date+"', '"+time+"', '"+customer+"', '"+foodName+"', '"+description+"', '"+price+"', '"+categ+"', '1', '"+newTotal+"', '', '"+formOrderEmployee+"', 'Pending'); ");
                        if(qry.exec())
                        {
                             refreshOrderListStatus();
                        }
                        else
                        {
                            QMessageBox::critical(this, tr(""), qry.lastError().text());
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::on_tableView_OrderMenu_doubleClicked(const QModelIndex &index)
{
    //Add Order to Order List

}

void MainWindow::on_tableView_OrderList_clicked(const QModelIndex &index)
{
    //Delete Order from Order List Dine In
    int loc = ui->tableView_OrderList->currentIndex().row();

    QString id, foodName, description, price;
    id = ui->tableView_OrderList->model()->data(ui->tableView_OrderList->model()->index(loc, 0)).toString();
    foodName = ui->tableView_OrderList->model()->data(ui->tableView_OrderList->model()->index(loc, 6)).toString();
    description = ui->tableView_OrderList->model()->data(ui->tableView_OrderList->model()->index(loc, 7)).toString();
    price = ui->tableView_OrderList->model()->data(ui->tableView_OrderList->model()->index(loc, 8)).toString();

    QString gQty;

    QSqlQuery qry;
    qry.prepare("UPDATE `paboritocafe_v1`.`orderlist` SET `Qty`= Qty - 1 WHERE `ID`= '"+id+"'; ");
    if(qry.exec())
    {
        qDebug() << "Update item from Order List!!!";


        QSqlQuery qry;
        qry.prepare("select Qty from `paboritocafe_v1`.`orderlist` where `Food Name` = '"+foodName+"' and Description = '"+ description+"' and Price = '"+price+"' and `Order Type` = 'Dine In'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                gQty = qry.value(0).toString();

            }

            qDebug() << "Qty: " + gQty;

            QString newTotal = QString::number(price.toDouble() * gQty.toDouble());
            qDebug() << "Total: " + newTotal;

            QSqlQuery qry;
            qry.prepare("UPDATE `paboritocafe_v1`.`orderlist` SET `Total`= '"+newTotal+"' where `Food Name` = '"+foodName+"' and Description = '"+ description+"' and Price = '"+price+"' and `Order Type` = 'Dine In'; ");
            if(qry.exec())
            {
                QSqlQuery qry;
                qry.prepare("DELETE from `paboritocafe_v1`.`orderlist` where Qty = '0';");
                if(qry.exec())
                {
                    refreshOrderListStatus();
                }
                else
                {
                    QMessageBox::critical(this, tr("Error::Delete Item = 0"), qry.lastError().text());
                }
            }
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error::Updating Item from Order List"), qry.lastError().text());
    }
}

void MainWindow::on_pushButton_OrderSubmit_clicked()
{    
    QString orderType = ui->comboBox_OrderType->currentText();
    QString orderID = ui->lineEdit_OrderID->text();
    QString customer = ui->lineEdit_OrderCustomerName->text();
    QString date = ui->label_HomePageDate->text();
    QString emp = ui->lineEdit_OrderEmployeeName->text();
    QString sales = ui->lineEdit_OrderTotal->text();// Total
    QString disc = ui->comboBox_OrderDiscount->currentText();
    QString vat = ui->comboBox_OrderVAT->currentText();
    QString paymentStatus = ui->comboBox_OrderPaymentStatus->currentText();
    QString total = ui->doubleSpinBox_GrandTotal->text();

    QTime showTime = QTime::currentTime();
    time = locale().toString(showTime, "h:mm a");

    if(oldTime != "")
    {
        time = oldTime;
    }

    int row = ui->tableView_OrderList->model()->rowCount();

    if(paymentStatus == "" || (row < 1))
    {
        if(paymentStatus == "")
        {
            QMessageBox::information(this, tr("Payment Status"), tr("Select Payment Status"));
        }
        if(row < 1)
        {
            QMessageBox::information(this, tr("Orders"), tr("Can't submit without orders."));
        }
    }
    else if(paymentStatus != "" && (row > 0))
    {
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::question(this, "Submit Order to Kitchen", "Are you sure to submit order to the kitchen???", QMessageBox::Yes|QMessageBox::No);
        if(confirm == QMessageBox::Yes)
        {    QSqlQueryModel *model = new QSqlQueryModel;

            QSqlQuery *qry = new QSqlQuery;
            qry->prepare("select distinct `Order Type` from orderlist");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_OrderList->setModel(model);

                int row = ui->tableView_OrderList->model()->rowCount();

                if(row > 1)
                {
                    orderType = "Dine In/Take Out";
                }

                QSqlQuery qry;
                qry.prepare("UPDATE `paboritocafe_v1`.`orderlist` SET `Time`= '"+time+"' WHERE `Order ID`= '"+orderID+"' ;");
                if(qry.exec())
                {
                    qDebug() << "Time Updated for: " + orderID;

                    QSqlQuery qry;
                    qry.prepare("select ID, `Payment Status` from `paboritocafe_v1`.`orderlist`;");
                    if(qry.exec())
                    {
                        QString id, payment;

                        while(qry.next())
                        {
                            id = qry.value(0).toString();
                            payment = qry.value(1).toString();

                            qDebug() << "ID: " + id;
                            qDebug() << "{}: " + paymentStatus;

                            if(payment == "")
                            {
                                QSqlQuery qry;
                                qry.prepare("UPDATE `paboritocafe_v1`.`orderlist` SET `Payment Status` = '"+paymentStatus+"' WHERE `ID` = '"+id+"'; ");
                                if(qry.exec())
                                {
                                    qDebug() << "Update Payment Status";
                                }
                                else
                                {
                                    QMessageBox::critical(this, tr("Error::Update Payment Status"), qry.lastError().text());
                                }
                            }
                        }

                        QSqlQuery qry;
                        qry.prepare("INSERT INTO `paboritocafe_v1`.`orderstatus` (`Order Type`, `Order ID`, Date, Time, Customer, `Food Name`, Description, Price, Category, Qty, Total, `Payment Status`, Employee, Status) select `Order Type`, `Order ID`, Date, Time, Customer, `Food Name`, Description, Price, Category, Qty, Total, `Payment Status`, Employee, Status from orderlist where `Order ID` = '"+orderID+"'; ");
                        if(qry.exec())
                        {
                            QSqlQuery qry;
                            qry.prepare("INSERT INTO `paboritocafe_v1`.`orderstatus` (`Order ID`) VALUES ('"+orderID+"');");
                            if(qry.exec())
                            {
                                QSqlQuery qry;
                                qry.prepare("INSERT into reports (`Order Type`, `Order ID`, `Customer`, `Date`, `Time`, `Employee`, `Food Name`, `Description`, `Price`, `Qty`, `Category`, `Total`) select `Order Type`, `Order ID`, `Customer`, `Date`, `Time`, `Employee`, `Food Name`, `Description`, `Price`, `Qty`, `Category`, `Total` from orderstatus where `Order ID` = '"+orderID+"'; ");
                                if(qry.exec())
                                {
                                    QSqlQuery qry;
                                    qry.prepare("INSERT into reports_summary (`Order Type`, `Order ID`, Customer, Date, Time, Employee, Sales, Discount, Vat, Total, `Payment Status`) values ('"+orderType+"', '"+orderID+"', '"+customer+"', '"+date+"', '"+time+"', '"+emp+"', '"+sales+"', '"+disc+"', '"+vat+"', '"+total+"', '"+paymentStatus+"');");
                                    if(qry.exec())
                                    {
                                        QMessageBox::information(this, tr("Submit Order"), tr("Order Submitted"));
                                        oldTime = "";
                                        on_pushButton_OrderPage_clicked();
                                    }
                                    else
                                    {
                                        QMessageBox::critical(this, tr("Error::INSERT into REPORTS SUMMARY"), qry.lastError().text());
                                    }
                                }
                                else
                                {
                                    QMessageBox::critical(this, tr("Error::INSERT into REPORTS"), qry.lastError().text());
                                }
                            }
                            else
                            {
                                QMessageBox::critical(this, tr("Error::INSERT ORDER ID"), qry.lastError().text());
                            }
                        }
                        else
                        {
                            QMessageBox::critical(this, tr("Error::Transfer Orderlist to OrderStatus"), qry.lastError().text());
                        }
                    }
                    else
                    {

                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("Error::Updating Time for Order ID"), qry.lastError().text());
                }
            }
        }
    }
}

void MainWindow::on_pushButton_OrderClear_clicked()
{
    //Clear Order Form

    qDebug() << "\nClear Order";

    QSqlQuery qry;
    qry.prepare("select count(id) from `paboritocafe_v1`.`orderstatus`;");
    if(qry.exec())
    {
        int count;
        while(qry.next())
        {
            count = qry.value(0).toInt();
        }

        qDebug() << count;

        if(count > 0)
        {
            qDebug() << "This 1";

            QSqlQuery qry;
            qry.prepare("truncate `paboritocafe_v1`.`orderlist`;");
            if(qry.exec())
            {
                QSqlQuery qry;
                qry.prepare("select replace(max(distinct `Order ID`), 'S', '') from `paboritocafe_v1`.`orderstatus`;");
                if(qry.exec())
                {
                    QString orderID;
                    while(qry.next())
                    {
                        orderID = qry.value(0).toString();
                    }

                    std::ostringstream ss;
                    ss << std::setw(8) << std::setfill('0') << orderID.toInt() + 1;
                    std::string s = ss.str();

                    QString num;
                    num = QString::fromStdString(s);
                    ui->lineEdit_OrderID->setText("S"+num);

                    qDebug() << ui->lineEdit_OrderID->text();

                    ui->lineEdit_OrderDate->setText(date);
                }
                else
                {
                    QMessageBox::critical(this, tr("Error::Order ID 1"), qry.lastError().text());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("This 1"), qry.lastError().text());
            }
        }
        else if(count == 0)
        {
            qDebug() << "This 2";
            QSqlQuery qry;
            qry.prepare("truncate `paboritocafe_v1`.`orderlist`;");
            if(qry.exec())
            {
                QSqlQuery qry;
                qry.prepare("select replace(max(distinct `Order ID`), 'S', '') from `paboritocafe_v1`.`reports`;");
                if(qry.exec())
                {
                    QString orderID;
                    while(qry.next())
                    {
                        orderID = qry.value(0).toString();
                    }

                    std::ostringstream ss;
                    ss << std::setw(8) << std::setfill('0') << orderID.toInt() + 1;
                    std::string s = ss.str();

                    QString num;
                    num = QString::fromStdString(s);
                    ui->lineEdit_OrderID->setText("S"+num);

                    qDebug() << ui->lineEdit_OrderID->text();

                    ui->lineEdit_OrderDate->setText(date);
                }
                else
                {
                    QMessageBox::critical(this, tr("Error::Order ID 2"), qry.lastError().text());
                }
            }
            else
            {
                QMessageBox::critical(this, tr("This 2"), qry.lastError().text());
            }
        }
    }

    ui->lineEdit_OrderEmployeeName->setText(ui->label_HomePageEmployeeName->text());
    ui->comboBox_OrderType->clearEditText();
    ui->comboBox_OrderPaymentStatus->clearEditText();
    ui->lineEdit_OrderCustomerName->clear();
    ui->comboBox_OrderType->currentText();
    ui->comboBox_OrderPaymentStatus->currentText();
}

void MainWindow::on_tableView_OrderStatus_doubleClicked(const QModelIndex &index)
{
    //Dine In Order Status
    int loc = ui->tableView_OrderStatus->currentIndex().row();

    QString orderType, orderID, time, customerName;
    orderType = ui->tableView_OrderStatus->model()->data(ui->tableView_OrderStatus->model()->index(loc, 1)).toString();
    orderID = ui->tableView_OrderStatus->model()->data(ui->tableView_OrderStatus->model()->index(loc, 2)).toString();
    time = ui->tableView_OrderStatus->model()->data(ui->tableView_OrderStatus->model()->index(loc, 4)).toString();
    customerName = ui->tableView_OrderStatus->model()->data(ui->tableView_OrderStatus->model()->index(loc, 5)).toString();

    ui->lineEdit_OrderCustomerName->setText(customerName);

    qDebug() << orderType;
    qDebug() << orderID;

    confirmOrderStatus *confirm = new confirmOrderStatus();
    confirm->show();

    connect(this, SIGNAL(sendEditItem(QStringList)), confirm, SLOT(receiveOrderStatus(QStringList)));

    QStringList orderStatus;
    orderStatus.append(orderType);
    orderStatus.append(orderID);
    orderStatus.append(time);
    orderStatus.append(customerName);

    emit sendEditItem(orderStatus);

    confirm->exec();

    ui->lineEdit_OrderID->setText(orderID);
    ui->lineEdit_OrderCustomerName->setText(customerName);

    refreshOrderListStatus();
}

void MainWindow::on_pushButton_AddAcount_clicked()
{
    createAccount c;
    c.setModal(true);
    c.exec();

    on_pushButton_ManageAccounts_clicked();
}

void MainWindow::on_pushButton_DelAccount_clicked()
{
    int loc = ui->tableView_ManageAccounts->currentIndex().row();

    QString id, name, first, middle, last;
    id = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 0)).toString();
    first = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 1)).toString();
    middle = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 2)).toString();
    last = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 3)).toString();

    name = first + " " + middle + " " + last;

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete account of:\n\n" + name, QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from `paboritocafe_v1`.`accounts` where ID = '"+id+"'; ");
        if(qry.exec())
        {
            QMessageBox::information(this, tr("Delete Account"), tr("Account Deleted . . ."));
            on_pushButton_ManageAccounts_clicked();
        }
        else
        {
            QMessageBox::critical(this, tr("Error::Delete Account"), qry.lastError().text());
        }
    }
}

void MainWindow::on_tableView_ManageAccounts_doubleClicked(const QModelIndex &index)
{
    int loc = ui->tableView_ManageAccounts->currentIndex().row();

    QString id, first, middle, last, username, password, type;
    id = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 0)).toString();
    first = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 1)).toString();
    middle = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 2)).toString();
    last = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 3)).toString();
    username = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 4)).toString();
    password = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 5)).toString();
    type = ui->tableView_ManageAccounts->model()->data(ui->tableView_ManageAccounts->model()->index(loc, 6)).toString();

    EditAccount *edit = new EditAccount();
    edit->show();

    connect(this, SIGNAL(sendEditAccount(QStringList)), edit, SLOT(receiveEditAccount(QStringList)));

    QStringList account;
    account.append(id);
    account.append(first);
    account.append(middle);
    account.append(last);
    account.append(username);
    account.append(password);
    account.append(type);

    emit sendEditAccount(account);

    edit->exec();

    on_pushButton_ManageAccounts_clicked();
}

void MainWindow::on_comboBox_ReportsSortByDate_currentTextChanged(const QString &arg1)
{
    //ComboBox Date

    QString date, cust, emp;
    date = ui->comboBox_ReportsSortByDate->currentText();
    cust = ui->comboBox_ReportsSortByCustomer->currentText();
    emp = ui->comboBox_ReportsSortByEmp->currentText();

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;

    if(ui->groupBox_ReportsSortBy->isChecked())
    {
        if(ui->checkBox_Reports_EnableDate->isChecked() && !ui->checkBox_Reports_EnableCust->isChecked() && !ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Not Checked
            //Not Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Date = '"+date+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && !ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Not Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Date = '"+date+"' and Customer = '"+cust+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Date = '"+date+"' and Customer = '"+cust+"' and Employee = '"+emp+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && !ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Not Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Date = '"+date+"' and Employee = '"+emp+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
    }

    loadReportsSalesDiscTotal();
}

void MainWindow::on_comboBox_ReportsSortByCustomer_currentTextChanged(const QString &arg1)
{
    //ComboBox Customer

    QString date, cust, emp;
    date = ui->comboBox_ReportsSortByDate->currentText();
    cust = ui->comboBox_ReportsSortByCustomer->currentText();
    emp = ui->comboBox_ReportsSortByEmp->currentText();

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;

    if(ui->groupBox_ReportsSortBy->isChecked())
    {
        if(!ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && !ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Not Checked
            //Checked
            //Not Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Customer = '"+cust+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && !ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Not Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Customer = '"+cust+"' and Date = '"+date+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Customer = '"+cust+"' and Date = '"+date+"' and Employee = '"+emp+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(!ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Not Checked
            //Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Date = '"+date+"' and Employee = '"+emp+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
    }

    loadReportsSalesDiscTotal();
}

void MainWindow::on_comboBox_ReportsSortByEmp_currentTextChanged(const QString &arg1)
{
    //ComboBox Employee

    QString date, cust, emp;
    date = ui->comboBox_ReportsSortByDate->currentText();
    cust = ui->comboBox_ReportsSortByCustomer->currentText();
    emp = ui->comboBox_ReportsSortByEmp->currentText();

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;

    if(ui->groupBox_ReportsSortBy->isChecked())
    {
        if(!ui->checkBox_Reports_EnableDate->isChecked() && !ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Not Checked
            //Not Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Employee = '"+emp+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(!ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Not Checked
            //Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Employee = '"+emp+"' and Customer = '"+cust+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Employee = '"+emp+"' and Customer = '"+cust+"' and Date = '"+date+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && !ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Not Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Employee = '"+emp+"' and Date = '"+date+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
    }

    loadReportsSalesDiscTotal();
}

void MainWindow::on_groupBox_ReportsSortBy_clicked(bool checked)
{
    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;

    if(!checked)
    {
        qry->prepare("select *from `paboritocafe_v1`.`reports_summary`; ");
        if(qry->exec())
        {
            model->setQuery(*qry);
            ui->tableView_ReportsSummary->setModel(model);

            ui->tableView_ReportsSummary->setColumnHidden(0, true);

            ui->tableView_ReportsSummary->setVisible(false);
            ui->tableView_ReportsSummary->resizeColumnsToContents();
            ui->tableView_ReportsSummary->resizeRowsToContents();
            ui->tableView_ReportsSummary->setVisible(true);
        }
    }
}


void MainWindow::on_checkBox_Reports_EnableDate_clicked(bool checked)
{
    QString date, cust, emp;
    date = ui->comboBox_ReportsSortByDate->currentText();
    cust = ui->comboBox_ReportsSortByCustomer->currentText();
    emp = ui->comboBox_ReportsSortByEmp->currentText();

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;

    //Enable Date
    if(checked)
    {
        QSqlQueryModel *model1 = new QSqlQueryModel;

        QSqlQuery *qry1 = new QSqlQuery;
        qry1->prepare("select distinct Date from `paboritocafe_v1`.`reports_summary`; ");
        if(qry1->exec())
        {
            model1->setQuery(*qry1);
            ui->comboBox_ReportsSortByDate->setModel(model1);

            ui->comboBox_ReportsSortByDate->clearEditText();
        }
        else
        {
            QMessageBox::critical(this, tr("Reports::Error::Enable Date"), qry1->lastError().text());
        }

        ui->comboBox_ReportsSortByDate->setEnabled(true);

        if(ui->checkBox_Reports_EnableDate->isChecked() && !ui->checkBox_Reports_EnableCust->isChecked() && !ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Not Checked
            //Not Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Date = '"+date+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && !ui->checkBox_Reports_EnableCust->isChecked() && !ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Not Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Date = '"+date+"' and Customer = '"+cust+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Date = '"+date+"' and Customer = '"+cust+"' and Employee = '"+emp+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
    }
    else
    {
        ui->comboBox_ReportsSortByDate->setDisabled(true);
    }

    loadReportsSalesDiscTotal();
}

void MainWindow::on_checkBox_Reports_EnableCust_clicked(bool checked)
{
    QString date, cust, emp;
    date = ui->comboBox_ReportsSortByDate->currentText();
    cust = ui->comboBox_ReportsSortByCustomer->currentText();
    emp = ui->comboBox_ReportsSortByEmp->currentText();

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;

    //Enable Customer
    if(checked)
    {
        QSqlQueryModel *model1 = new QSqlQueryModel;

        QSqlQuery *qry1 = new QSqlQuery;
        qry1->prepare("select distinct Customer from `paboritocafe_v1`.`reports_summary`; ");
        if(qry1->exec())
        {
            model1->setQuery(*qry1);
            ui->comboBox_ReportsSortByCustomer->setModel(model1);

            ui->comboBox_ReportsSortByCustomer->clearEditText();
        }
        else
        {
            QMessageBox::critical(this, tr("Reports::Error::Enable Customer"), qry1->lastError().text());
        }

        ui->comboBox_ReportsSortByCustomer->setEnabled(true);

        if(!ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && !ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Not Checked
            //Checked
            //Not Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Customer = '"+cust+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(!ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && !ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Not Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Customer = '"+cust+"' and Date = '"+date+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Customer = '"+cust+"' and Date = '"+date+"' and Employee = '"+emp+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
    }
    else
    {
        ui->comboBox_ReportsSortByCustomer->setDisabled(true);
    }

    loadReportsSalesDiscTotal();
}

void MainWindow::on_checkBox_Reports_EnableEmp_clicked(bool checked)
{
    QString date, cust, emp;
    date = ui->comboBox_ReportsSortByDate->currentText();
    cust = ui->comboBox_ReportsSortByCustomer->currentText();
    emp = ui->comboBox_ReportsSortByEmp->currentText();

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;

    //Enable Emp
    if(checked)
    {
        QSqlQueryModel *model1 = new QSqlQueryModel;

        QSqlQuery *qry1 = new QSqlQuery;
        qry1->prepare("select distinct Employee from `paboritocafe_v1`.`reports_summary`; ");
        if(qry1->exec())
        {
            model1->setQuery(*qry1);
            ui->comboBox_ReportsSortByEmp->setModel(model1);

            ui->comboBox_ReportsSortByEmp->clearEditText();
        }
        else
        {
            QMessageBox::critical(this, tr("Reports::Error::Enable Employee"), qry1->lastError().text());
        }

        ui->comboBox_ReportsSortByEmp->setEnabled(true);

        if(!ui->checkBox_Reports_EnableDate->isChecked() && !ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Not Checked
            //Not Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Employee = '"+emp+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(!ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Not Checked
            //Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Employee = '"+emp+"' and Customer = '"+cust+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
        else if(ui->checkBox_Reports_EnableDate->isChecked() && ui->checkBox_Reports_EnableCust->isChecked() && ui->checkBox_Reports_EnableEmp->isChecked())
        {
            //Checked
            //Checked
            //Checked

            qry->prepare("select *from `paboritocafe_v1`.`reports_summary` where Employee = '"+emp+"' and Customer = '"+cust+"' and Date = '"+date+"'; ");
            if(qry->exec())
            {
                model->setQuery(*qry);
                ui->tableView_ReportsSummary->setModel(model);

                ui->tableView_ReportsSummary->setColumnHidden(0, true);

                ui->tableView_ReportsSummary->setVisible(false);
                ui->tableView_ReportsSummary->resizeColumnsToContents();
                ui->tableView_ReportsSummary->resizeRowsToContents();
                ui->tableView_ReportsSummary->setVisible(true);
            }
        }
    }
    else
    {
        ui->comboBox_ReportsSortByEmp->setDisabled(true);
    }

    loadReportsSalesDiscTotal();
}

void MainWindow::on_pushButton_OrderAddVat_clicked()
{
    addVat vat;
    vat.setModal(true);
    vat.exec();

    loadVat();
}

void MainWindow::on_pushButton_AddOrderDiscount_clicked()
{
    addDiscount dis;
    dis.setModal(true);
    dis.exec();

    loadDiscount();
}

void MainWindow::on_comboBox_OrderDiscount_currentTextChanged(const QString &arg1)
{
    QString orderID = ui->lineEdit_OrderID->text();
    QString textTotal = "";

    QSqlQuery qry2;
    qry2.prepare("select sum(Total) from `paboritocafe_v1`.`orderlist` where `Order ID` = '"+orderID+"'; ");
    if(qry2.exec())
    {
        QString total;
        while(qry2.next())
        {
            total = qry2.value(0).toString();

            ui->lineEdit_OrderTotal->setText(textTotal + total);

            double vat, discount, total, grandTotal;
            vat = ui->comboBox_OrderVAT->currentText().toDouble() / 100;
            discount = ui->comboBox_OrderDiscount->currentText().toDouble() / 100;
            total = ui->lineEdit_OrderTotal->text().toDouble();

            double finalVat, finalDiscount;
            finalVat = ((1 + vat) * total);

            //calculate vat
            ui->doubleSpinBox_GrandTotal->setValue(finalVat);

            finalDiscount = finalVat - ((discount) * finalVat);

            //calculate discount
            ui->doubleSpinBox_GrandTotal->setValue(finalDiscount);
        }
    }
}

void MainWindow::on_comboBox_OrderVAT_currentTextChanged(const QString &arg1)
{
    QString orderID = ui->lineEdit_OrderID->text();
    QString textTotal = "";

    QSqlQuery qry2;
    qry2.prepare("select sum(Total) from `paboritocafe_v1`.`orderlist` where `Order ID` = '"+orderID+"'; ");
    if(qry2.exec())
    {
        QString total;
        while(qry2.next())
        {
            total = qry2.value(0).toString();

            ui->lineEdit_OrderTotal->setText(textTotal + total);

            double vat, discount, total, grandTotal;
            vat = ui->comboBox_OrderVAT->currentText().toDouble() / 100;
            discount = ui->comboBox_OrderDiscount->currentText().toDouble() / 100;
            total = ui->lineEdit_OrderTotal->text().toDouble();

            double finalVat, finalDiscount;
            finalVat = ((1 + vat) * total);

            //calculate vat
            ui->doubleSpinBox_GrandTotal->setValue(finalVat);

            finalDiscount = finalVat - ((discount) * finalVat);

            //calculate discount
            ui->doubleSpinBox_GrandTotal->setValue(finalDiscount);
        }
    }
}

void MainWindow::on_tabWidget_OrderPage_currentChanged(int index)
{
    int row = ui->tableView_OrderList->model()->rowCount();

    if(index == 0)
    {
        if(row == 0)
        {
            on_pushButton_OrderClear_clicked();
        }
        else
        {

        }
    }
}

void MainWindow::printMonthlyReports(QPrinter *printer)
{
    on_pushButton_ReportsPage_clicked();

    QString currentDate;
    currentDate = ui->label_HomePageDate->text();

    QString netSales;
    netSales = ui->doubleSpinBox_ReportsTotal->text();

    QTextStream out(&strStream);

    const int rowCount = ui->tableView_ReportsSummary->model()->rowCount();
    //const int columnCount = ui->tableView_ReportsSummary->model()->columnCount();

    out <<  "<html>\n"
        "<head>\n"
        "<meta Content=\"Text/html; charset=Windows-1251\">\n"
        <<  QString("<title>%1</title>\n").arg("")
        <<  "</head>\n"
        "<body bgcolor=#ffffff link=#5000A0>\n";

     out << "<h1> </h1><table border=0 cellspacing=0 cellpadding=0 width=700><tr>"
                   "<td><b>Reports Summary</b></td>"
//                   "<td><b>Threshold Value</b></td>"
                     "</tr>"
                     "<tr>"
                   "<td> Date : " << currentDate << "</td>"
                    "</tr>"
                    "<tr>"
                   "<td> Sales Net : " << netSales << "</td>"
//                   "</tr>"
//                   "<tr>"
//                   "<td> Highest water level : "
//                //<< highestLevel
//                << "m</td>"
//                   "<td> Safe Level : "
//                //<< str[2]
//                << "m</td>"
//                   "</tr>"
//                   "<tr>"
//                   "<td> Lowest water level : "
//               // << lowestLevel
//                << "m</td>"
//                   "<td> Low aspectRatio Level : "
//                //<< str[3]
//                << "m</td>"
//                   "</tr>"
//                   "<tr>"
//                   "<td></td>"
//                   "<td> High aspectRatio Level : "
//                //<< str[4]
//                << "m</td>"
//                   "</tr>"
//                   "<tr>"
//                   "<td><b>Sampling Period:  </b>"
//                //<< str[0]
//                << " seconds</td>"
//                   "</tr>"
//                   "</table><h1> </h1>"

        //taman dri
        "<table border=1 cellspacing=0 cellpadding=0 width=700>\n";

    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
//    for (int column = 1; column < columnCount; column++)
//        if (!ui->tableView_ReportsSummary->isColumnHidden(column))
           //out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(0, Qt::Horizontal).toString());
            out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(1, Qt::Horizontal).toString());
            out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(2, Qt::Horizontal).toString());
            out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(3, Qt::Horizontal).toString());
             out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(4, Qt::Horizontal).toString());
             out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(5, Qt::Horizontal).toString());
              out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(6, Qt::Horizontal).toString());
               out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(7, Qt::Horizontal).toString());
                out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(8, Qt::Horizontal).toString());
                out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(9, Qt::Horizontal).toString());
                 out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(10, Qt::Horizontal).toString());
                  //out << QString("<th>%1</th>").arg(ui->tableView_ReportsSummary->model()->headerData(11, Qt::Horizontal).toString());

    out << "</tr></thead>\n";

    // data table
            for (int row = 0; row < rowCount; row++)
            {
                out << "<tr>";
//                //COLUMN 0
//                QString id = QString::number(row + 1);
//                out << QString("<td bkcolor=0>%1</td>").arg((!id.isEmpty()) ? id : QString("&nbsp;"));

                //COLUMN 1
                QString orderType = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 1)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!orderType.isEmpty()) ? orderType : QString("&nbsp;"));
                //COLUMN 2
                QString orderID = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 2)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!orderID.isEmpty()) ? orderID : QString("&nbsp;"));
                //COLUMN 3
                QString customer = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 3)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!customer.isEmpty()) ? customer : QString("&nbsp;"));
                //COLUMN 4
                QString date = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 4)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!date.isEmpty()) ? date : QString("&nbsp;"));
                //COLUMN 5
                QString time = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 5)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!time.isEmpty()) ? time : QString("&nbsp;"));
                //COLUMN 5
                QString employee = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 6)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!employee.isEmpty()) ? employee : QString("&nbsp;"));
                //COLUMN 7
                QString sales = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 7)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!sales.isEmpty()) ? sales : QString("&nbsp;"));
                //COLUMN 8
                QString discount = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 8)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!discount.isEmpty()) ? discount : QString("&nbsp;"));
                //COLUMN 9
                QString vat = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 9)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!vat.isEmpty()) ? vat : QString("&nbsp;"));
//                //COLUMN 10
                QString total = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 10)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!total.isEmpty()) ? total : QString("&nbsp;"));

//                QString paymentStatus = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 10)).toString().simplified();
//                out << QString("<td bkcolor=0>%1</td>").arg((!paymentStatus.isEmpty()) ? paymentStatus : QString("&nbsp;"));

//                QString price = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 11)).toString().simplified();
//                out << QString("<td bkcolor=0>%1</td>").arg((!price.isEmpty()) ? price : QString("&nbsp;"));

//                QString price = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 16)).toString().simplified();
//                out << QString("<td bkcolor=0>%1</td>").arg((!price.isEmpty()) ? price : QString("&nbsp;"));

//                QString price2 = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 17)).toString().simplified();
//                out << QString("<td bkcolor=0>%1</td>").arg((!price2.isEmpty()) ? price2 : QString("&nbsp;"));

//                QString price4 = ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(row, 18)).toString().simplified();
//                out << QString("<td bkcolor=0>%1</td>").arg((!price4.isEmpty()) ? price4 : QString("&nbsp;"));


                out << "</tr>\n";
            }

    out <<  "</table>\n "
        "</body>\n"
        "</html>\n";

    document = new QTextDocument();
    document->setHtml(strStream);
    document->print(printer);
    document->clear();
    strStream.clear();
    //delete document; //Error
}

void MainWindow::clearOrders()
{
    ui->label_CustomerName_0->setText("Customer");
    ui->label_CustomerName_1->setText("Customer");
    ui->label_CustomerName_2->setText("Customer");
    ui->label_CustomerName_3->setText("Customer");
    ui->label_CustomerName_4->setText("Customer");
    ui->label_CustomerName_5->setText("Customer");
    ui->label_CustomerName_6->setText("Customer");
    ui->label_CustomerName_7->setText("Customer");
    ui->label_CustomerName_8->setText("Customer");
    ui->label_CustomerName_9->setText("Customer");

    ui->label_Time_0->setText("Time");
    ui->label_Time_1->setText("Time");
    ui->label_Time_2->setText("Time");
    ui->label_Time_3->setText("Time");
    ui->label_Time_4->setText("Time");
    ui->label_Time_5->setText("Time");
    ui->label_Time_6->setText("Time");
    ui->label_Time_7->setText("Time");
    ui->label_Time_8->setText("Time");
    ui->label_Time_9->setText("Time");

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;

    qry->prepare("select `ORDER` from empty;");
    if(qry->exec())
    {
        model->setQuery(*qry);

        ui->tableView_OrderView_0->setModel(model);
        ui->tableView_OrderView_1->setModel(model);
        ui->tableView_OrderView_2->setModel(model);
        ui->tableView_OrderView_3->setModel(model);
        ui->tableView_OrderView_4->setModel(model);
        ui->tableView_OrderView_5->setModel(model);
        ui->tableView_OrderView_6->setModel(model);
        ui->tableView_OrderView_7->setModel(model);
        ui->tableView_OrderView_8->setModel(model);
        ui->tableView_OrderView_9->setModel(model);

        ui->label_CustomerName_0->hide();
        ui->label_Time_0->hide();
        ui->tableView_OrderView_0->hide();
        ui->pushButton_ModifyOrder_0->hide();
        ui->pushButton_CancelOrder_0->hide();
        ui->pushButton_ConfirmOrder_0->hide();

        ui->label_CustomerName_1->hide();
        ui->label_Time_1->hide();
        ui->tableView_OrderView_1->hide();
        ui->pushButton_ModifyOrder_1->hide();
        ui->pushButton_CancelOrder_1->hide();
        ui->pushButton_ConfirmOrder_1->hide();

        ui->label_CustomerName_2->hide();
        ui->label_Time_2->hide();
        ui->tableView_OrderView_2->hide();
        ui->pushButton_ModifyOrder_2->hide();
        ui->pushButton_CancelOrder_2->hide();
        ui->pushButton_ConfirmOrder_2->hide();

        ui->label_CustomerName_3->hide();
        ui->label_Time_3->hide();
        ui->tableView_OrderView_3->hide();
        ui->pushButton_ModifyOrder_3->hide();
        ui->pushButton_CancelOrder_3->hide();
        ui->pushButton_ConfirmOrder_3->hide();

        ui->label_CustomerName_4->hide();
        ui->label_Time_4->hide();
        ui->tableView_OrderView_4->hide();
        ui->pushButton_ModifyOrder_4->hide();
        ui->pushButton_CancelOrder_4->hide();
        ui->pushButton_ConfirmOrder_4->hide();

        ui->label_CustomerName_5->hide();
        ui->label_Time_5->hide();
        ui->tableView_OrderView_5->hide();
        ui->pushButton_ModifyOrder_5->hide();
        ui->pushButton_CancelOrder_5->hide();
        ui->pushButton_ConfirmOrder_5->hide();

        ui->label_CustomerName_6->hide();
        ui->label_Time_6->hide();
        ui->tableView_OrderView_6->hide();
        ui->pushButton_ModifyOrder_6->hide();
        ui->pushButton_CancelOrder_6->hide();
        ui->pushButton_ConfirmOrder_6->hide();

        ui->label_CustomerName_7->hide();
        ui->label_Time_7->hide();
        ui->tableView_OrderView_7->hide();
        ui->pushButton_ModifyOrder_7->hide();
        ui->pushButton_CancelOrder_7->hide();
        ui->pushButton_ConfirmOrder_7->hide();

        ui->label_CustomerName_8->hide();
        ui->label_Time_8->hide();
        ui->tableView_OrderView_8->hide();
        ui->pushButton_ModifyOrder_8->hide();
        ui->pushButton_CancelOrder_8->hide();
        ui->pushButton_ConfirmOrder_8->hide();

        ui->label_CustomerName_9->hide();
        ui->label_Time_9->hide();
        ui->tableView_OrderView_9->hide();
        ui->pushButton_ModifyOrder_9->hide();
        ui->pushButton_CancelOrder_9->hide();
        ui->pushButton_ConfirmOrder_9->hide();
    }
}

void MainWindow::setOrders()
{
    orderStatus_2();
    QSqlQueryModel *modelGet[10];
    QSqlQuery *qryGet[10];

    if(getOrder[0] != "")
    {
        for(int x = 0; x < 10; x++)
        {
            //qDebug() << "Getting Order Details for: " + order[x];

            QSqlQuery qry;
            qry.prepare("select distinct Customer, Time from paboritocafe_v1.orderstatus where `Order ID` = '"+getOrder[x]+"'; ");
            if(qry.exec())
            {
                if(qry.next())
                {
                    //qDebug() << "index: " + QString::number(x);

                    getCustomerName[x] = qry.value(0).toString();
                    getTime[x] = qry.value(1).toString();

                    //qDebug() << getCustomerName[x] + " " + getTime[x];

                    modelGet[x] = new QSqlQueryModel();

                    qryGet[x] = new QSqlQuery();
                    qryGet[x]->prepare("select `Order Type`, `Food Name`, Category, Qty from paboritocafe_v1.orderstatus where `Order ID` = '"+getOrder[x]+"' ORDER BY IF(`Order Type` RLIKE '^[a-z]', 1, 2), `Order Type`; ");
                    if(qryGet[x]->exec())
                    {
                        modelGet[x]->setQuery(*qryGet[x]);
                    }
                }
           }
        }

        //Order 0
        if(getOrder[0] != "")
        {
            ui->label_CustomerName_0->show();
            ui->label_Time_0->show();
            ui->tableView_OrderView_0->show();
            ui->pushButton_ModifyOrder_0->show();
            ui->pushButton_CancelOrder_0->show();
            ui->pushButton_ConfirmOrder_0->show();

            ui->label_CustomerName_0->setText(getCustomerName[0]);
            ui->label_Time_0->setText(getTime[0]);

            ui->tableView_OrderView_0->setModel(modelGet[0]);


            ui->tableView_OrderView_0->setVisible(false);
            ui->tableView_OrderView_0->resizeColumnsToContents();
            ui->tableView_OrderView_0->resizeRowsToContents();
            ui->tableView_OrderView_0->setVisible(true);

            ui->tableView_OrderView_0->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_0->hide();
            ui->label_Time_0->hide();
            ui->tableView_OrderView_0->hide();
            ui->pushButton_ModifyOrder_0->hide();
            ui->pushButton_CancelOrder_0->hide();
            ui->pushButton_ConfirmOrder_0->hide();
        }

        //Order 1
        if(getOrder[1] != "")
        {
            ui->label_CustomerName_1->show();
            ui->label_Time_1->show();
            ui->tableView_OrderView_1->show();
            ui->pushButton_ModifyOrder_1->show();
            ui->pushButton_CancelOrder_1->show();
            ui->pushButton_ConfirmOrder_1->show();

            ui->label_CustomerName_1->setText(getCustomerName[1]);
            ui->label_Time_1->setText(getTime[1]);

            ui->tableView_OrderView_1->setModel(modelGet[1]);


            ui->tableView_OrderView_1->setVisible(false);
            ui->tableView_OrderView_1->resizeColumnsToContents();
            ui->tableView_OrderView_1->resizeRowsToContents();
            ui->tableView_OrderView_1->setVisible(true);

            ui->tableView_OrderView_1->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_1->hide();
            ui->label_Time_1->hide();
            ui->tableView_OrderView_1->hide();
            ui->pushButton_ModifyOrder_1->hide();
            ui->pushButton_CancelOrder_1->hide();
            ui->pushButton_ConfirmOrder_1->hide();
        }

        //Order 2
        if(getOrder[2] != "")
        {
            ui->label_CustomerName_2->show();
            ui->label_Time_2->show();
            ui->tableView_OrderView_2->show();
            ui->pushButton_ModifyOrder_2->show();
            ui->pushButton_CancelOrder_2->show();
            ui->pushButton_ConfirmOrder_2->show();

            ui->label_CustomerName_2->setText(getCustomerName[2]);
            ui->label_Time_2->setText(getTime[2]);

            ui->tableView_OrderView_2->setModel(modelGet[2]);


            ui->tableView_OrderView_2->setVisible(false);
            ui->tableView_OrderView_2->resizeColumnsToContents();
            ui->tableView_OrderView_2->resizeRowsToContents();
            ui->tableView_OrderView_2->setVisible(true);

            ui->tableView_OrderView_2->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_2->hide();
            ui->label_Time_2->hide();
            ui->tableView_OrderView_2->hide();
            ui->pushButton_ModifyOrder_2->hide();
            ui->pushButton_CancelOrder_2->hide();
            ui->pushButton_ConfirmOrder_2->hide();
        }

        //Order 3
        if(getOrder[3] != "")
        {
            ui->label_CustomerName_3->show();
            ui->label_Time_3->show();
            ui->tableView_OrderView_3->show();
            ui->pushButton_ModifyOrder_3->show();
            ui->pushButton_CancelOrder_3->show();
            ui->pushButton_ConfirmOrder_3->show();

            ui->label_CustomerName_3->setText(getCustomerName[3]);
            ui->label_Time_3->setText(getTime[3]);

            ui->tableView_OrderView_3->setModel(modelGet[3]);


            ui->tableView_OrderView_3->setVisible(false);
            ui->tableView_OrderView_3->resizeColumnsToContents();
            ui->tableView_OrderView_3->resizeRowsToContents();
            ui->tableView_OrderView_3->setVisible(true);

            ui->tableView_OrderView_3->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_3->hide();
            ui->label_Time_3->hide();
            ui->tableView_OrderView_3->hide();
            ui->pushButton_ModifyOrder_3->hide();
            ui->pushButton_CancelOrder_3->hide();
            ui->pushButton_ConfirmOrder_3->hide();
        }

        //Order 4
        if(getOrder[4] != "")
        {
            ui->label_CustomerName_4->show();
            ui->label_Time_4->show();
            ui->tableView_OrderView_4->show();
            ui->pushButton_ModifyOrder_4->show();
            ui->pushButton_CancelOrder_4->show();
            ui->pushButton_ConfirmOrder_4->show();

            ui->label_CustomerName_4->setText(getCustomerName[4]);
            ui->label_Time_4->setText(getTime[4]);

            ui->tableView_OrderView_4->setModel(modelGet[4]);


            ui->tableView_OrderView_4->setVisible(false);
            ui->tableView_OrderView_4->resizeColumnsToContents();
            ui->tableView_OrderView_4->resizeRowsToContents();
            ui->tableView_OrderView_4->setVisible(true);

            ui->tableView_OrderView_4->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_4->hide();
            ui->label_Time_4->hide();
            ui->tableView_OrderView_4->hide();
            ui->pushButton_ModifyOrder_4->hide();
            ui->pushButton_CancelOrder_4->hide();
            ui->pushButton_ConfirmOrder_4->hide();
        }

        //Order 5
        if(getOrder[5] != "")
        {
            ui->label_CustomerName_5->show();
            ui->label_Time_5->show();
            ui->tableView_OrderView_5->show();
            ui->pushButton_ModifyOrder_5->show();
            ui->pushButton_CancelOrder_5->show();
            ui->pushButton_ConfirmOrder_5->show();

            ui->label_CustomerName_5->setText(getCustomerName[5]);
            ui->label_Time_5->setText(getTime[5]);

            ui->tableView_OrderView_5->setModel(modelGet[5]);


            ui->tableView_OrderView_5->setVisible(false);
            ui->tableView_OrderView_5->resizeColumnsToContents();
            ui->tableView_OrderView_5->resizeRowsToContents();
            ui->tableView_OrderView_5->setVisible(true);

            ui->tableView_OrderView_5->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_5->hide();
            ui->label_Time_5->hide();
            ui->tableView_OrderView_5->hide();
            ui->pushButton_ModifyOrder_5->hide();
            ui->pushButton_CancelOrder_5->hide();
            ui->pushButton_ConfirmOrder_5->hide();
        }

        //Order 6
        if(getOrder[6] != "")
        {
            ui->label_CustomerName_6->show();
            ui->label_Time_6->show();
            ui->tableView_OrderView_6->show();
            ui->pushButton_ModifyOrder_6->show();
            ui->pushButton_CancelOrder_6->show();
            ui->pushButton_ConfirmOrder_6->show();

            ui->label_CustomerName_6->setText(getCustomerName[6]);
            ui->label_Time_6->setText(getTime[6]);

            ui->tableView_OrderView_6->setModel(modelGet[6]);


            ui->tableView_OrderView_6->setVisible(false);
            ui->tableView_OrderView_6->resizeColumnsToContents();
            ui->tableView_OrderView_6->resizeRowsToContents();
            ui->tableView_OrderView_6->setVisible(true);

            ui->tableView_OrderView_6->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_6->hide();
            ui->label_Time_6->hide();
            ui->tableView_OrderView_6->hide();
            ui->pushButton_ModifyOrder_6->hide();
            ui->pushButton_CancelOrder_6->hide();
            ui->pushButton_ConfirmOrder_6->hide();
        }

        //Order 7
        if(getOrder[7] != "")
        {
            ui->label_CustomerName_7->show();
            ui->label_Time_7->show();
            ui->tableView_OrderView_7->show();
            ui->pushButton_ModifyOrder_7->show();
            ui->pushButton_CancelOrder_7->show();
            ui->pushButton_ConfirmOrder_7->show();

            ui->label_CustomerName_7->setText(getCustomerName[7]);
            ui->label_Time_7->setText(getTime[7]);

            ui->tableView_OrderView_7->setModel(modelGet[7]);


            ui->tableView_OrderView_7->setVisible(false);
            ui->tableView_OrderView_7->resizeColumnsToContents();
            ui->tableView_OrderView_7->resizeRowsToContents();
            ui->tableView_OrderView_7->setVisible(true);

            ui->tableView_OrderView_7->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_7->hide();
            ui->label_Time_7->hide();
            ui->tableView_OrderView_7->hide();
            ui->pushButton_ModifyOrder_7->hide();
            ui->pushButton_CancelOrder_7->hide();
            ui->pushButton_ConfirmOrder_7->hide();
        }

        //Order 8
        if(getOrder[8] != "")
        {
            ui->label_CustomerName_8->show();
            ui->label_Time_8->show();
            ui->tableView_OrderView_8->show();
            ui->pushButton_ModifyOrder_8->show();
            ui->pushButton_CancelOrder_8->show();
            ui->pushButton_ConfirmOrder_8->show();

            ui->label_CustomerName_8->setText(getCustomerName[8]);
            ui->label_Time_8->setText(getTime[8]);

            ui->tableView_OrderView_8->setModel(modelGet[8]);


            ui->tableView_OrderView_8->setVisible(false);
            ui->tableView_OrderView_8->resizeColumnsToContents();
            ui->tableView_OrderView_8->resizeRowsToContents();
            ui->tableView_OrderView_8->setVisible(true);

            ui->tableView_OrderView_8->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_8->hide();
            ui->label_Time_8->hide();
            ui->tableView_OrderView_8->hide();
            ui->pushButton_ModifyOrder_8->hide();
            ui->pushButton_CancelOrder_8->hide();
            ui->pushButton_ConfirmOrder_8->hide();
        }

        //Order 9
        if(getOrder[9] != "")
        {
            ui->label_CustomerName_9->show();
            ui->label_Time_9->show();
            ui->tableView_OrderView_9->show();
            ui->pushButton_ModifyOrder_9->show();
            ui->pushButton_CancelOrder_9->show();
            ui->pushButton_ConfirmOrder_9->show();

            ui->label_CustomerName_9->setText(getCustomerName[9]);
            ui->label_Time_9->setText(getTime[9]);

            ui->tableView_OrderView_9->setModel(modelGet[9]);


            ui->tableView_OrderView_9->setVisible(false);
            ui->tableView_OrderView_9->resizeColumnsToContents();
            ui->tableView_OrderView_9->resizeRowsToContents();
            ui->tableView_OrderView_9->setVisible(true);

            ui->tableView_OrderView_9->setShowGrid(false);
        }
        else
        {
            ui->label_CustomerName_9->hide();
            ui->label_Time_9->hide();
            ui->tableView_OrderView_9->hide();
            ui->pushButton_ModifyOrder_9->hide();
            ui->pushButton_CancelOrder_9->hide();
            ui->pushButton_ConfirmOrder_9->hide();
        }
    }
}

void MainWindow::on_pushButton_MonthlyReports_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Print/Save to PDF", "Do you wish to print this table?", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QMessageBox::information(this, tr("Print Preview"), tr("Click OK and Wait"));

        QPrinter printer;

        QPrintPreviewDialog *dialog = new QPrintPreviewDialog(&printer, this);
        connect(dialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printMonthlyReports(QPrinter*)));
        if(dialog->exec())
        {
            //document->print(&printer); //Reason it asks TWICE
            strStream.clear();
            delete document;

            on_pushButton_ReportsPage_clicked();
        }
        else
        {

        }
    }
}

void MainWindow::on_pushButton_Export2ExcelMonthlyReports_clicked()
{
    QDate showMonth = QDate::currentDate();
    QString showMonthString = locale().toString(showMonth, "yyyy-MMM-dd");

    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery *qry = new QSqlQuery(db);

    //qry->prepare("select ID, Brand, Rim_Size, Tire_Size, Description, Remarks, Products, Stocks, Fasttires, Banale, Previous_Price, replace(replace(Price, ',', ''), '$', '') as Price, ReOrder, `Sugt OrderForm`, Vehicle, `Date Added`, Type from inventory;");
    qry->prepare("SELECT `Order Type`, `Order ID`, Customer, Date, Time, Employee, replace(replace(`Sales`, ',', ''), '$', '') as `Sales`,  Discount, Vat, replace(replace(`Total`, ',', ''), '$', '') as `Total` from reports_summary ORDER BY `Order ID` asc; ");
    qry->exec();
    model->setQuery(*qry);

    ui->tableView_ReportsSummary->setModel(model);
    //ui->tableView_ReportsSummary->model()->setHeaderData(0, Qt::Horizontal, tr("IDNum"));

    QString textData;
    int rows = ui->tableView_ReportsSummary->model()->rowCount();
    int columns = ui->tableView_ReportsSummary->model()->columnCount();

    for (int i = 0; i < 1; i++)
    {
        for (int j = 0; j < columns; j++)
        {
                textData +=  ui->tableView_ReportsSummary->model()->headerData(j, Qt::Horizontal).toString();
                textData += ",";
        }
    }

    textData += "\r\n";

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
                textData += ui->tableView_ReportsSummary->model()->data(ui->tableView_ReportsSummary->model()->index(i, j)).toString();
                textData += "," ;     // for .csv file format
        }
        textData += "\r\n";             // (optional: for new line segmentation)
    }


    QString name = qgetenv("USER");
    if (name.isEmpty())
        name = qgetenv("USERNAME");
    qDebug() << name;


//    QFile csvFile("C:/Users/Desktop/"+showMonthString+"(Monthly Reports).csv");
//    if(csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
//    {
//        if(ui->tableView_ReportsSummary)
//        {
//            QMessageBox::information(this, tr("Export to Excel"), tr("File Saved as Excel File!"));
//        }

//        QTextStream out(&csvFile);
//        out << textData;

//        csvFile.close();

//        on_pushButton_ReportsPage_clicked();
//    }
}

void MainWindow::on_pushButton_ChangeView_clicked()
{
    if(ui->stackedWidget_2->currentIndex() == 0)
    {
        ui->stackedWidget_2->setCurrentIndex(1);
    }
    else if(ui->stackedWidget_2->currentIndex() == 1)
    {
        ui->stackedWidget_2->setCurrentIndex(0);
    }
}

void MainWindow::on_pushButton_ModifyOrder_0_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_0->text();
        orderid = getOrder[0];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[0]);
        mod.append(cust);
        mod.append(time);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_ModifyOrder_1_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_1->text();
        orderid = getOrder[1];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[1]);
        mod.append(cust);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_ModifyOrder_2_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_2->text();
        orderid = getOrder[2];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[2]);
        mod.append(cust);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_ModifyOrder_3_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_3->text();
        orderid = getOrder[3];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[3]);
        mod.append(cust);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_ModifyOrder_4_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_0->text();
        orderid = getOrder[4];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[4]);
        mod.append(cust);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_ModifyOrder_5_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_5->text();
        orderid = getOrder[5];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[5]);
        mod.append(cust);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_ModifyOrder_6_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_6->text();
        orderid = getOrder[6];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[6]);
        mod.append(cust);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_ModifyOrder_7_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_7->text();
        orderid = getOrder[7];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[7]);
        mod.append(cust);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_ModifyOrder_8_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_8->text();
        orderid = getOrder[8];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[8]);
        mod.append(cust);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_ModifyOrder_9_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to Modify this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QString cust, orderid;
        cust = ui->label_CustomerName_9->text();
        orderid = getOrder[9];

        ui->lineEdit_OrderID->setText(orderid);
        ui->lineEdit_OrderCustomerName->setText(cust);

        QSqlQuery qry;
        qry.prepare("select Time from orderstatus where `Order ID` = '"+orderid+"' and Customer = '"+cust+"'; ");
        if(qry.exec())
        {
            while(qry.next())
            {
                oldTime = qry.value(0).toString();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error::get Time"), qry.lastError().text());
        }

        managerPass *man = new managerPass();
        man->show();

        connect(this, SIGNAL(sendModifyOrder(QStringList)), man, SLOT(receiveModifyOrder(QStringList)));

        QStringList mod;
        mod.append(getOrder[9]);
        mod.append(cust);

        emit sendModifyOrder(mod);

        man->exec();

        refreshOrderListStatus();
    }
}

void MainWindow::on_pushButton_CancelOrder_0_clicked()
{
    getCustomerName[0] = ui->label_CustomerName_0->text();
    getTime[0] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[0]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[0]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[0]+"' and Time = '"+getTime[0]+"' and Customer = '"+getCustomerName[0]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[0]+"' and Time = '"+getTime[0]+"' and Customer = '"+getCustomerName[0]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_CancelOrder_1_clicked()
{
    getCustomerName[1] = ui->label_CustomerName_1->text();
    getTime[1] = ui->label_Time_1->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[1]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[1]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[1]+"' and Time = '"+getTime[1]+"' and Customer = '"+getCustomerName[1]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[1]+"' and Time = '"+getTime[1]+"' and Customer = '"+getCustomerName[1]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_CancelOrder_2_clicked()
{
    getCustomerName[2] = ui->label_CustomerName_2->text();
    getTime[2] = ui->label_Time_2->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[2]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[2]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[2]+"' and Time = '"+getTime[2]+"' and Customer = '"+getCustomerName[2]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[2]+"' and Time = '"+getTime[2]+"' and Customer = '"+getCustomerName[2]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_CancelOrder_3_clicked()
{
    getCustomerName[3] = ui->label_CustomerName_3->text();
    getTime[3] = ui->label_Time_3->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[3]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[3]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[3]+"' and Time = '"+getTime[3]+"' and Customer = '"+getCustomerName[3]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[3]+"' and Time = '"+getTime[3]+"' and Customer = '"+getCustomerName[3]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_CancelOrder_4_clicked()
{
    getCustomerName[4] = ui->label_CustomerName_4->text();
    getTime[4] = ui->label_Time_4->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[4]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[4]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[4]+"' and Time = '"+getTime[4]+"' and Customer = '"+getCustomerName[4]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[4]+"' and Time = '"+getTime[4]+"' and Customer = '"+getCustomerName[4]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_CancelOrder_5_clicked()
{
    getCustomerName[5] = ui->label_CustomerName_5->text();
    getTime[5] = ui->label_Time_5->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[5]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[5]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[5]+"' and Time = '"+getTime[5]+"' and Customer = '"+getCustomerName[5]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[5]+"' and Time = '"+getTime[5]+"' and Customer = '"+getCustomerName[5]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_CancelOrder_6_clicked()
{
    getCustomerName[6] = ui->label_CustomerName_6->text();
    getTime[6] = ui->label_Time_6->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[6]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[6]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[6]+"' and Time = '"+getTime[6]+"' and Customer = '"+getCustomerName[6]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[6]+"' and Time = '"+getTime[6]+"' and Customer = '"+getCustomerName[6]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_CancelOrder_7_clicked()
{
    getCustomerName[7] = ui->label_CustomerName_7->text();
    getTime[7] = ui->label_Time_7->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[7]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[7]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[7]+"' and Time = '"+getTime[7]+"' and Customer = '"+getCustomerName[7]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[7]+"' and Time = '"+getTime[7]+"' and Customer = '"+getCustomerName[7]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_CancelOrder_8_clicked()
{
    getCustomerName[8] = ui->label_CustomerName_8->text();
    getTime[8] = ui->label_Time_8->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[8]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[8]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[8]+"' and Time = '"+getTime[8]+"' and Customer = '"+getCustomerName[8]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[8]+"' and Time = '"+getTime[8]+"' and Customer = '"+getCustomerName[8]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_CancelOrder_9_clicked()
{
    getCustomerName[9] = ui->label_CustomerName_9->text();
    getTime[9] = ui->label_Time_9->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CANCEL this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[9]+"';");
        if(qry.exec())
        {
            qDebug() << "Delete from orderstatus";

            QSqlQuery qry;
            qry.prepare("select max(id) from reports where `Order ID` = '"+getOrder[9]+"'; ");
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
                    qry.prepare("DELETE from reports where `Order ID` = '"+getOrder[9]+"' and Time = '"+getTime[9]+"' and Customer = '"+getCustomerName[9]+"';");
                    if(qry.exec())
                    {
                        qDebug() << "Delete from reports";
                        QSqlQuery qry;
                        qry.prepare("DELETE from reports_summary where `Order ID` = '"+getOrder[9]+"' and Time = '"+getTime[9]+"' and Customer = '"+getCustomerName[9]+"';");
                        if(qry.exec())
                        {
                            qDebug() << "Delete from reports_summary";
                            on_pushButton_OrderPage_clicked();
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
}

void MainWindow::on_pushButton_ConfirmOrder_0_clicked()
{
    getCustomerName[0] = ui->label_CustomerName_0->text();
    getTime[0] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[0]+"' and Customer = '"+getCustomerName[0]+"' and Time = '"+getTime[0]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[0]+"' and Customer = '"+getCustomerName[0]+"' and Time = '"+getTime[0]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[0]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[0]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}

void MainWindow::on_pushButton_ConfirmOrder_1_clicked()
{
    getCustomerName[1] = ui->label_CustomerName_0->text();
    getTime[1] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[1]+"' and Customer = '"+getCustomerName[1]+"' and Time = '"+getTime[1]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[1]+"' and Customer = '"+getCustomerName[1]+"' and Time = '"+getTime[1]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[1]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[1]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}

void MainWindow::on_pushButton_ConfirmOrder_2_clicked()
{
    getCustomerName[2] = ui->label_CustomerName_0->text();
    getTime[2] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[2]+"' and Customer = '"+getCustomerName[2]+"' and Time = '"+getTime[2]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[2]+"' and Customer = '"+getCustomerName[2]+"' and Time = '"+getTime[2]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[2]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[2]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}

void MainWindow::on_pushButton_ConfirmOrder_3_clicked()
{
    getCustomerName[3] = ui->label_CustomerName_0->text();
    getTime[3] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[3]+"' and Customer = '"+getCustomerName[3]+"' and Time = '"+getTime[3]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[3]+"' and Customer = '"+getCustomerName[3]+"' and Time = '"+getTime[3]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[3]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[3]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}

void MainWindow::on_pushButton_ConfirmOrder_4_clicked()
{
    getCustomerName[4] = ui->label_CustomerName_0->text();
    getTime[4] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[4]+"' and Customer = '"+getCustomerName[4]+"' and Time = '"+getTime[4]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[4]+"' and Customer = '"+getCustomerName[4]+"' and Time = '"+getTime[4]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[4]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[4]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}

void MainWindow::on_pushButton_ConfirmOrder_5_clicked()
{
    getCustomerName[5] = ui->label_CustomerName_0->text();
    getTime[5] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[5]+"' and Customer = '"+getCustomerName[5]+"' and Time = '"+getTime[5]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[5]+"' and Customer = '"+getCustomerName[5]+"' and Time = '"+getTime[5]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[5]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[5]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}

void MainWindow::on_pushButton_ConfirmOrder_6_clicked()
{
    getCustomerName[6] = ui->label_CustomerName_0->text();
    getTime[6] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[6]+"' and Customer = '"+getCustomerName[6]+"' and Time = '"+getTime[6]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[6]+"' and Customer = '"+getCustomerName[6]+"' and Time = '"+getTime[6]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[6]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[6]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}

void MainWindow::on_pushButton_ConfirmOrder_7_clicked()
{
    getCustomerName[7] = ui->label_CustomerName_0->text();
    getTime[7] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[7]+"' and Customer = '"+getCustomerName[7]+"' and Time = '"+getTime[7]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[7]+"' and Customer = '"+getCustomerName[7]+"' and Time = '"+getTime[7]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[7]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[7]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}

void MainWindow::on_pushButton_ConfirmOrder_8_clicked()
{
    getCustomerName[8] = ui->label_CustomerName_0->text();
    getTime[8] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[8]+"' and Customer = '"+getCustomerName[8]+"' and Time = '"+getTime[8]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[8]+"' and Customer = '"+getCustomerName[8]+"' and Time = '"+getTime[8]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[8]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[8]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}

void MainWindow::on_pushButton_ConfirmOrder_9_clicked()
{
    getCustomerName[9] = ui->label_CustomerName_0->text();
    getTime[9] = ui->label_Time_0->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Cancel Order", "Are you sure to CONFIRM this order???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
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
                qry.prepare("UPDATE reports SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[9]+"' and Customer = '"+getCustomerName[9]+"' and Time = '"+getTime[9]+"'; ");
                if(qry.exec())
                {
                    qDebug() << "Update Reports";

                    QSqlQuery qry;
                    qry.prepare("UPDATE reports_summary SET `Order ID` = '"+finalOrderID+"' where `Order ID` = '"+getOrder[9]+"' and Customer = '"+getCustomerName[9]+"' and Time = '"+getTime[9]+"'; ");
                    if(qry.exec())
                    {
                        qDebug() << "Update Reports Summary";

                        QSqlQuery qry;
                        qry.prepare("select `Food Name`, Description, Category, Qty from orderstatus where `Order ID` = '"+getOrder[9]+"'; ");
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
                            qry.prepare("DELETE from orderstatus where `Order ID` = '"+getOrder[9]+"'; ");
                            if(qry.exec())
                            {
                                qDebug() << "Delete from Order Status";

                                QSqlQuery qry;
                                qry.prepare("INSERT into orderid (`Order ID`) values ('"+finalOrderID+"'); ");
                                if(qry.exec())
                                {
                                    QMessageBox::information(this, tr("Confirm Order"), tr("Order Confrimed!!!"));
                                    on_pushButton_OrderPage_clicked();
                                    qDebug() << "Order Confirmed";
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
}
