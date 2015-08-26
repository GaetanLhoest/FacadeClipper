#include "view/serialworker.h"
#include "ui_serialworker.h"

#include "methods/asmopencv.h"
#include "methods/perspective.h"
#include "methods/treatments.h"
#include "methods/texture.h"
#include "methods/pw.h"

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>

using namespace std;
using namespace cv;
using namespace ASM;

SerialWorker::SerialWorker(QStringList namelist, string *directory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialWorker)
{
    ui->setupUi(this);
    src = new Mat();
    imsrc = new Mat();
    tmpModified= new Mat();
    modified= new Mat();

    if (namelist.size() != 0){
        *src = imread(namelist.at(0).toUtf8().constData(), 1);
        *imsrc = imread(namelist.at(0).toUtf8().constData(), 1);
        *tmpModified= imread(namelist.at(0).toUtf8().constData(), 1);
        *modified= imread(namelist.at(0).toUtf8().constData(), 1);
    }
    nameListIndex = 0;
    savedirectory = directory;
    positions= new vector<Point>;

    scene = new scene2(*tmpModified);
    ui->graphicsView->setScene(scene);

    displayImage(*src);
    short color[3] = {0,255,0};

    scene->setColor(color);
    scene->setGetPositions();
    this->setWindowState(Qt::WindowMaximized);
    this->nameList = namelist;
    this->savedirectory = directory;
    checkAndCreateTmpDir();
    formatSaveDirectory();
    ui->applyButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
}

SerialWorker::~SerialWorker()
{
    delete ui;
    delete scene;
    delete src;
    delete imsrc;
    delete tmpModified;
    delete modified;
    delete positions;
}

void SerialWorker::displayImage(Mat &m){
    QImage qi = mat_to_qimage_ref(m, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void SerialWorker::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}

void SerialWorker::on_cutButton_clicked()
{
    vector<Point> clicked = scene->getClickedPos();

    if (clicked.size() > 2)
        verticalCut2Points(src, clicked[clicked.size() - 1], clicked[clicked.size() - 2]);

    string tmpSrcName = getTmpSrcName();
    imwrite(tmpSrcName, *src);

    scene->unsetGetPositions();
    *tmpModified = src->clone();
    displayImage(*src);
    ui->cutButton->setEnabled(false);
    ui->doNotCutButton->setEnabled(false);
    ui->resetButton->setEnabled(true);

}

void SerialWorker::on_doNotCutButton_clicked()
{
    ui->cutButton->setEnabled(false);
    ui->doNotCutButton->setEnabled(false);
    ui->resetButton->setEnabled(true);
    string tmpSrcName = getTmpSrcName();
    imwrite(tmpSrcName, *src);

    scene->unsetGetPositions();
}

void SerialWorker::on_resetButton_clicked()
{
    if (nameList.size() != 0){
        *src = imread(nameList.at(nameListIndex).toUtf8().constData(), 1);
        *tmpModified = imread(nameList.at(nameListIndex).toUtf8().constData(), 1);
    }
    scene->setGetPositions();

    ui->resetButton->setEnabled(false);
    ui->applyButton->setEnabled(false);
    ui->cutButton->setEnabled(true);
    ui->doNotCutButton->setEnabled(true);

    displayImage(*src);
}

void SerialWorker::on_nextButton_clicked()
{
    string tmpName = getTmpName();
    imwrite(tmpName, *tmpModified);


    if (nameListIndex < nameList.size() - 1){
        nameListIndex++;
        *src = imread(nameList.at(nameListIndex).toUtf8().constData(), 1);
        *tmpModified= imread(nameList.at(nameListIndex).toUtf8().constData(), 1);
        displayImage(*tmpModified);
        ui->resetButton->setEnabled(false);
        ui->cutButton->setEnabled(true);
        ui->doNotCutButton->setEnabled(true);
    }
    else {
        ui->cutButton->setEnabled(false);
        ui->doNotCutButton->setEnabled(false);
        ui->applyButton->setEnabled(true);
        ui->nextButton->setEnabled(false);
        ui->resetButton->setEnabled(false);
    }

}

string SerialWorker::getTmpName()
{
    string name = nameList.at(nameListIndex).toUtf8().constData();
    size_t pos = name.find_last_of("/");
    size_t pos2 = name.find_last_of(".");
    string str;
    name = str.append(name.begin()+pos+1, name.begin()+pos2);
    name = name + ".png";

    cout << "Get tmp Name: " << tmpDir + "/tmp_" + name << endl;

    return tmpDir + "/tmp_" + name;
}

string SerialWorker::getTmpSrcName()
{
    string name = nameList.at(nameListIndex).toUtf8().constData();
    size_t pos = name.find_last_of("/");
    size_t pos2 = name.find_last_of(".");
    string str;
    name = str.append(name.begin()+pos+1, name.begin()+pos2);
    name = name + ".png";

    cout << "Get tmp source name: " << tmpDir + "/tmp_src_" + name << endl;

    return tmpDir + "/tmp_src_" + name;
}

string SerialWorker::getSaveName()
{
    string name = nameList.at(nameListIndex).toUtf8().constData();
    size_t pos = name.find_last_of("/");
    size_t pos2 = name.find_last_of(".");
    string str;
    name = str.append(name.begin()+pos+1, name.begin()+pos2);
    name = name + ".png";

    string sdirectory = *savedirectory;
    if (sdirectory[sdirectory.length()-1] != '/')
        sdirectory = sdirectory + "/";

    cout << "Get save name: " << sdirectory + "treated_" + name << endl;

    return sdirectory + "treated_" + name;
}

void SerialWorker::formatSaveDirectory()
{
    string sdirectory = *savedirectory;
    if (sdirectory[sdirectory.length()-1] != '/')
        sdirectory = sdirectory + "/";
    *savedirectory = sdirectory;
}

void SerialWorker::checkAndCreateTmpDir(){
    const char *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    string dir = string(homedir);
    if (dir[dir.length()-1] != '/')
        dir = dir + "/";

    dir = dir + ".fctmp";

    struct stat info;
    if(!( stat( dir.c_str(), &info ) != 0) and !(info.st_mode & S_IFDIR)){
        const int dir_err = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_err)
        {
            printf("Error creating directory ~/.fctmp !\n");
            exit(1);
        }
    }
    tmpDir = dir;
}

