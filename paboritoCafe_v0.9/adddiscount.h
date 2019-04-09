#ifndef ADDDISCOUNT_H
#define ADDDISCOUNT_H

#include <QDialog>

namespace Ui {
class addDiscount;
}

class addDiscount : public QDialog
{
    Q_OBJECT

public:
    explicit addDiscount(QWidget *parent = 0);
    ~addDiscount();

private slots:
    void on_pushButton_clicked();

private:
    Ui::addDiscount *ui;
};

#endif // ADDDISCOUNT_H
