#include "kitchenview.h"
#include "ui_kitchenview.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>

#include <QDate>

#include <QTimer>

kitchenView::kitchenView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::kitchenView)
{
    ui->setupUi(this);

    connClose();
    connOpen();

    timer = new QTimer(this);
    //timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(orders()));
    timer->start(1000);

    //Set Date
    QDate showDate = QDate::currentDate();
    QString showDateString = locale().toString(showDate, "MM/dd/yyyy");

    ui->label_Date->setText(showDateString);
}

kitchenView::~kitchenView()
{
    delete ui;
}

void kitchenView::orderStatus()
{
    QSqlQueryModel *model, *model1;
    model = new QSqlQueryModel;
    model1 = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;
    qry->prepare("select *from `paboritocafe_v1`.`orderstatus` ORDER BY ID, `Order Type` asc; ");
    if(qry->exec())
    {
        //qDebug() << "Order Status (Dine In)";

        //for TableView
        model->setQuery(*qry);
        ui->tableView_KitchenView->setModel(model);
        ui->tableView_KitchenView->setColumnHidden(0, true);
        //ui->tableView_KitchenView->setColumnHidden(1, true);
        ui->tableView_KitchenView->setColumnHidden(2, true);
        ui->tableView_KitchenView->setColumnHidden(3, true);
        ui->tableView_KitchenView->setColumnHidden(8, true);
        ui->tableView_KitchenView->setColumnHidden(9, true);
        ui->tableView_KitchenView->setColumnHidden(11, true);
        ui->tableView_KitchenView->setColumnHidden(12, true);
        ui->tableView_KitchenView->setColumnHidden(13, true);
        ui->tableView_KitchenView->setVisible(false);
        ui->tableView_KitchenView->resizeColumnsToContents();
        ui->tableView_KitchenView->resizeRowsToContents();
        ui->tableView_KitchenView->setVisible(true);
    }
}

void kitchenView::connOpen()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("paboritocafe_v1");
    db.setHostName("localhost");
    db.setUserName("paborito");
    db.setPassword("paborito");
    db.setPort(3306);

    if(db.open())
    {
        //qDebug()<<"Connected!";
        //ui->label_Status->setText("Connected!!!");
    }
    else
    {
        //ui->label_Status->setText("Failed to connect!!!");
        //qDebug()<<"Failed to connect!!!";
    }
}

void kitchenView::connClose()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.close();
    db.removeDatabase(QSqlDatabase::defaultConnection);
}

void kitchenView::getOrders()
{
    clear();
    order[0] = "";
    order[1] = "";
    order[2] = "";
    order[3] = "";
    order[4] = "";
    order[5] = "";
    order[6] = "";
    order[7] = "";
    order[8] = "";
    order[9] = "";

    QSqlQuery qry;
    qry.prepare("select distinct `Order ID` from paboritocafe_v1.orderstatus ORDER BY `Order ID` asc; ");
    if(qry.exec())
    {
        int count = 0;
        //qDebug() << "Fetch Order:";

        while(qry.next())
        {
            order[count] = qry.value(0).toString();
            //qDebug() << order[count];

            count++;
        }


        //qDebug() << "First Row: " + order[0];

        if(order[0] == "" || order[1] == "" || order[2] == "" || order[3] == "" || order[4] == "" || order[5] == "" || order[6] == "" || order[7] == "" || order[8] == "" || order[9] == "")
        {
            return;
        }
            //qDebug() << "Done fetching Order";
    }
    else
    {
        QMessageBox::critical(this, ("Error::Getting Orders Kitchen View", "Database cannot be found"), qry.lastError().text());
        timer->stop();
    }
}

