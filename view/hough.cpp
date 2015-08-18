#include "methods/asmopencv.h"
#include "view/hough.h"
#include "ui_Hough.h"

#include <QImage>


using namespace cv;

Hough::Hough(QString im, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Hough)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    this->scene = new QGraphicsScene;
    ui->graphicsView->setScene(this->scene);

    this->src = new Mat();
    *src = imread(im.toUtf8().constData(), 1);

    displayImage(*src);

}

Hough::~Hough()
{
    delete ui;
    delete src;
}

void Hough::on_pushButton_clicked()
{
    if (ui->radioButton_s->isChecked()){
        hStandard();
    }
    else{
        hProbabilistic();
    }
}

void Hough::hStandard(){
    Mat gray;
    cvtColor(*src, gray, COLOR_BGR2GRAY);
    Mat edges;
    Canny(gray, edges, 98, 248);

    vector<Vec2f> lines;
    HoughLines(edges, lines, 1, CV_PI/180, ui->spinBox_st->value());

    Mat hough = src->clone();
    for( size_t i = 0; i < lines.size(); i++ ){
       float rho = lines[i][0], theta = lines[i][1];
       Point pt1, pt2;
       double a = cos(theta), b = sin(theta);
       double x0 = a*rho, y0 = b*rho;
       pt1.x = cvRound(x0 + 1000*(-b));
       pt1.y = cvRound(y0 + 1000*(a));
       pt2.x = cvRound(x0 - 1000*(-b));
       pt2.y = cvRound(y0 - 1000*(a));
       line( hough, pt1, pt2, Scalar(0,0,255), 10, CV_AA);
    }
    displayImage(hough);

}

void Hough::hProbabilistic(){
    Mat gray;
    cvtColor(*src, gray, COLOR_BGR2GRAY);
    Mat edges;
    Canny(gray, edges, 98, 248);

    vector<Vec4i> lines;
    HoughLinesP(edges, lines, 1, CV_PI/180, 80, ui->spinBox_mlg->value(), ui->spinBox_mll->value());

    Mat hough = src->clone();
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line( hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 10, CV_AA);
    }
    displayImage(hough);
}

void Hough::displayImage(Mat &m){
    QImage qi = ASM::mat_to_qimage_ref(m, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();

}

void Hough::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}
