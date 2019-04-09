#ifndef EDITACCOUNT_H
#define EDITACCOUNT_H

#include <QDialog>

namespace Ui {
class EditAccount;
}

class EditAccount : public QDialog
{
    Q_OBJECT

public:
    explicit EditAccount(QWidget *parent = 0);
    ~EditAccount();

private slots:
    void receiveEditAccount(QStringList account);

    void on_pushButton_Confirm_clicked();

    void on_pushButton_Cancel_clicked();

    void on_pushButton_ChangePass_clicked();

    void on_lineEdit_OldPass_textChanged(const QString &arg1);

    void on_lineEdit_NewPass_textChanged(const QString &arg1);

    void on_lineEdit_ConfirmNewPass_textChanged(const QString &arg1);

    void on_pushButton_ConfirmChangePass_clicked();

    void on_pushButton_CancelChangePass_clicked();

private:
    Ui::EditAccount *ui;

    QString id, first, middle, last, username, password, type;
};

#endif // EDITACCOUNT_H
