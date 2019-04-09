#ifndef NEWITEM_H
#define NEWITEM_H

#include <QDialog>

namespace Ui {
class newItem;
}

class newItem : public QDialog
{
    Q_OBJECT

public:
    explicit newItem(QWidget *parent = 0);
    ~newItem();

private slots:
    void on_pushButton_Add_clicked();

    void on_pushButton_Cancel_clicked();

    void on_pushButton_SetImage_clicked();

    void on_comboBox_Categ_currentTextChanged(const QString &arg1);

private:
    Ui::newItem *ui;
};

#endif // NEWITEM_H
