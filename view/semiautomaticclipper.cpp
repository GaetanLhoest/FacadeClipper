#include "view/semiautomaticclipper.h"
#include "ui_semiautomaticclipper.h"

#include "methods/asmopencv.h"
#include "methods/perspective.h"
#include "methods/treatments.h"
#include "methods/texture.h"
#include "methods/pw.h"

#include <string>

#include <time.h>

using namespace std;
using namespace cv;
using namespace ASM;

SemiAutomaticClipper::SemiAutomaticClipper(QString im, string *directory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SemiAutomaticClipper)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    this->im = im;

    src = new Mat();
    *src = imread(im.toUtf8().constData(), 1);
    imsrc = new Mat();
    *imsrc = imread(im.toUtf8().constData(), 1);
    tmpModified= new Mat();
    *tmpModified= imread(im.toUtf8().constData(), 1);
    modified= new Mat();
    *modified= imread(im.toUtf8().constData(), 1);

    positions= new vector<Point>;

    savedirectory = directory;

    scene = new scene2(*tmpModified);

    ui->graphicsView->setScene(scene);
    displayImage(*src);
    short color[3] = {0,255,0};

    ui->resetButton->setEnabled(false);
    ui->saveButton->setEnabled(false);



    scene->setColor(color);
    scene->setGetPositions();
}

SemiAutomaticClipper::~SemiAutomaticClipper()
{
    delete ui;
}

void SemiAutomaticClipper::displayImage(Mat &m){
    QImage qi = mat_to_qimage_ref(m, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void SemiAutomaticClipper::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}

void SemiAutomaticClipper::on_applyButton_clicked()
{
// Texture search
    int sqs = ui->texSqSizespinBox->value();
    int meanT = ui->texMeanTresspinBox->value();
    int meanRGBT = ui->texMeanRGBTresspinBox->value();
    int stdT = ui->texStdTresspinBox->value();

    clock_t startTime = clock();

    getSameTextureAsGreenZone(src, tmpModified, sqs, meanT, meanRGBT, stdT);
    cout << "Same textures " << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;

// Floodfill the texture points
    int lo = ui->floodLoTresspinBox->value();
    int up = ui->floodUpTresspinBox->value();

    floodfillGreenPixelsThreaded(src, tmpModified, lo, up);
    cout << "Floodfills " << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;

// Fill holes
    int sqsHoles = ui->fillHolesSqSizespinBox->value();

    fillHolesThreaded(tmpModified, sqsHoles);
    cout << "Fill holes " << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;


// Power watershed with the "Fill holes" results
    Mat *monoseed = new Mat(src->rows, src->cols, CV_8UC1, Scalar(125));
    findPWMaskThreaded(tmpModified, monoseed);
    unsigned char * output;
    output = applyPowerWatershed(*src, *monoseed, 0);
    Mat *outputm = new Mat(Size(src->cols, src->rows), CV_8UC1, output, Mat::AUTO_STEP);
    cout << "Power watershed " << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;

// Canny flooding with the "Fill holes" results
    Mat *cannyflo = new Mat();
    *cannyflo = tmpModified->clone();
    cannyFloodBorderThreaded(imsrc, cannyflo, monoseed);
    cout << "Canny flood " << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;


// Combine power watershed, canny flooding results and "Fill holes" results
    combineResults(tmpModified, cannyflo, outputm);
    cout << "Combining results " << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;

// Fill holes for the second time
    sqsHoles = ui->fillHolesSqSizespinBox->value();
    fillHolesThreaded(tmpModified, sqsHoles);
    cout << "Fill holes " << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;


    delete monoseed;
    delete cannyflo;
    delete output;
    displayImage(*tmpModified);
    ui->saveButton->setEnabled(true);
}

void SemiAutomaticClipper::on_cutButton_clicked()
{


    vector<Point> clicked = scene->getClickedPos();

    verticalCut2Points(src, clicked[clicked.size() - 1], clicked[clicked.size() - 2]);


    scene->unsetGetPositions();
    *tmpModified = src->clone();
    displayImage(*src);
    ui->cutButton->setEnabled(false);
    ui->doNotCutButton->setEnabled(false);
    ui->resetButton->setEnabled(true);
    ui->applyButton->setEnabled(true);
}

void SemiAutomaticClipper::on_doNotCutButton_clicked()
{
    ui->cutButton->setEnabled(false);
    ui->doNotCutButton->setEnabled(false);
    ui->applyButton->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->resetButton->setEnabled(true);

    scene->unsetGetPositions();
}

void SemiAutomaticClipper::on_resetButton_clicked()
{
    *src = imread(im.toUtf8().constData(), 1);
    *imsrc = imread(im.toUtf8().constData(), 1);
    *tmpModified = imread(im.toUtf8().constData(), 1);
    *modified = imread(im.toUtf8().constData(), 1);

    scene->setGetPositions();

    ui->resetButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->applyButton->setEnabled(false);
    ui->cutButton->setEnabled(true);
    ui->doNotCutButton->setEnabled(true);

    displayImage(*src);
}

void SemiAutomaticClipper::on_saveButton_clicked()
{

    getDefinedZones(imsrc, tmpModified, modified);
    string name = im.toUtf8().constData();
    string sdirectory = *savedirectory;
    string str;
    size_t pos = name.find_last_of("/");
    size_t pos2 = name.find_last_of(".");

    name = str.append(name.begin()+pos+1, name.begin()+pos2);
    //pos = name.find_last_of(".");
    //name = str.append(name.begin(), name.begin()+pos);

    if (sdirectory[sdirectory.length()-1] != '/')
        sdirectory = sdirectory + "/";
    string fullname = sdirectory + "treated_" + name + ".png";
    imwrite(fullname, *modified);
}
