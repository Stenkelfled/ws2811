#include "lui.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file(":/styles/splitter.css");
    if(file.open(QFile::ReadOnly)){
        QString style_sheet = QString::fromLatin1(file.readAll());
        file.close();
        a.setStyleSheet(style_sheet);
    } else {
        qDebug() << "splitter-style-file not found";
    }
    Lui w;
    w.show();

    return a.exec();
}
