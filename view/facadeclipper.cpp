#include "facadeclipper.h"
#include "ui_Facadeclipper.h"

#include <iostream>
#include <QString>
#include <QFileDialog>
#include <unistd.h>
#include <pwd.h>

#include "floodfill.h"
#include "hough.h"
#include "canny.h"
#include "perspectivecorrection.h"
#include "powerwatershed.h"
#include "texturesearchengine.h"
#include "zonesextractor.h"
#include "semiautomaticclipper.h"
#include "settings.h"
#include "serialworker.h"

using namespace std;

FacadeClipper::FacadeClipper(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FacadeClipper)
{
    ui->setupUi(this);
    model = new QStringListModel(this);
    namelist = new QStringList();
    currentName = new QString();

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    const char *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    this->savedirectory = new std::string(homedir);
}

FacadeClipper::~FacadeClipper()
{
    delete ui;
    delete model;
    delete namelist;
    delete currentName;
    delete scene;
    delete savedirectory;
}

void FacadeClipper::on_mainApplyButton_clicked()
{
    QString method = ui->methodsList->currentText();
    std::string utf8_text = method.toUtf8().constData();

    if (utf8_text == "FloodFill"){
        FloodFill w(*currentName);
        w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
        w.exec();
    }
    else if (utf8_text == "Hough"){
        Hough w(*currentName);
        w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
        w.exec();
    }
    else if (utf8_text == "Canny"){
        Canny w(*currentName);
        w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
        w.exec();
    }
    else if (utf8_text == "Perspective Correction"){
        PerspectiveCorrection w(*currentName);
        w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
        w.exec();
    }
    else if (utf8_text == "Power Watershed"){
        PowerWatershed w(*currentName);
        w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
        w.exec();
    }
    else if (utf8_text == "Same texture search engine"){
        TextureSearchEngine w(*currentName);
        w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
        w.exec();
    }
    else if (utf8_text == "Zones extractor"){
        ZonesExtractor w(*currentName);
        w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
        w.exec();
    }
    else if (utf8_text == "Semi Automatic Clipper"){
        SemiAutomaticClipper w(*currentName, savedirectory);
        w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
        w.exec();
    }
    else if (utf8_text == "Serial Worker"){
        SerialWorker w(*namelist, savedirectory);
        w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
        w.exec();
    }
    else{
        ui->textEdit->setTextColor(Qt::red);
        ui->textEdit->insertPlainText(QString::fromStdString(utf8_text));
        ui->textEdit->insertPlainText(" is not implemented yet.\n");
        ui->textEdit->setTextColor(Qt::black);
    }
}

void FacadeClipper::on_actionOuvrir_triggered()
{
    QStringList fnames = QFileDialog::getOpenFileNames(this, "Select one or more files to open", "~/", "Images (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG)");
    namelist->append(fnames);

    // Displaying the current operations in the notification area
    ui->textEdit->setTextColor(Qt::green);
    ui->textEdit->insertPlainText("Opening:\n");
    ui->textEdit->setTextColor(Qt::black);

    for (int i = 0; i < fnames.size(); i++){
        ui->textEdit->insertPlainText(fnames.at(i).toLocal8Bit().constData());
        ui->textEdit->insertPlainText("\n");
    }

    // Updating the list of opened elements
    model->setStringList(*namelist);
    ui->listView->setModel(model);
    ui->listView->setEditTriggers(QAbstractItemView::AnyKeyPressed |QAbstractItemView::DoubleClicked);
    ui->listView->show();

    // Showing the first picture in the graphic area
    QString qs = model->index(0, 0).data().toString();
    *currentName = qs;
    displayImage(qs);

}


void FacadeClipper::on_listView_clicked(const QModelIndex &index)
{
    QString qs = model->index(index.row(), index.column()).data().toString();
    *currentName = qs;
    displayImage(qs);

}

void FacadeClipper::on_listView_pressed(const QModelIndex &index)
{
    QString qs = model->index(index.row(), index.column()).data().toString();
    *currentName = qs;
    displayImage(qs);
}

void FacadeClipper::displayImage(const QString qs){
    scene->clear();
    scene->addPixmap(QPixmap(qs));
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}

void FacadeClipper::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}



void FacadeClipper::on_actionSettings_triggered()
{
    Settings w(savedirectory);
    w.setWindowIcon(QIcon("../tfe-image-processing/icons/homeico522.png"));
    w.exec();
}

void FacadeClipper::setSaveDirectory(std::string directory)
{
    *savedirectory = directory;
}

