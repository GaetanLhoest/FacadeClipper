#include "view/zonesextractor.h"
#include "ui_zonesextractor.h"


#include "methods/asmopencv.h"
#include "methods/texture.h"
#include "methods/treatments.h"
#include "methods/perspective.h"
#include "methods/pw.h"

using namespace cv;
using namespace std;
using namespace ASM;

ZonesExtractor::ZonesExtractor(QString im, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZonesExtractor)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    this->im = im;

    src = new Mat();
    *src = imread(im.toUtf8().constData(), 1);
    imsrc = new Mat();
    *imsrc = imread(im.toUtf8().constData(), 1);

    tmpLines = new Mat();
    *tmpLines = imread(im.toUtf8().constData(), 1);
    tmpModified= new Mat();
    *tmpModified= imread(im.toUtf8().constData(), 1);
    modified= new Mat();
    *modified= imread(im.toUtf8().constData(), 1);

    positions= new vector<Point>;

    scene = new Scene(*src, *tmpLines, *tmpModified, *modified, *positions);

    ui->graphicsView->setScene(scene);

    displayImage(*src);

    color = new short[3];
    color[0] = 0;
    color[1] = 255;
    color[2] = 0;
    created = false;
    notdeleted = false;

    scene->setColor(color);
    scene->setGetPositions();

}

ZonesExtractor::~ZonesExtractor()
{
    delete ui;
    delete src;
    delete imsrc;
    delete tmpLines;
    delete tmpModified;
    delete modified;
    delete positions;
    delete scene;
    if (created){
        delete mean;
        delete meanRGB;
        delete stdev;
    }
    if (notdeleted){
        delete monoseed;
    }
    delete color;
}

