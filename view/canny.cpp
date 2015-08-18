#include "view/canny.h"
#include "ui_Canny.h"

#include <QImage>

#include "methods/asmopencv.h"

using namespace cv;

Canny::Canny(QString im, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Canny)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    this->scene = new QGraphicsScene();
    ui->graphicsView->setScene(this->scene);

    this->src = new Mat();
    *src = imread(im.toUtf8().constData(), 1);


    displayImage(*src);
}

Canny::~Canny()
{
    delete ui;
    delete src;
}

void Canny::displayImage(Mat &m){
    QImage qi = ASM::mat_to_qimage_ref(m, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void Canny::on_pushButton_clicked()
{
    Mat gray;
    cvtColor(*src, gray, COLOR_BGR2GRAY);
    blur(gray, gray, Size(3,3));
    Mat edges;
    cv::Canny(gray, edges, ui->spinBox_t1->value(), ui->spinBox_t2->value(), 3);
    cvtColor(edges, edges, COLOR_GRAY2BGR);

    displayImage(edges);

}

void Canny::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}
