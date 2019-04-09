#ifndef MANAGERPASS_H
#define MANAGERPASS_H

#include <QDialog>

#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

#include "mainwindow.h"

namespace Ui {
class managerPass;
}

class managerPass : public QDialog
{
    Q_OBJECT

public:
    explicit managerPass(QWidget *parent = 0);
    ~managerPass();

    QString orderID, cust, time;

private slots:
    void on_pushButton_OK_clicked();
    void modifyOrderFunct();
    void receiveModifyOrder(QStringList mod);

    void on_pushButton_Cancel_clicked();

private:
    Ui::managerPass *ui;   
};

#endif // MANAGERPASS_H
