#include "mainwindow.h"
#include "loginwindow.h"
#include "kitchenview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    kitchenView z;
    z.show();

    loginWindow x;
    x.show();


    return a.exec();
}