void SerialWorker::on_applyButton_clicked()
{
    string str1, str2;
    int sqs = 5;
    int meanT = 4;
    int meanRGBT = 4;
    int stdT = 4;

    int lo = 20;
    int up = 20;

    int sqsHoles = 5;

    for (nameListIndex = 0; nameListIndex < nameList.size(); nameListIndex++){
        str1 = getTmpSrcName();
        *src = imread(str1, 1);
        str2 = getTmpName();
        *tmpModified = imread(str2, 1);

        *imsrc = imread(nameList.at(nameListIndex).toUtf8().constData());

        getSameTextureAsGreenZone(src, tmpModified, sqs, meanT, meanRGBT, stdT);
        floodfillGreenPixelsThreaded(src, tmpModified, lo, up);

        fillHolesThreaded(tmpModified, sqsHoles);

        Mat *monoseed = new Mat(src->rows, src->cols, CV_8UC1, Scalar(125));
        findPWMaskThreaded(tmpModified, monoseed);
        unsigned char * output;
        output = applyPowerWatershed(*src, *monoseed, 0);
        Mat *outputm = new Mat(Size(src->cols, src->rows), CV_8UC1, output, Mat::AUTO_STEP);

        Mat *cannyflo = new Mat();
        *cannyflo = tmpModified->clone();
        cannyFloodBorderThreaded(imsrc, cannyflo, monoseed);

        combineResults(tmpModified, cannyflo, outputm);

        fillHolesThreaded(tmpModified, sqsHoles);

        delete monoseed;
        delete cannyflo;
        delete output;
        delete outputm;

        displayImage(*tmpModified);

        saveResults();
        remove(str1.c_str());
        remove(str2.c_str());

    }
    cout << "finished" << endl;
}

void SerialWorker::saveResults()
{
    getDefinedZones(imsrc, tmpModified, modified);

    string fullname = getSaveName();
    imwrite(fullname, *modified);
}
