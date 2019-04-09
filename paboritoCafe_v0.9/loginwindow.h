#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QtSql/QSqlDatabase>

namespace Ui {
class loginWindow;
}

class loginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit loginWindow(QWidget *parent = 0);
    ~loginWindow();

    QSqlDatabase db;

private slots:
    void connOpen();
    void connClose();
    void runBatchFiles();
    void on_pushButton_Login_clicked();
    void on_pushButton_Cancel_clicked();
    void on_checkBox_ShowPassword_clicked(bool checked);
    void on_pushButton_CreateNewAccount_clicked();

private:
    Ui::loginWindow *ui;

signals:
    void sendDataAccount(QStringList);
};

#endif // LOGINWINDOW_H
