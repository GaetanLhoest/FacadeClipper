#include "texturesearchengine.h"
#include "ui_texturesearchengine.h"

#include "methods/texture.h"
#include "methods/asmopencv.h"

using namespace cv;
using namespace std;
using namespace ASM;

TextureSearchEngine::TextureSearchEngine(QString im, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureSearchEngine)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    this->im = im;

    created = false;

    src = new Mat();
    *src = imread(im.toUtf8().constData(), 1);

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
}

TextureSearchEngine::~TextureSearchEngine()
{
    delete ui;
    delete scene;
    delete src;
    delete tmpLines;
    delete tmpModified;
    delete modified;
    delete positions;
    if (created){
        delete mean;
        delete meanRGB;
        delete stdev;
    }
    delete color;

}

void TextureSearchEngine::displayImage(Mat &m){
    QImage qi = mat_to_qimage_ref(m, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void TextureSearchEngine::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}

void TextureSearchEngine::on_applyButton_clicked()
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
    displayImage(*modified);

    ui->resetButton->setEnabled(true);
    ui->applyButton->setEnabled(false);
}

void TextureSearchEngine::on_resetButton_clicked()
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
    ui->applyButton->setEnabled(true);
}
