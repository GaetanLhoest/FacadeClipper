#include "powerwatershed.h"
#include "ui_powerwatershed.h"

#include "methods/asmopencv.h"
#include "methods/pw.h"


using namespace std;
using namespace cv;
using namespace ASM;

PowerWatershed::PowerWatershed(QString im,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerWatershed)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);

    ui->objectButton->setEnabled(false);
    ui->maskButton->setEnabled(false);



    src = new Mat();
    *src = imread(im.toUtf8().constData(), 1);

    tmpLinesMul = new Mat();
    *tmpLinesMul = imread(im.toUtf8().constData(), 1);
    tmpModifiedMul = new Mat();
    *tmpModifiedMul = imread(im.toUtf8().constData(), 1);
    modifiedMul = new Mat();
    *modifiedMul = imread(im.toUtf8().constData(), 1);

    tmpLinesMono = new Mat();
    *tmpLinesMono = imread(im.toUtf8().constData(), 1);
    tmpModifiedMono = new Mat();
    *tmpModifiedMono = imread(im.toUtf8().constData(), 1);
    modifiedMono = new Mat();
    *modifiedMono = imread(im.toUtf8().constData(), 1);

    monoseed = new Mat(src->rows, src->cols, CV_8UC1, Scalar(125));
    multiseeds = new Mat(src->rows, src->cols, CV_8UC1, Scalar(0));

    museedvalue = 1;
    moseedvalue = 255;

    positionsmul = new vector<Point>;
    scenemul = new Scene(*src, *tmpLinesMul, *tmpModifiedMul, *modifiedMul, *positionsmul);

    color = new short[3];
    color[0] = 0;
    color[1] = 255;
    color[2] = 0;
    scenemul->setColor(color);


    setMouseTracking(true);

    ui->graphicsView->setScene(scenemul);

    positionsmono = new vector<Point>;
    scenemono = new Scene(*src, *tmpLinesMono, *tmpModifiedMono, *modifiedMono, *positionsmono);

    color[0] = 255;
    color[1] = 255;
    color[2] = 255;
    scenemono->setColor(color);

    scene = scenemono;
    displayImage(*src);


    scene = scenemul;
    displayImage(*src);




}

PowerWatershed::~PowerWatershed()
{
    delete ui;
    delete src;
    delete tmpLinesMul;
    delete tmpModifiedMul;
    delete modifiedMul;
    delete scenemul;
    delete tmpLinesMono;
    delete tmpModifiedMono;
    delete modifiedMono;
    delete scenemono;
    delete positionsmul;
    delete positionsmono;
    delete monoseed;
    delete multiseeds;
}

void PowerWatershed::displayImage(Mat &m){
    QImage qi = mat_to_qimage_ref(m, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void PowerWatershed::displayImage8UC1(Mat &m){
    QImage qi = cvMatToQImage(m);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void PowerWatershed::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}

void PowerWatershed::on_applyButton_clicked()
{


    unsigned char * output;


    if (ui->mosRadio->isChecked()){
        while (!positionsmono->empty()){
            Point s = positionsmono->back();
            positionsmono->pop_back();
            Point e = positionsmono->back();
            positionsmono->pop_back();
            line( *monoseed, s, e, moseedvalue, 10, CV_8UC1);
        }
        output = applyPowerWatershed(*src, *monoseed, 0);
    }
    else{
        while (!positionsmul->empty()){
            Point s = positionsmul->back();
            positionsmul->pop_back();
            Point e = positionsmul->back();
            positionsmul->pop_back();
            line( *multiseeds, s, e, museedvalue, 10, CV_8UC1);
        }
        output = applyPowerWatershed(*src, *multiseeds, 1);
    }

    Mat outputm(Size(src->cols, src->rows), CV_8UC1, output, Mat::AUTO_STEP);
    displayImage8UC1(outputm);

}

void PowerWatershed::on_musRadio_clicked()
{
    ui->objectButton->setEnabled(false);
    ui->maskButton->setEnabled(false);

    ui->addSButton->setEnabled(true);

    scene = scenemul;
    ui->graphicsView->setScene(scene);

}

void PowerWatershed::on_mosRadio_clicked()
{
    ui->objectButton->setEnabled(true);
    ui->maskButton->setEnabled(true);

    ui->addSButton->setEnabled(false);

    scene = scenemono;
    ui->graphicsView->setScene(scene);

}

void PowerWatershed::on_addSButton_clicked()
{
    while (!positionsmul->empty()){
        Point s = positionsmul->back();
        positionsmul->pop_back();
        Point e = positionsmul->back();
        positionsmul->pop_back();
        line( *multiseeds, s, e, museedvalue, 10, CV_8UC1);
    }
    color[0] = short(rand() % 255);
    color[1] = short(rand() % 255);
    color[2] = short(rand() % 255);
    scenemul->setColor(color);
    museedvalue += 1;

}

void PowerWatershed::on_objectButton_clicked()
{
    while (!positionsmono->empty()){
        Point s = positionsmono->back();
        positionsmono->pop_back();
        Point e = positionsmono->back();
        positionsmono->pop_back();
        line( *monoseed, s, e, moseedvalue, 10, CV_8UC1);
    }
    color[0] = 255;
    color[1] = 255;
    color[2] = 255;
    scenemono->setColor(color);
    moseedvalue = 255;
}

void PowerWatershed::on_maskButton_clicked()
{
    while (!positionsmono->empty()){
        Point s = positionsmono->back();
        positionsmono->pop_back();
        Point e = positionsmono->back();
        positionsmono->pop_back();
        line( *monoseed, s, e, moseedvalue, 10, CV_8UC1);
    }
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    scenemono->setColor(color);
    moseedvalue = 1;
}
