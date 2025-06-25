#include "fenmain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    fenMain w;
    w.show();
    return a.exec();
}
