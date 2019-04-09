#ifndef KITCHENVIEW_H
#define KITCHENVIEW_H

#include <QMainWindow>
#include <QTimer>
//#include <QSqlDatabase>

namespace Ui {
class kitchenView;
}

class kitchenView : public QMainWindow
{
    Q_OBJECT

public:
    explicit kitchenView(QWidget *parent = 0);
    ~kitchenView();

    QString order[10] = {"", "", "", "", "", "", "", "", "", ""};

    QString getCustomerName[10] = {"", "", "", "", "", "", "", "", "", ""};
    QString getTime[10] = {"", "", "", "", "", "", "", "", "", ""};

    QTimer *timer;

public slots:
    void orderStatus();
    void connOpen();
    void connClose();

    void getOrders();

    void orders();

    void clear();

private slots:


private:
    Ui::kitchenView *ui;
};

#endif // KITCHENVIEW_H
