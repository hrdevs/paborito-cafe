#ifndef ADDVAT_H
#define ADDVAT_H

#include <QDialog>

namespace Ui {
class addVat;
}

class addVat : public QDialog
{
    Q_OBJECT

public:
    explicit addVat(QWidget *parent = 0);
    ~addVat();

private slots:
    void on_pushButton_clicked();

private:
    Ui::addVat *ui;
};

#endif // ADDVAT_H
