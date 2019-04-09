#include "adddiscount.h"
#include "ui_adddiscount.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

addDiscount::addDiscount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addDiscount)
{
    ui->setupUi(this);
}

addDiscount::~addDiscount()
{
    delete ui;
}

void addDiscount::on_pushButton_clicked()
{
    QString val;
    val = ui->lineEdit->text();

    QSqlQuery qry;
    qry.prepare("select value from `paboritocafe_v1`.`discount_type`;");
    if(qry.exec())
    {
        QString data;

        while(qry.next())
        {
            data = qry.value(0).toString();
        }

        if(data == "")
        {
            QSqlQuery qry;
            qry.prepare("INSERT INTO `paboritocafe_v1`.`discount_type` (`Value`) VALUES ('"+val+"');");
            if(qry.exec())
            {
                this->close();
            }
            else
            {
                QMessageBox::critical(this, tr("Error::Add Vat"), qry.lastError().text());
            }
        }
        else
        {
            QSqlQuery qry;
            qry.prepare("UPDATE `paboritocafe_v1`.`discount_type` SET `Value`= '"+val+"';");
            if(qry.exec())
            {
                this->close();
            }
            else
            {
                QMessageBox::critical(this, tr("Error::Add Discount"), qry.lastError().text());
            }
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error::Add Discount"), qry.lastError().text());
    }
}