void kitchenView::orders()
{
    ui->stackedWidget->setCurrentIndex(1);
    getOrders();

    QSqlQueryModel *modelGet[10];
    QSqlQuery *qryGet[10];

    if(order[0] != "")
    {
        for(int x = 0; x < 10; x++)
        {
            //qDebug() << "Getting Order Details for: " + order[x];

            QSqlQuery qry;
            qry.prepare("select distinct Customer, Time from paboritocafe_v1.orderstatus where `Order ID` = '"+order[x]+"'; ");
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
                    qryGet[x]->prepare("select `Order Type`, `Food Name`, Category, Qty from paboritocafe_v1.orderstatus where `Order ID` = '"+order[x]+"' ORDER BY IF(`Order Type` RLIKE '^[a-z]', 1, 2), `Order Type`; ");
                    if(qryGet[x]->exec())
                    {
                        modelGet[x]->setQuery(*qryGet[x]);
                    }
                }
           }
        }

        //Order 0
        if(order[0] != "")
        {
            ui->orderCustomer_0->show();
            ui->orderTime_0->show();
            ui->orderListView_0->show();

            ui->orderCustomer_0->setText(getCustomerName[0]);
            ui->orderTime_0->setText(getTime[0]);

            ui->orderListView_0->setModel(modelGet[0]);

            ui->orderListView_0->setVisible(false);
            ui->orderListView_0->resizeColumnsToContents();
            ui->orderListView_0->resizeRowsToContents();
            ui->orderListView_0->setVisible(true);

            ui->orderListView_0->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_0->hide();
            ui->orderTime_0->hide();
            ui->orderListView_0->hide();
        }

        //Order 1
        if(order[1] != "")
        {
            ui->orderCustomer_1->show();
            ui->orderTime_1->show();
            ui->orderListView_1->show();

            ui->orderCustomer_1->setText(getCustomerName[1]);
            ui->orderTime_1->setText(getTime[1]);

            ui->orderListView_1->setModel(modelGet[1]);

            ui->orderListView_1->setVisible(false);
            ui->orderListView_1->resizeColumnsToContents();
            ui->orderListView_1->resizeRowsToContents();
            ui->orderListView_1->setVisible(true);

            ui->orderListView_1->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_1->hide();
            ui->orderTime_1->hide();
            ui->orderListView_1->hide();
        }

        //Order 2
        if(order[2] != "")
        {
            ui->orderCustomer_2->show();
            ui->orderTime_2->show();
            ui->orderListView_2->show();

            ui->orderCustomer_2->setText(getCustomerName[2]);
            ui->orderTime_2->setText(getTime[2]);

            ui->orderListView_2->setModel(modelGet[2]);

            ui->orderListView_2->setVisible(false);
            ui->orderListView_2->resizeColumnsToContents();
            ui->orderListView_2->resizeRowsToContents();
            ui->orderListView_2->setVisible(true);

            ui->orderListView_2->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_2->hide();
            ui->orderTime_2->hide();
            ui->orderListView_2->hide();
        }

        //Order 3
        if(order[3] != "")
        {
            ui->orderCustomer_3->show();
            ui->orderTime_3->show();
            ui->orderListView_3->show();

            ui->orderCustomer_3->setText(getCustomerName[3]);
            ui->orderTime_3->setText(getTime[3]);

            ui->orderListView_3->setModel(modelGet[3]);

            ui->orderListView_3->setVisible(false);
            ui->orderListView_3->resizeColumnsToContents();
            ui->orderListView_3->resizeRowsToContents();
            ui->orderListView_3->setVisible(true);

            ui->orderListView_3->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_3->hide();
            ui->orderTime_3->hide();
            ui->orderListView_3->hide();
        }

        //Order 4
        if(order[4] != "")
        {
            ui->orderCustomer_4->show();
            ui->orderTime_4->show();
            ui->orderListView_4->show();

            ui->orderCustomer_4->setText(getCustomerName[4]);
            ui->orderTime_4->setText(getTime[4]);

            ui->orderListView_4->setModel(modelGet[4]);

            ui->orderListView_4->setVisible(false);
            ui->orderListView_4->resizeColumnsToContents();
            ui->orderListView_4->resizeRowsToContents();
            ui->orderListView_4->setVisible(true);

            ui->orderListView_4->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_4->hide();
            ui->orderTime_4->hide();
            ui->orderListView_4->hide();
        }

        //Order 5
        if(order[5] != "")
        {
            ui->orderCustomer_5->show();
            ui->orderTime_5->show();
            ui->orderListView_5->show();

            ui->orderCustomer_5->setText(getCustomerName[5]);
            ui->orderTime_5->setText(getTime[5]);

            ui->orderListView_5->setModel(modelGet[5]);

            ui->orderListView_5->setVisible(false);
            ui->orderListView_5->resizeColumnsToContents();
            ui->orderListView_5->resizeRowsToContents();
            ui->orderListView_5->setVisible(true);

            ui->orderListView_5->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_5->hide();
            ui->orderTime_5->hide();
            ui->orderListView_5->hide();
        }

        //Order 6
        if(order[6] != "")
        {
            ui->orderCustomer_6->show();
            ui->orderTime_6->show();
            ui->orderListView_6->show();

            ui->orderCustomer_6->setText(getCustomerName[6]);
            ui->orderTime_6->setText(getTime[6]);

            ui->orderListView_6->setModel(modelGet[6]);

            ui->orderListView_6->setVisible(false);
            ui->orderListView_6->resizeColumnsToContents();
            ui->orderListView_6->resizeRowsToContents();
            ui->orderListView_6->setVisible(true);

            ui->orderListView_6->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_6->hide();
            ui->orderTime_6->hide();
            ui->orderListView_6->hide();
        }

        //Order 7
        if(order[7] != "")
        {
            ui->orderCustomer_7->show();
            ui->orderTime_7->show();
            ui->orderListView_7->show();

            ui->orderCustomer_7->setText(getCustomerName[7]);
            ui->orderTime_7->setText(getTime[7]);

            ui->orderListView_7->setModel(modelGet[7]);

            ui->orderListView_7->setVisible(false);
            ui->orderListView_7->resizeColumnsToContents();
            ui->orderListView_7->resizeRowsToContents();
            ui->orderListView_7->setVisible(true);

            ui->orderListView_7->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_7->hide();
            ui->orderTime_7->hide();
            ui->orderListView_7->hide();
        }

        //Order 8
        if(order[8] != "")
        {
            ui->orderCustomer_8->show();
            ui->orderTime_8->show();
            ui->orderListView_8->show();

            ui->orderCustomer_8->setText(getCustomerName[8]);
            ui->orderTime_8->setText(getTime[8]);

            ui->orderListView_8->setModel(modelGet[8]);

            ui->orderListView_8->setVisible(false);
            ui->orderListView_8->resizeColumnsToContents();
            ui->orderListView_8->resizeRowsToContents();
            ui->orderListView_8->setVisible(true);

            ui->orderListView_8->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_8->hide();
            ui->orderTime_8->hide();
            ui->orderListView_8->hide();
        }

        //Order 9
        if(order[9] != "")
        {
            ui->orderCustomer_9->show();
            ui->orderTime_9->show();
            ui->orderListView_9->show();

            ui->orderCustomer_9->setText(getCustomerName[9]);
            ui->orderTime_9->setText(getTime[9]);

            ui->orderListView_9->setModel(modelGet[9]);

            ui->orderListView_9->setVisible(false);
            ui->orderListView_9->resizeColumnsToContents();
            ui->orderListView_9->resizeRowsToContents();
            ui->orderListView_9->setVisible(true);

            ui->orderListView_9->setShowGrid(false);
        }
        else
        {
            ui->orderCustomer_9->hide();
            ui->orderTime_9->hide();
            ui->orderListView_9->hide();
        }
    //qDebug() << "Set Order List";
    }
}

