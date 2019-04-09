#ifndef EDITITEM_H
#define EDITITEM_H

#include <QDialog>

namespace Ui {
class editItem;
}

class editItem : public QDialog
{
    Q_OBJECT

public:
    explicit editItem(QWidget *parent = 0);
    ~editItem();

private slots:
    void receiveEditItem(QStringList gfoodName);

    void on_pushButton_Apply_clicked();

    void on_pushButton_Cancel_clicked();

    void on_pushButton_SetImage_clicked();

private:
    Ui::editItem *ui;

    QString id;

signals:
};

#endif // EDITITEM_H
