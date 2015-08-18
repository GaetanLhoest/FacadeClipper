#include "perspectivecorrection.h"
#include "ui_perspectivecorrection.h"

#include <methods/asmopencv.h>
#include <methods/treatments.h>
#include <methods/perspective.h>
#include <iostream>
#include <math.h>

#include <QImage>


using namespace cv;

PerspectiveCorrection::PerspectiveCorrection(QString im, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PerspectiveCorrection)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    this->scene = new QGraphicsScene;
    ui->graphicsView->setScene(this->scene);

    this->src = new Mat();
    *src = imread(im.toUtf8().constData(), 1);

    displayImage(*src);

}

PerspectiveCorrection::~PerspectiveCorrection()
{
    delete ui;
    delete scene;
    delete src;
}

void PerspectiveCorrection::displayImage(Mat &m){
    QImage qi = ASM::mat_to_qimage_ref(m, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    scene->clear();
    scene->addPixmap(im);
    scene->update();
}

void PerspectiveCorrection::resizeEvent(QResizeEvent *){
    ui->graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    scene->update();
}



void PerspectiveCorrection::on_pushButton_h_clicked()
{
    if (ui->radioButton_s->isChecked()){
        Mat hough = mhStandard(src, ui->spinBox_hst->value());
        displayImage(hough);
    }
    else{
        Mat hough = mhProbabilistic(src, ui->spinBox_hlg->value(), ui->spinBox_hll->value());
        displayImage(hough);
    }
}

void PerspectiveCorrection::on_pushButton_el_clicked()
{
    Mat edges;
    vector<Vec4i> lines;

    if (ui->radioButton_s->isChecked()){
        vector<Vec2f> linesf;
        edges = mhStandardv(src, ui->spinBox_hst->value(), linesf);
        for( size_t i = 0; i < linesf.size(); i++ ){
            float rho = linesf[i][0], theta = linesf[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            Vec4i line;
            line[0] = pt1.x;
            line[1] = pt1.y;
            line[2] = pt2.x;
            line[3] = pt2.y;
            lines.push_back(line);
        }
    }
    else{
        edges = mhProbabilisticv(src, ui->spinBox_hlg->value(), ui->spinBox_hll->value(), lines);
    }


    vector<Vec2i> extr;
    int w = edges.cols;
    int h = edges.rows;

    Vec4i top(0,0,w,0);
    Vec4i right(w,0,w,h);
    Vec4i bot(0,h,w,h);
    Vec4i left(0,0,0,h);


    for (size_t i = 0; i < lines.size(); i++){
        Vec2i x;
        x = findIntersection(lines[i], top);
        if (x[0] > 0 && x[0] < w)
            extr.push_back(x);

        x = findIntersection(lines[i], right);
        if (x[1] > 0 && x[1] < h)
            extr.push_back(x);

        x = findIntersection(lines[i], bot);
        if (x[0] > 0 && x[0] < w)
            extr.push_back(x);

        x = findIntersection(lines[i], left);
        if (x[1] > 0 && x[1] < h)
            extr.push_back(x);
    }

    Mat fullLines = src->clone();
    for( size_t i = 0; i < extr.size(); i+=2 )
    {
        Vec4i l(extr[i][0], extr[i][1], extr[i+1][0], extr[i+1][1]);
        line( fullLines, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 10, CV_AA);
    }
    displayImage(fullLines);
}

void PerspectiveCorrection::on_pushButton_vh_clicked()
{
    Mat edges;
    vector<Vec4i> lines;

    if (ui->radioButton_s->isChecked()){
        vector<Vec2f> linesf;
        edges = mhStandardv(src, ui->spinBox_hst->value(), linesf);
        for( size_t i = 0; i < linesf.size(); i++ ){
            float rho = linesf[i][0], theta = linesf[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            Vec4i line;
            line[0] = pt1.x;
            line[1] = pt1.y;
            line[2] = pt2.x;
            line[3] = pt2.y;
            lines.push_back(line);
        }
    }
    else{
        edges = mhProbabilisticv(src, ui->spinBox_hlg->value(), ui->spinBox_hll->value(), lines);
    }


    vector<Vec2i> extr;
    int w = edges.cols;
    int h = edges.rows;

    getExtremitiesIntersections(lines, w, h, extr);


    Mat fullLines = src->clone();
    for( size_t i = 0; i < extr.size(); i+=2 )
    {
        Vec4i e(extr[i][0], extr[i][1], extr[i+1][0], extr[i+1][1]);
        if (isVH(e)){
            line( fullLines, Point(e[0], e[1]), Point(e[2], e[3]), Scalar(0,0,255), 10, CV_AA);
        }
    }
    displayImage(fullLines);
}



void PerspectiveCorrection::on_pushButton_4ip_clicked()
{
    Mat edges;
    vector<Vec4i> lines;

    if (ui->radioButton_s->isChecked()){
        vector<Vec2f> linesf;
        edges = mhStandardv(src, ui->spinBox_hst->value(), linesf);
        for( size_t i = 0; i < linesf.size(); i++ ){
            float rho = linesf[i][0], theta = linesf[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            Vec4i line;
            line[0] = pt1.x;
            line[1] = pt1.y;
            line[2] = pt2.x;
            line[3] = pt2.y;
            lines.push_back(line);
        }
    }
    else{
        edges = mhProbabilisticv(src, ui->spinBox_hlg->value(), ui->spinBox_hll->value(), lines);
    }


    vector<Vec2i> extr;
    int w = edges.cols;
    int h = edges.rows;

    getExtremitiesIntersections(lines, w, h, extr);


    Mat fullLines = src->clone();
    vector<short> vhcodes;
    vector<Vec4i> vhlines;
    for( size_t i = 0; i < extr.size(); i+=2 )
    {
        Vec4i e(extr[i][0], extr[i][1], extr[i+1][0], extr[i+1][1]);
        if (isVH(e)){
            vhlines.push_back(e);
            vhcodes.push_back(isHOrV(e));
        }
    }

    Vec2i **inters;
    inters = new Vec2i *[vhlines.size()];
    for (unsigned int i = 0; i < vhlines.size(); i++)
        inters[i] = new Vec2i[vhlines.size()];

    getVHIntersection(vhlines, inters, vhcodes, w, h);

    for (size_t i = 0; i < vhlines.size(); i++){
        line( fullLines, Point(vhlines[i][0], vhlines[i][1]), Point(vhlines[i][2], vhlines[i][3]), Scalar(0,0,255), 20, CV_AA);
        for (size_t j = 0; j < vhlines.size(); j++){
            circle(fullLines, Point(inters[i][j][0], inters[i][j][1]), 10, Scalar(0,0,0), 10);
        }
    }


    //Searching intersection distance with nearest border

    long **distances;
    distances = new long *[vhlines.size()];
    short **cornercodes;
    cornercodes = new short *[vhlines.size()];

    for (unsigned int i = 0; i < vhlines.size(); i++){
        distances[i]= new long[vhlines.size()];
        cornercodes[i] = new short[vhlines.size()];
    }

    nearestCornerDistances(vhlines, inters, distances, cornercodes, w, h);



    //Searching extremities intersection of lines

    Vec2i extremities[vhlines.size()][2];
    long extrdistance[vhlines.size()];
    short extrcornercodes[vhlines.size()][2];

    getExtremitiesData(vhlines,
                       extremities,
                       extrcornercodes,
                       extrdistance,
                       distances,
                       cornercodes,
                       inters);



    for (size_t i = 0; i < vhlines.size(); i++){
        Vec2i p1 = extremities[i][0];
        Vec2i p2 = extremities[i][1];
        circle(fullLines, Point(p1[0], p1[1]), 20, Scalar(0,255,0), 20);
        circle(fullLines, Point(p2[0], p2[1]), 20, Scalar(0,255,0), 20);

    }

    //Finding the largest quadrilateral

    //Getting the nearest borders

    short bordercodes[vhlines.size()];
    getBorderCodes(vhlines, bordercodes, extrcornercodes);


    //Getting the 4 nearest borders lines formed by intersections

    Vec4i quadborders[4];
    long borderdistances[4];

    getNearestBordersLines(vhlines,
                           extremities,
                           quadborders,
                           borderdistances,
                           extrdistance,
                           bordercodes);

    Vec4i topquad = quadborders[TOP];
    Vec4i rightquad = quadborders[RIGHT];
    Vec4i botquad = quadborders[BOT];
    Vec4i leftquad = quadborders[LEFT];

    // Find if there is only 4 extremities

    sortBorder(topquad, TOP);
    sortBorder(rightquad, RIGHT);
    sortBorder(botquad, BOT);
    sortBorder(leftquad, LEFT);


    short counter = 0;
    Vec2i corner;
    while (!isQuadrilateral(quadborders[TOP],
                            quadborders[RIGHT],
                            quadborders[BOT],
                            quadborders[LEFT])
           && counter < 100){


        findShorterQuadrilateral(vhlines,
                                 extremities,
                                 quadborders,
                                 extrdistance,
                                 borderdistances,
                                 w,
                                 h,
                                 bordercodes);

        topquad = quadborders[TOP];
        rightquad = quadborders[RIGHT];
        botquad = quadborders[BOT];
        leftquad = quadborders[LEFT];

        sortBorder(topquad, TOP);
        sortBorder(rightquad, RIGHT);
        sortBorder(botquad, BOT);
        sortBorder(leftquad, LEFT);

        quadborders[TOP] = topquad;
        quadborders[RIGHT] = rightquad;
        quadborders[BOT] = botquad;
        quadborders[LEFT] = leftquad;

        counter++;

    }

    line( fullLines, Point(topquad[0], topquad[1]), Point(topquad[2], topquad[3]), Scalar(255,0,255), 40, CV_AA);
    line( fullLines, Point(rightquad[0], rightquad[1]), Point(rightquad[2], rightquad[3]), Scalar(255,0,255), 40, CV_AA);
    line( fullLines, Point(botquad[0], botquad[1]), Point(botquad[2], botquad[3]), Scalar(255,0,255), 40, CV_AA);
    line( fullLines, Point(leftquad[0], leftquad[1]), Point(leftquad[2], leftquad[3]), Scalar(255,0,255), 40, CV_AA);

    displayImage(fullLines);

}



void PerspectiveCorrection::on_pushButton_apply_clicked()
{
    Mat edges;
    vector<Vec4i> lines;

    if (ui->radioButton_s->isChecked()){
        vector<Vec2f> linesf;
        edges = mhStandardv(src, ui->spinBox_hst->value(), linesf);
        for( size_t i = 0; i < linesf.size(); i++ ){
            float rho = linesf[i][0], theta = linesf[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            Vec4i line;
            line[0] = pt1.x;
            line[1] = pt1.y;
            line[2] = pt2.x;
            line[3] = pt2.y;
            lines.push_back(line);
        }
    }
    else{
        edges = mhProbabilisticv(src, ui->spinBox_hlg->value(), ui->spinBox_hll->value(), lines);
    }


    vector<Vec2i> extr;
    int w = edges.cols;
    int h = edges.rows;

    getExtremitiesIntersections(lines, w, h, extr);


    Mat fullLines = src->clone();
    vector<short> vhcodes;
    vector<Vec4i> vhlines;
    for( size_t i = 0; i < extr.size(); i+=2 )
    {
        Vec4i e(extr[i][0], extr[i][1], extr[i+1][0], extr[i+1][1]);
        if (isVH(e)){
            vhlines.push_back(e);
            vhcodes.push_back(isHOrV(e));
        }
    }

    Vec2i **inters;
    inters = new Vec2i *[vhlines.size()];
    for (unsigned int i = 0; i < vhlines.size(); i++)
        inters[i] = new Vec2i[vhlines.size()];

    getVHIntersection(vhlines, inters, vhcodes, w, h);

    for (size_t i = 0; i < vhlines.size(); i++){
        line( fullLines, Point(vhlines[i][0], vhlines[i][1]), Point(vhlines[i][2], vhlines[i][3]), Scalar(0,0,255), 20, CV_AA);
        for (size_t j = 0; j < vhlines.size(); j++){
            circle(fullLines, Point(inters[i][j][0], inters[i][j][1]), 10, Scalar(0,0,0), 10);
        }
    }


    //Searching intersection distance with nearest border

    long **distances;
    distances = new long *[vhlines.size()];
    short **cornercodes;
    cornercodes = new short *[vhlines.size()];

    for (unsigned int i = 0; i < vhlines.size(); i++){
        distances[i]= new long[vhlines.size()];
        cornercodes[i] = new short[vhlines.size()];
    }

    nearestCornerDistances(vhlines, inters, distances, cornercodes, w, h);



    //Searching extremities intersection of lines

    Vec2i extremities[vhlines.size()][2];
    long extrdistance[vhlines.size()];
    short extrcornercodes[vhlines.size()][2];

    getExtremitiesData(vhlines,
                       extremities,
                       extrcornercodes,
                       extrdistance,
                       distances,
                       cornercodes,
                       inters);



    for (size_t i = 0; i < vhlines.size(); i++){
        Vec2i p1 = extremities[i][0];
        Vec2i p2 = extremities[i][1];
        circle(fullLines, Point(p1[0], p1[1]), 20, Scalar(0,255,0), 20);
        circle(fullLines, Point(p2[0], p2[1]), 20, Scalar(0,255,0), 20);

    }

    //Finding the largest quadrilateral

    //Getting the nearest borders

    short bordercodes[vhlines.size()];
    getBorderCodes(vhlines, bordercodes, extrcornercodes);


    //Getting the 4 nearest borders lines formed by intersections

    Vec4i quadborders[4];
    long borderdistances[4];

    getNearestBordersLines(vhlines,
                           extremities,
                           quadborders,
                           borderdistances,
                           extrdistance,
                           bordercodes);

    Vec4i topquad = quadborders[TOP];
    Vec4i rightquad = quadborders[RIGHT];
    Vec4i botquad = quadborders[BOT];
    Vec4i leftquad = quadborders[LEFT];

    // Find if there is only 4 extremities

    sortBorder(topquad, TOP);
    sortBorder(rightquad, RIGHT);
    sortBorder(botquad, BOT);
    sortBorder(leftquad, LEFT);


    short counter = 0;
    Vec2i corner;
    while (!isQuadrilateral(quadborders[TOP],
                            quadborders[RIGHT],
                            quadborders[BOT],
                            quadborders[LEFT])
           && counter < 100){


        findShorterQuadrilateral(vhlines,
                                 extremities,
                                 quadborders,
                                 extrdistance,
                                 borderdistances,
                                 w,
                                 h,
                                 bordercodes);

        topquad = quadborders[TOP];
        rightquad = quadborders[RIGHT];
        botquad = quadborders[BOT];
        leftquad = quadborders[LEFT];

        sortBorder(topquad, TOP);
        sortBorder(rightquad, RIGHT);
        sortBorder(botquad, BOT);
        sortBorder(leftquad, LEFT);

        quadborders[TOP] = topquad;
        quadborders[RIGHT] = rightquad;
        quadborders[BOT] = botquad;
        quadborders[LEFT] = leftquad;

        counter++;

    }

    Vec2i quadcorners[4];
    quadcorners[TOPLEFT][0] = quadborders[TOP][0];
    quadcorners[TOPLEFT][1] = quadborders[TOP][1];

    quadcorners[TOPRIGHT][0] = quadborders[TOP][2];
    quadcorners[TOPRIGHT][1] = quadborders[TOP][3];

    quadcorners[BOTLEFT][0] = quadborders[BOT][0];
    quadcorners[BOTLEFT][1] = quadborders[BOT][1];

    quadcorners[BOTRIGHT][0] = quadborders[BOT][2];
    quadcorners[BOTRIGHT][1] = quadborders[BOT][3];

    Vec2i rectangle[4];
    findShortestRectangle(quadcorners, rectangle, w, h);

    Point2f inputQuad[4];
    Point2f outputQuad[4];

    for (short i = 0; i < 4; i++){
        inputQuad[i] = Point2f(quadcorners[i][0], quadcorners[i][1]);
        outputQuad[i] = Point2f(rectangle[i][0], rectangle[i][1]);
    }

    Mat M = getPerspectiveTransform(inputQuad, outputQuad);


    warpPerspective(*src, fullLines, M, Size(w,h));

    displayImage(fullLines);

}


