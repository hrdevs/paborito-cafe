#include "editaccount.h"
#include "ui_editaccount.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

EditAccount::EditAccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditAccount)
{
    ui->setupUi(this);

    ui->pushButton_ConfirmChangePass->setDisabled(true);
}

EditAccount::~EditAccount()
{
    delete ui;
}

void EditAccount::receiveEditAccount(QStringList account)
{
    id = account[0];
    first = account[1];
    middle = account[2];
    last = account[3];
    username = account[4];
    password = account[5];
    type = account[6];

    ui->lineEdit_FirstName->setText(first);
    ui->lineEdit_MiddelName->setText(middle);
    ui->lineEdit_LastName->setText(last);
    ui->lineEdit_Username->setText(username);
    ui->lineEdit_Password->setText(password);
    ui->comboBox_accountType->setCurrentText(type);
}

void EditAccount::on_pushButton_Confirm_clicked()
{
    QString firstName, middelName, lastName, userName, passWord, accountType;
    firstName = ui->lineEdit_FirstName->text();
    middelName = ui->lineEdit_MiddelName->text();
    lastName = ui->lineEdit_LastName->text();
    userName = ui->lineEdit_Username->text();
    passWord = ui->lineEdit_Password->text();
    accountType = ui->comboBox_accountType->currentText();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Confirm Edit Account", "Are you sure to confirm changes from your account???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("UPDATE `paboritocafe_v1`.`accounts` set `First` = '"+firstName+"', `Middle` = '"+middelName+"', `Last` = '"+lastName+"', `Username` = '"+userName+"', `Password` = '"+passWord+"', `Type` = '"+accountType+"' where `ID` = '"+id+"'; ");
        if(qry.exec())
        {
            QMessageBox::information(this, tr("Edit Account"), tr("Done editing account . . ."));
            this->close();
        }
        else
        {
            QMessageBox::critical(this, tr("Error::Edit Account"), qry.lastError().text());
        }
    }
}

void EditAccount::on_pushButton_Cancel_clicked()
{
    this->close();
}

void EditAccount::on_pushButton_ChangePass_clicked()
{
    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Change Password", "Change Password???", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
       ui->stackedWidget->setCurrentIndex(1);
    }
}

void EditAccount::on_lineEdit_OldPass_textChanged(const QString &arg1)
{
    QString password, oldPass;
    password = ui->lineEdit_Password->text();
    oldPass = ui->lineEdit_OldPass->text();

    if(oldPass != password)
    {
        ui->label_ChangePassStatus->setText("Password doesn't match you old password . . .");
        ui->label_ChangePassStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");
    }
    else
    {
        ui->label_ChangePassStatus->setText("Please set your new password . . .");
        ui->label_ChangePassStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");
    }
}

void EditAccount::on_lineEdit_NewPass_textChanged(const QString &arg1)
{
    QString oldPass = ui->lineEdit_OldPass->text();

    if(oldPass != "")
    {
        ui->label_ChangePassStatus->setText("Please confirm your new password . . .");
        ui->label_ChangePassStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");
    }
    else if(oldPass == "")
    {
        ui->label_ChangePassStatus->setText("Please enter your old password FIRST . . .");
        ui->label_ChangePassStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");
    }
}

void EditAccount::on_lineEdit_ConfirmNewPass_textChanged(const QString &arg1)
{
    QString newPass, confirmNewPass;
    newPass = ui->lineEdit_NewPass->text();
    confirmNewPass = ui->lineEdit_ConfirmNewPass->text();

    if(newPass == confirmNewPass)
    {
        ui->label_ChangePassStatus->setText("Click the 'Confirm' button to apply changes . . .");
        ui->label_ChangePassStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");

        ui->pushButton_ConfirmChangePass->setEnabled(true);
    }
    else
    {
        ui->label_ChangePassStatus->setText("New password doesn't match . . .");
        ui->label_ChangePassStatus->setStyleSheet("QLabel {font-weight: bold; color: red}");

        ui->pushButton_ConfirmChangePass->setDisabled(true);
    }
}

void EditAccount::on_pushButton_ConfirmChangePass_clicked()
{
    QString confirmNewPass = ui->lineEdit_ConfirmNewPass->text();

    QMessageBox::StandardButton confirm;
    confirm = QMessageBox::question(this, "Confirm New Password", "Are you sure to proceed changing your password????", QMessageBox::Yes|QMessageBox::No);
    if(confirm == QMessageBox::Yes)
    {
        QSqlQuery qry;
        qry.prepare("UPDATE `paboritocafe_v1`.`accounts` SET `Password`= '"+confirmNewPass+"' WHERE `ID`= '"+id+"'; ");
        if(qry.exec())
        {
            QMessageBox::information(this, tr("Confirm New Password"), tr("Password Changed Successful!!!"));
            this->close();
        }
        else
        {
            QMessageBox::critical(this, tr("Error::Change Password"), qry.lastError().text());
        }
    }
}

void EditAccount::on_pushButton_CancelChangePass_clicked()
{
    ui->lineEdit_OldPass->clear();
    ui->lineEdit_NewPass->clear();
    ui->lineEdit_ConfirmNewPass->clear();

    ui->stackedWidget->setCurrentIndex(0);
}
