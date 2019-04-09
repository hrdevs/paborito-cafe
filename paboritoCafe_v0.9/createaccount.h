#ifndef CREATEACCOUNT_H
#define CREATEACCOUNT_H

#include <QDialog>

namespace Ui {
class createAccount;
}

class createAccount : public QDialog
{
    Q_OBJECT

public:
    explicit createAccount(QWidget *parent = 0);
    ~createAccount();

private slots:

    void on_pushButton_Confirm_clicked();

    void on_pushButton_Cancel_clicked();

private:
    Ui::createAccount *ui;

    QString first = "", middle = "", last = "", username = "", password = "", type = "", status = "";
};

#endif // CREATEACCOUNT_H