void kitchenView::clear()
{
    ui->orderCustomer_0->setText("Customer");
    ui->orderCustomer_1->setText("Customer");
    ui->orderCustomer_2->setText("Customer");
    ui->orderCustomer_3->setText("Customer");
    ui->orderCustomer_4->setText("Customer");
    ui->orderCustomer_5->setText("Customer");
    ui->orderCustomer_6->setText("Customer");
    ui->orderCustomer_7->setText("Customer");
    ui->orderCustomer_8->setText("Customer");
    ui->orderCustomer_9->setText("Customer");

    ui->orderTime_0->setText("Time");
    ui->orderTime_1->setText("Time");
    ui->orderTime_2->setText("Time");
    ui->orderTime_3->setText("Time");
    ui->orderTime_4->setText("Time");
    ui->orderTime_5->setText("Time");
    ui->orderTime_6->setText("Time");
    ui->orderTime_7->setText("Time");
    ui->orderTime_8->setText("Time");
    ui->orderTime_9->setText("Time");

    QSqlQueryModel *model = new QSqlQueryModel;

    QSqlQuery *qry = new QSqlQuery;

    qry->prepare("select `ORDER` from empty;");
    if(qry->exec())
    {
        model->setQuery(*qry);

        ui->orderListView_0->setModel(model);
        ui->orderListView_1->setModel(model);
        ui->orderListView_2->setModel(model);
        ui->orderListView_3->setModel(model);
        ui->orderListView_4->setModel(model);
        ui->orderListView_5->setModel(model);
        ui->orderListView_6->setModel(model);
        ui->orderListView_7->setModel(model);
        ui->orderListView_8->setModel(model);
        ui->orderListView_9->setModel(model);

        ui->orderCustomer_0->hide();
        ui->orderTime_0->hide();
        ui->orderListView_0->hide();

        ui->orderCustomer_1->hide();
        ui->orderTime_1->hide();
        ui->orderListView_1->hide();

        ui->orderCustomer_2->hide();
        ui->orderTime_2->hide();
        ui->orderListView_2->hide();

        ui->orderCustomer_3->hide();
        ui->orderTime_3->hide();
        ui->orderListView_3->hide();

        ui->orderCustomer_4->hide();
        ui->orderTime_4->hide();
        ui->orderListView_4->hide();

        ui->orderCustomer_5->hide();
        ui->orderTime_5->hide();
        ui->orderListView_5->hide();

        ui->orderCustomer_6->hide();
        ui->orderTime_6->hide();
        ui->orderListView_6->hide();

        ui->orderCustomer_7->hide();
        ui->orderTime_7->hide();
        ui->orderListView_7->hide();

        ui->orderCustomer_8->hide();
        ui->orderTime_8->hide();
        ui->orderListView_8->hide();

        ui->orderCustomer_9->hide();
        ui->orderTime_9->hide();
        ui->orderListView_9->hide();
    }
}
