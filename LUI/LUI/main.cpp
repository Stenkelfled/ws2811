#include "lui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Lui w;
    w.show();

    return a.exec();
}
