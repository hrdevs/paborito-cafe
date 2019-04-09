#ifndef CONFIRMORDERSTATUS_H
#define CONFIRMORDERSTATUS_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class confirmOrderStatus;
}

class confirmOrderStatus : public QDialog
{
    Q_OBJECT

public:
    explicit confirmOrderStatus(QWidget *parent = 0);
    ~confirmOrderStatus();

    void hidePromptManager();
    void showPromptManager();

    void modifyOrderFunct();
    void confirmOrderFunct();
    void cancelOrderFunct();

    void connOpen();
    void connClose();

    QSqlDatabase db;

private slots:
    void receiveOrderStatus(QStringList orderStatus);

    void on_pushButton_ModifyOrder_clicked();

    void on_pushButton_ConfirmOrder_clicked();

    void on_pushButton_CancelOrder_clicked();

    void on_pushButton_Close_clicked();

    void on_lineEdit_ManagerPassword_textChanged(const QString &arg1);

private:
    Ui::confirmOrderStatus *ui;

    QString orderType, orderID, time, customerName;

signals:

};

#endif // CONFIRMORDERSTATUS_H
