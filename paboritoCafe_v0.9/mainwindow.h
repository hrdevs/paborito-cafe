#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextDocument>
#include <QPainter>
#include <QTextStream>

#include "managerpass.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSqlDatabase db;
    QString ModifyOrderID;

    QString getOrder[10] = {"", "", "", "", "", "", "", "", "", ""};

    QString getCustomerName[10] = {"", "", "", "", "", "", "", "", "", ""};
    QString getTime[10] = {"", "", "", "", "", "", "", "", "", ""};

    QString oldTime = "";

public slots:
    void HomePage();
    void OrderPage();
    void MenuPage();
    void ManageAccountsPage();
    void ReportsPage();

    void setMenuTable(QString& categ);

    void addAccount();
    void delAccount();

    void orderList();
    void orderStatus();
    void orderStatus_2();
    void refreshOrderListStatus();

    void loadReportsSalesDiscTotal();

    void loadVat();
    void loadDiscount();

    void printMonthlyReports(QPrinter *printer);

    void clearOrders();
    void setOrders();

private slots:
    void receiveDataAccountFrom(QStringList pass);

    void on_pushButton_Logout_clicked();

    void on_pushButton_OrderPage_clicked();

    void on_pushButton_OrderPage_HomePage_clicked();

    void on_pushButton_MenuPage_clicked();

    void on_pushButton_MenuHomePage_clicked();

    void on_pushButton_ReportsPage_clicked();

    void on_pushButton_ReportsHomePage_clicked();

    void on_pushButton_ManageAccounts_clicked();

    void on_pushButton_ManageAccountsHomePage_clicked();

    void on_pushButton_MenuAdd_clicked();

    void on_pushButton_MenuRiceBowls_clicked();

    void on_pushButton_MenuFingerFoods_clicked();

    void on_pushButton_MenuPasta_clicked();

    void on_pushButton_MenuPizza_clicked();

    void on_pushButton_MenuSpecialty_clicked();

    void on_pushButton_MenuColdDrinks_clicked();

    void on_pushButton_MenuHotDrinls_clicked();

    void on_pushButton_MenuOthers_clicked();

    void on_pushButton_MenuDel_clicked();

    void on_tableView_Menu_doubleClicked(const QModelIndex &index);

    void on_tableView_Menu_clicked(const QModelIndex &index);

    void on_pushButton_OrderRiceBowls_clicked();

    void on_pushButton_OrderFingerFoods_clicked();

    void on_pushButton_OrderPasta_clicked();

    void on_pushButton_OrderPizza_clicked();

    void on_pushButton_OrderSpecialty_clicked();

    void on_pushButton_OrderColdDrinks_clicked();

    void on_pushButton_OrderHotDrinks_clicked();

    void on_pushButton_OrderOthers_clicked();

    void on_tableView_OrderMenu_clicked(const QModelIndex &index);

    void on_tableView_OrderMenu_doubleClicked(const QModelIndex &index);

    void on_tableView_OrderList_clicked(const QModelIndex &index);

    void on_pushButton_OrderSubmit_clicked();

    void on_pushButton_OrderClear_clicked();

    void on_tableView_OrderStatus_doubleClicked(const QModelIndex &index);

    void on_pushButton_AddAcount_clicked();
    
    void on_pushButton_DelAccount_clicked();
    
    void on_tableView_ManageAccounts_doubleClicked(const QModelIndex &index);
    
    void on_comboBox_ReportsSortByDate_currentTextChanged(const QString &arg1);

    void on_comboBox_ReportsSortByCustomer_currentTextChanged(const QString &arg1);

    void on_comboBox_ReportsSortByEmp_currentTextChanged(const QString &arg1);

    void on_groupBox_ReportsSortBy_clicked(bool checked);

    void on_checkBox_Reports_EnableDate_clicked(bool checked);

    void on_checkBox_Reports_EnableCust_clicked(bool checked);

    void on_checkBox_Reports_EnableEmp_clicked(bool checked);

    void on_pushButton_OrderAddVat_clicked();

    void on_pushButton_AddOrderDiscount_clicked();

    void on_comboBox_OrderDiscount_currentTextChanged(const QString &arg1);

    void on_comboBox_OrderVAT_currentTextChanged(const QString &arg1);

    void on_tabWidget_OrderPage_currentChanged(int index);

    void on_pushButton_MonthlyReports_clicked();

    void on_pushButton_Export2ExcelMonthlyReports_clicked();

    void on_pushButton_ChangeView_clicked();

    void on_pushButton_ModifyOrder_0_clicked();

    void on_pushButton_ModifyOrder_1_clicked();

    void on_pushButton_ModifyOrder_2_clicked();

    void on_pushButton_ModifyOrder_3_clicked();

    void on_pushButton_ModifyOrder_4_clicked();

    void on_pushButton_ModifyOrder_5_clicked();

    void on_pushButton_ModifyOrder_6_clicked();

    void on_pushButton_ModifyOrder_7_clicked();

    void on_pushButton_ModifyOrder_8_clicked();

    void on_pushButton_ModifyOrder_9_clicked();

    void on_pushButton_CancelOrder_0_clicked();

    void on_pushButton_CancelOrder_1_clicked();

    void on_pushButton_CancelOrder_2_clicked();

    void on_pushButton_CancelOrder_3_clicked();

    void on_pushButton_CancelOrder_4_clicked();

    void on_pushButton_CancelOrder_5_clicked();

    void on_pushButton_CancelOrder_6_clicked();

    void on_pushButton_CancelOrder_7_clicked();

    void on_pushButton_CancelOrder_8_clicked();

    void on_pushButton_CancelOrder_9_clicked();

    void on_pushButton_ConfirmOrder_0_clicked();

    void on_pushButton_ConfirmOrder_1_clicked();

    void on_pushButton_ConfirmOrder_2_clicked();

    void on_pushButton_ConfirmOrder_3_clicked();

    void on_pushButton_ConfirmOrder_4_clicked();

    void on_pushButton_ConfirmOrder_5_clicked();

    void on_pushButton_ConfirmOrder_6_clicked();

    void on_pushButton_ConfirmOrder_7_clicked();

    void on_pushButton_ConfirmOrder_8_clicked();

    void on_pushButton_ConfirmOrder_9_clicked();

private:
    Ui::MainWindow *ui;

    QString first, middle, last, type, date, time;
    QString username, password;

    QString strStream;
    QTextDocument *document;

signals:
    void sendEditItem(QStringList);
    void sendUpdateKitchenView(QStringList);
    void sendEditAccount(QStringList);
    void sendModifyOrder(QStringList);
};

#endif // MAINWINDOW_H
