#include <mywidget.h>
#include <QApplication>
#include<QPainter>
#include<QPaintEvent>
#include<QtWidgets>
#include<cstdlib>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    MyWidget wgt;


    wgt.MakeTriangleArray();
    wgt.resize(300,300);
    wgt.show();


    return a.exec();
}
