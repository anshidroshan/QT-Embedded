#include "dpos.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dpos w;
    w.show();

    return a.exec();
}