void ZonesExtractor::displayImage(Mat &m){
    QImage qi = mat_to_qimage_ref(m, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void ZonesExtractor::displayImage8UC1(Mat &m){
    QImage qi = cvMatToQImage(m);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void ZonesExtractor::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}

void ZonesExtractor::on_texturePreviewButton_clicked()
{
    texturesMatching();
    displayImage(*modified);

    ui->resetButton->setEnabled(true);
    scene->setGetPositions();
}

void ZonesExtractor::texturesMatching()
{
    int w = src->cols;
    int h = src->rows;

    int sqs = ui->squareSizeBox->value();

    mean = new double[(w-(2*sqs))*(h-(2*sqs))];
    meanRGB = new double[(w-(2*sqs))*(h-(2*sqs))*3];
    stdev = new double[(w-(2*sqs))*(h-(2*sqs))];

    created = true;


    double meant, stdevt;
    double meantRGB[3];

    Mat gsrc;
    cvtColor(*src, gsrc, CV_BGR2GRAY);

    getLinesStatData(tmpLines, src, &gsrc, &meant, meantRGB, &stdevt);

    getSquaresStatData(src, &gsrc, sqs, mean, meanRGB, stdev);

    vector<int> positions;
    getMatchingPositions(meant, meantRGB, stdevt, ui->meanTresSpinBox->value(), ui->meanRGBSpinBox->value(), ui->stdTresSpinBox->value(), mean, meanRGB, stdev, w, h, sqs, &positions);

    setPositionsToGreen(modified, &positions);

}

void ZonesExtractor::on_resetButton_clicked()
{
    delete mean;
    delete meanRGB;
    delete stdev;
    delete scene;
    delete tmpLines;
    delete tmpModified;
    delete modified;
    delete positions;
    created = false;

    tmpLines = new Mat();
    *tmpLines = imread(im.toUtf8().constData(), 1);
    tmpModified= new Mat();
    *tmpModified= imread(im.toUtf8().constData(), 1);
    modified= new Mat();
    *modified= imread(im.toUtf8().constData(), 1);

    positions= new vector<Point>;

    scene = new Scene(*src, *tmpLines, *tmpModified, *modified, *positions);

    ui->graphicsView->setScene(scene);


    color = new short[3];
    color[0] = 0;
    color[1] = 255;
    color[2] = 0;
    scene->setColor(color);

    displayImage(*src);

    ui->resetButton->setEnabled(false);
}

void ZonesExtractor::on_floodPreviewButton_clicked()
{
    Mat *interm = new Mat();
    *interm = src->clone();
    floodfillGreenPixels(src, interm, modified, ui->flospinBox->value(), ui->fupspinBox->value());
    *modified = interm->clone();
    displayImage(*modified);
    delete interm;
}



void ZonesExtractor::on_cannyFloodPreviewButton_clicked()
{
    Mat canny_m;
    cvtColor(*imsrc, canny_m, COLOR_BGR2GRAY);
    blur(canny_m, canny_m, Size(3,3));
    cv::Canny(canny_m, canny_m, 0, 0, 3);
    cvtColor(canny_m, canny_m, COLOR_GRAY2BGR);

    Mat canny_p;
    cvtColor(*imsrc, canny_p, COLOR_BGR2GRAY);
    cv::Canny(canny_p, canny_p, 0, 0, 3);
    cvtColor(canny_p, canny_p, COLOR_GRAY2BGR);

    unsigned char *input = (unsigned char*)(modified->data);
    unsigned char *output = (unsigned char*)(canny_m.data);
    unsigned char *output2 = (unsigned char*)(canny_p.data);

    Scalar fcolor = Scalar(0, 255, 0);
    Rect ccomp;
    int connectivity = 4;
    int newMaskVal = 255;
    int ffillMode = 1;
    int flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);


    for (int j = 0; j < canny_m.rows; j++){
        for (int i = 0; i < canny_m.cols; i++){

            unsigned short cb = output[(canny_m.cols * j * 3) + i*3 ];
            unsigned short cg = output[(canny_m.cols * j * 3) + i*3 + 1];
            unsigned short cr = output[(canny_m.cols * j * 3) + i*3 + 2];

            if (cb > 125 and cg > 125 and cr > 125) {
                output2[(canny_p.cols * j * 3) + i*3 ] = 255;
                output2[(canny_p.cols * j * 3) + i*3 + 1] = 255;
                output2[(canny_p.cols * j * 3) + i*3 + 2] = 255;
            }
        }
    }


    for (int j = 0; j < modified->rows; j++){
        for (int i = 0; i < modified->cols; i++){
            unsigned short b = input[(modified->cols * j * 3) + i*3 ];
            unsigned short g = input[(modified->cols * j * 3) + i*3 + 1];
            unsigned short r = input[(modified->cols * j * 3) + i*3 + 2];

            unsigned short cb = output2[(canny_p.cols * j * 3) + i*3 ];
            unsigned short cg = output2[(canny_p.cols * j * 3) + i*3 + 1];
            unsigned short cr = output2[(canny_p.cols * j * 3) + i*3 + 2];

            if (cr < 20 and cg < 20 and cb < 20){
                if (r == 0 and g == 255 and b == 0) {
                    floodFill(canny_p, Point(i,j), fcolor, &ccomp, Scalar(10, 10, 10), Scalar(10, 10, 10), flags);
                }
            }
        }
    }

    for (int j = 0; j < modified->rows; j++){
        for (int i = 0; i < modified->cols; i++){

            unsigned short cb = output2[(canny_p.cols * j * 3) + i*3 ];
            unsigned short cg = output2[(canny_p.cols * j * 3) + i*3 + 1];
            unsigned short cr = output2[(canny_p.cols * j * 3) + i*3 + 2];

            if (cr == 0 and cg == 255 and cb == 0){
                input[(modified->cols * j * 3) + i*3 ] = 0;
                input[(modified->cols * j * 3) + i*3 + 1] = 255;
                input[(modified->cols * j * 3) + i*3 + 2] = 0;
            }
        }
    }

    displayImage(*modified);

}

void ZonesExtractor::on_prespectiveAndCuttingButton_clicked()
{
    Mat edges;
    vector<Vec4i> lines;



    edges = mhProbabilisticv(src, 200, 40, lines);


    vector<Vec2i> extr;
    int w = edges.cols;
    int h = edges.rows;

    getExtremitiesIntersections(lines, w, h, extr);

    vector<Vec4i> vectLines;

    Mat fullLines = src->clone();
    for( size_t i = 0; i < extr.size(); i+=2 )
    {
        Vec4i e(extr[i][0], extr[i][1], extr[i+1][0], extr[i+1][1]);

        if (isHOrV(e) == VERTICAL){
            vectLines.push_back(e);
        }
    }


    vector<Vec2i> vanPoint;
    double counter = 0;

    for (size_t i = 0; i < vectLines.size(); i++){
        for (size_t j = i + 1; j < vectLines.size(); j++){
            vanPoint.push_back(findIntersection(vectLines[i], vectLines[j]));
            counter += 1;
        }
    }

    Vec2d vaniPoint = Vec2d(0,0);
    for (size_t i = 0; i < vanPoint.size(); i++){
        vaniPoint[0] += (float)vanPoint[i][0] / counter;
        vaniPoint[1] += (float)vanPoint[i][1] / counter;
    }

    vector<Point> clicked = scene->getClickedPos();

    Point p1, p2, va;

    p1 = clicked[clicked.size() - 1];
    p2 = clicked[clicked.size() - 2];
    va.x = vaniPoint[0];
    va.y = vaniPoint[1];

    vector<Vec4i> newBorders;
    Vec4i border;
    border[0] = p1.x;
    border[1] = p1.y;
    border[2] = va.x;
    border[3] = va.y;

    newBorders.push_back(border);

    border[0] = p2.x;
    border[1] = p2.y;

    newBorders.push_back(border);

    vector<Vec2i> extr2;
    getExtremitiesIntersections(newBorders, w, h, extr2);

    Point left, right;
    if (p1.x < p2.x){
        left = p1;
        right = p2;
    }
    else {
        left = p2;
        right = p1;
    }

    Mat mask = Mat::zeros(src->size(), src->type());
    int ind = extr2.size() - 2;
    Vec4i e(extr2[ind][0], extr2[ind][1], extr2[ind+1][0], extr2[ind+1][1]);
    line( mask, Point(e[0], e[1]), Point(e[2], e[3]), Scalar(0,0,255), 10, CV_AA);
    ind = extr2.size() - 4;
    Vec4i f(extr2[ind][0], extr2[ind][1], extr2[ind+1][0], extr2[ind+1][1]);
    line( mask, Point(f[0], f[1]), Point(f[2], f[3]), Scalar(0,0,255), 10, CV_AA);

    Point flleft, flright;
    flleft.x = left.x / 2;
    flleft.y = left.y;
    flright.x = ((mask.cols - right.x) / 2) + right.x;
    flright.y = right.y;

    Scalar fcolor = Scalar(255, 255, 255);
    Rect ccomp;
    int connectivity = 4;
    int newMaskVal = 255;
    int ffillMode = 1;
    int flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);
    floodFill(mask, flleft, fcolor, &ccomp, Scalar(0, 0, 0), Scalar(0, 0, 0), flags);
    floodFill(mask, flright, fcolor, &ccomp, Scalar(0, 0, 0), Scalar(0, 0, 0), flags);


    unsigned char *input = (unsigned char*)(mask.data);
    unsigned char *output = (unsigned char*)(src->data);


    for (int j = 0; j < src->rows; j++){
        for (int i = 0; i < src->cols; i++){
            unsigned short b = input[(mask.cols * j * 3) + i*3 ];
            unsigned short g = input[(mask.cols * j * 3) + i*3 + 1];
            unsigned short r = input[(mask.cols * j * 3) + i*3 + 2];

            if (r == 255 and g == 255 and b == 255){
                output[(src->cols * j * 3) + i*3 ] = 255 ;
                output[(src->cols * j * 3) + i*3 + 1] = 255;
                output[(src->cols * j * 3) + i*3 + 2] = 255;
            }
        }
    }


    scene->unsetGetPositions();

    *tmpLines = src->clone();
    *tmpModified= src->clone();
    *modified= src->clone();

    displayImage(*src);
}


