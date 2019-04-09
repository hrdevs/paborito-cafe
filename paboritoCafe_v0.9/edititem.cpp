#include "edititem.h"
#include "ui_edititem.h"
#include "mainwindow.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

editItem::editItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editItem)
{
    ui->setupUi(this);
}

editItem::~editItem()
{
    delete ui;
}

void editItem::receiveEditItem(QStringList foodName)
{
    id = foodName[0];

    QString val = foodName[3];

    ui->lineEdit_FoodName->setText(foodName[1]);
    ui->lineEdit_Description->setText(foodName[2]);
    ui->doubleSpinBox_Price->setValue(val.toDouble());
    ui->comboBox_Categ->setCurrentText(foodName[4]);
    ui->lineEdit_SetImage->setText(foodName[5]);

    qDebug() << foodName[1];
}

void editItem::on_pushButton_Apply_clicked()
{
    QString name, desc, price, categ, image;
    name = ui->lineEdit_FoodName->text();
    desc = ui->lineEdit_Description->text();
    price = ui->doubleSpinBox_Price->text();
    categ = ui->comboBox_Categ->currentText();
    image = ui->lineEdit_SetImage->text();

    if(image.contains("\\"))
    {
        image.replace("\\", "/");
    }

    if(categ != "Rice Bowls" && categ != "Finger Foods" && categ != "Pasta" && categ != "Pizza" && categ != "Specialty" && categ != "Cold Drinks" && categ != "Hot Drinks" && categ != "Others")
    {
        QMessageBox::information(this, tr("Select from Category"), tr("Select from Category!!!"));
    }
    else
    {
        QSqlQuery qry;
        qry.prepare("UPDATE `paboritocafe_v1`.`menu` SET `Food Name`='"+name+"', `Description`='"+desc+"', `Price`='"+price+"', `Category`='"+categ+"', `Image`='"+image+"' WHERE `ID`='"+id+"';");
        if(qry.exec())
        {
            qDebug() << "Image Dir: " + image;
            QMessageBox::information(this, tr("Edit Item"), tr("Successfully Edited!"));
            this->close();
        }
        else
        {
            QMessageBox::critical(this, tr("Error::Edit Item"), qry.lastError().text());
        }
    }
}

void editItem::on_pushButton_Cancel_clicked()
{
    this->close();
}

void editItem::on_pushButton_SetImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image File . . ."), "C:/", "Image Files (*jpg *png *bmp);; All Files (*)");

    if(fileName != "")
    {
        ui->lineEdit_SetImage->setText(fileName);
    }
}
