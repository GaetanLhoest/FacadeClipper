#include "view/facadeclipper.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FacadeClipper w;
    w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
    w.show();

    return a.exec();
}