void ZonesExtractor::on_fillHolesButton_clicked()
{
    Mat zones = modified->clone();
    int sqsize = 5;

    unsigned char *input = (unsigned char*)(zones.data);
    int w = zones.cols;

    for (int j = 0; j < zones.rows; j++){
        for (int i = 0; i < zones.cols; i++){

            unsigned short b = input[(zones.cols * j * 3) + i*3 ];
            unsigned short g = input[(zones.cols * j * 3) + i*3 + 1];
            unsigned short r = input[(zones.cols * j * 3) + i*3 + 2];

            if (!(r == 0 and g == 255 and b == 0)){
                input[(zones.cols * j * 3) + i*3 ] = 0;
                input[(zones.cols * j * 3) + i*3 + 1] = 0;
                input[(zones.cols * j * 3) + i*3 + 2] = 0;
            }
        }
    }

    int racc, gacc, bacc;
    int index;
    Scalar fcolor = Scalar(255, 255, 255);
    Rect ccomp;
    int connectivity = 4;
    int newMaskVal = 255;
    int ffillMode = 1;
    int flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);

    for (int i = sqsize; i < zones.rows - sqsize; i++){
        for (int j = sqsize; j < zones.cols - sqsize; j++){

            racc = 0;
            gacc = 0;
            bacc = 0;

            for (int k = i-sqsize; k <= i+sqsize; k++){
                for (int l = j-sqsize; l <= j+sqsize; l++){
                    index = (k*3*w) + (3*l);
                    unsigned short b = input[index + 0];
                    unsigned short g = input[index + 1];
                    unsigned short r = input[index + 2];

                    racc += r;
                    gacc += g;
                    bacc += b;

                }
            }

            if (racc == 0 and gacc == 0 and bacc == 0){
                floodFill(zones, Point(i,j), fcolor, &ccomp, Scalar(0, 0, 0), Scalar(0, 0, 0), flags);

            }

        }
    }

    unsigned char *output = (unsigned char*)(modified->data);

    for (int j = 0; j < zones.rows; j++){
        for (int i = 0; i < zones.cols; i++){

            unsigned short b = input[(zones.cols * j * 3) + i*3 ];
            unsigned short g = input[(zones.cols * j * 3) + i*3 + 1];
            unsigned short r = input[(zones.cols * j * 3) + i*3 + 2];

            if (!(r == 255 and g == 255 and b == 255)){
                output[(zones.cols * j * 3) + i*3 ] = 0;
                output[(zones.cols * j * 3) + i*3 + 1] = 255;
                output[(zones.cols * j * 3) + i*3 + 2] = 0;
            }
        }
    }

    displayImage(*modified);
}

void ZonesExtractor::on_pushButton_clicked()
{
    scene->unsetGetPositions();
}

void ZonesExtractor::on_powerWatershedMaskpushButton_clicked()
{
    monoseed = new Mat(src->rows, src->cols, CV_8UC1, Scalar(125));
    notdeleted = true;

    findPWMask(modified, monoseed);
    displayImage8UC1(*monoseed);

}

void ZonesExtractor::on_powerWatershedpushButton_clicked()
{
    if (!notdeleted){
         monoseed = new Mat(src->rows, src->cols, CV_8UC1, Scalar(125));
         notdeleted = true;
         findPWMaskThreaded(modified, monoseed);
    }
    unsigned char * output;
    output = applyPowerWatershed(*src, *monoseed, 0);


    delete monoseed;
    notdeleted = false;
    Mat outputm(Size(src->cols, src->rows), CV_8UC1, output, Mat::AUTO_STEP);
    displayImage8UC1(outputm);
}
