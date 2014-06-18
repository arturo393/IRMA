#include <QtGui/QApplication>
#include "bpwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bpWIdget w;
    w.show();

    return a.exec();
}
