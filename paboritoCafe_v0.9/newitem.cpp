#include "newitem.h"
#include "ui_newitem.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

newItem::newItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newItem)
{
    ui->setupUi(this);

    ui->groupBox->hide();
}

newItem::~newItem()
{
    delete ui;
}

void newItem::on_pushButton_Add_clicked()
{
    QString foodName, desc, price, categ, image;
    foodName = ui->lineEdit_FoodName->text();
    desc = ui->lineEdit_Description->text();
    price = ui->doubleSpinBox_Price->text();
    categ = ui->comboBox_Categ->currentText();
    image = ui->lineEdit_SetImage->text();

    QString priceS, priceM, priceL;
    priceS = ui->doubleSpinBox->text();
    priceM = ui->doubleSpinBox_2->text();
    priceL = ui->doubleSpinBox_3->text();

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
        if(categ == "Pizza")
        {
            if(priceS.toDouble() == 0 || priceM.toDouble() == 0 || priceL.toDouble() == 0)
            {
                QMessageBox::information(this, tr("Set Prices for Pizza per size."), tr("Set Prices for Pizza per size."));
            }
            else
            {
                QSqlQuery qry;
                qry.prepare("INSERT INTO `paboritocafe_v1`.`menu` (`Food Name`, `Description`, `Price`, `Category`, `Sold`, `Image`) VALUES ('"+foodName+"', 'Regular', '"+priceS+"', '"+categ+"', '0', '"+image+"');");
                if(qry.exec())
                {
                    QSqlQuery qry;
                    qry.prepare("INSERT INTO `paboritocafe_v1`.`menu` (`Food Name`, `Description`, `Price`, `Category`, `Sold`, `Image`) VALUES ('"+foodName+"', 'Large', '"+priceM+"', '"+categ+"', '0', '"+image+"');");
                    if(qry.exec())
                    {
                        QSqlQuery qry;
                        qry.prepare("INSERT INTO `paboritocafe_v1`.`menu` (`Food Name`, `Description`, `Price`, `Category`, `Sold`, `Image`) VALUES ('"+foodName+"', 'Jumbo', '"+priceL+"', '"+categ+"', '0', '"+image+"');");
                        if(qry.exec())
                        {
                            qDebug() << "Image Dir: " + image;
                            QMessageBox::information(this, tr("New Item"), tr("Added Successfully"));
                            this->close();
                        }
                        else
                        {
                            QMessageBox::critical(this, tr("Error::New Item"), qry.lastError().text());
                        }
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("Error::New Item"), qry.lastError().text());
                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("Error::New Item"), qry.lastError().text());
                }
            }
        }
        else
        {
            QSqlQuery qry;
            qry.prepare("INSERT INTO `paboritocafe_v1`.`menu` (`Food Name`, `Description`, `Price`, `Category`, `Sold`, `Image`) VALUES ('"+foodName+"', '"+desc+"', '"+price+"', '"+categ+"', '0', '"+image+"');");
            if(qry.exec())
            {
                qDebug() << "Image Dir: " + image;
                QMessageBox::information(this, tr("New Item"), tr("Added Successfully"));
                this->close();
            }
            else
            {
                QMessageBox::critical(this, tr("Error::New Item"), qry.lastError().text());
            }
        }
    }
}

void newItem::on_pushButton_Cancel_clicked()
{
    this->close();
}

void newItem::on_pushButton_SetImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image File . . ."), "C:/", "Image Files (*jpg *png *bmp);; All Files (*)");

    if(fileName != "")
    {
        ui->lineEdit_SetImage->setText(fileName);
    }
}

void newItem::on_comboBox_Categ_currentTextChanged(const QString &arg1)
{
    QString categ = ui->comboBox_Categ->currentText();

    if(categ == "Pizza")
    {
        ui->groupBox->show();
        ui->lineEdit_Description->hide();
        ui->doubleSpinBox_Price->hide();
        ui->label_2->hide();
        ui->label_3->hide();
    }
    else
    {
        ui->groupBox->hide();
        ui->lineEdit_Description->show();
        ui->doubleSpinBox_Price->show();
        ui->label_2->show();
        ui->label_3->show();
    }
}
