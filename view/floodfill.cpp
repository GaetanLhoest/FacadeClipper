#include "view/floodfill.h"
#include "ui_Floodfill.h"
#include <iostream>

#include <QGraphicsSceneMouseEvent>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include "methods/asmopencv.h"

using namespace ASM;
using namespace std;
using namespace cv;

FloodFill::FloodFill(QString im, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FloodFill)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);

    positions = new vector<Point>;

    src = new Mat();
    *src = imread(im.toUtf8().constData(), 1);
    lines = new Mat();
    *lines = imread(im.toUtf8().constData(), 1);
    flood = new Mat();
    *flood = imread(im.toUtf8().constData(), 1);
    floodLines = new Mat();
    *floodLines = imread(im.toUtf8().constData(), 1);

    scene = new Scene(*src, *lines, *floodLines, *flood, *positions);
    setMouseTracking(true);
    ui->graphicsView->setScene(scene);
    displayImage((*src));


    color = new short[3];
    color[0] = 0;
    color[1] = 255;
    color[2] = 0;
    this->fcolor = new Scalar();
    *fcolor = Scalar(color[0], color[1], color[2]);

    scene->setColor(color);
    ui->progressBar->setValue(100);

}

FloodFill::~FloodFill()
{
    delete ui;
    delete scene;
    delete src;
    delete lines;
    delete flood;
    delete floodLines;
    delete positions;
}


void FloodFill::displayImage(const QString qs){
    scene->clear();
    scene->addPixmap(QPixmap(qs));
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}


void FloodFill::displayImage(Mat &m){
    QImage qi = mat_to_qimage_ref(m, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void FloodFill::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}

void FloodFill::on_pushButton_clicked()
{
    color[0] = short(rand() % 255);
    color[1] = short(rand() % 255);
    color[2] = short(rand() % 255);
    *fcolor = Scalar(color[0], color[1], color[2]);
    scene->setColor(color);

}


void FloodFill::on_pushButton_2_clicked()
{
    ui->progressBar->setValue(0);
    Mat msrc = *lines;
    Mat mflood = *flood;

    short vr = color[2];
    short vg = color[1];
    short vb = color[0];

    unsigned char *input = (unsigned char*)(msrc.data);
    int lo = ui->spinBox_lo->value();
    int up = ui->spinBox_up->value();
    int connectivity = 4;
    int newMaskVal = 255;
    int ffillMode = 1;
    int flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);
    Rect ccomp;
    int counter = 0;
    for(int j = 0;j < msrc.rows;j++){
        for(int i = 0;i < msrc.cols;i++){

            unsigned short b = input[(msrc.cols * j * 3) + i*3 ] ;
            unsigned short g = input[(msrc.cols * j * 3) + i*3 + 1];
            unsigned short r = input[(msrc.cols * j * 3) + i*3 + 2];

            if (r == vr and g == vg and b == vb){
                counter ++;
            }
        }
    }

    int nbr = counter;
    counter = 0;
    for(int j = 0;j < msrc.rows;j++){
        for(int i = 0;i < msrc.cols;i++){

            unsigned short b = input[(msrc.cols * j * 3) + i*3 ] ;
            unsigned short g = input[(msrc.cols * j * 3) + i*3 + 1];
            unsigned short r = input[(msrc.cols * j * 3) + i*3 + 2];

            if (r == vr and g == vg and b == vb){
                counter ++;
                if (counter % 10 == 0){
                    float value = ((float)counter/(float)nbr)*100;
                    ui->progressBar->setValue(int(value));
                    floodFill(mflood, Point(i,j), *fcolor, &ccomp, Scalar(lo, lo, lo), Scalar(up, up, up), flags);
                }
            }
        }
    }

    *lines = src->clone();
    *flood = mflood.clone();
    *floodLines = mflood.clone();
    displayImage((*floodLines));
    ui->progressBar->setValue(100);

}
