#include "addvat.h"
#include "ui_addvat.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

addVat::addVat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addVat)
{
    ui->setupUi(this);
}

addVat::~addVat()
{
    delete ui;
}

void addVat::on_pushButton_clicked()
{
    QString val;
    val = ui->lineEdit->text();

    QSqlQuery qry;
    qry.prepare("select value from `paboritocafe_v1`.`vat`;");
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
            qry.prepare("INSERT INTO `paboritocafe_v1`.`vat` (`Value`) VALUES ('"+val+"');");
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
            qry.prepare("UPDATE `paboritocafe_v1`.`vat` SET `Value`= '"+val+"';");
            if(qry.exec())
            {
                this->close();
            }
            else
            {
                QMessageBox::critical(this, tr("Error::Add Vat"), qry.lastError().text());
            }
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error::Add Vat"), qry.lastError().text());
    }
}
