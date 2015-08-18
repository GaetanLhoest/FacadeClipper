#include "treatments.h"

#include <thread>

using namespace cv;

Mat mhStandard(Mat *src, int treshold){
    Mat gray;
    cvtColor(*src, gray, COLOR_BGR2GRAY);
    Mat edges;
    Canny(gray, edges, 100, 241);

    vector<Vec2f> lines;
    HoughLines(edges, lines, 1, CV_PI/180, treshold);

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

    return hough;
}

Mat mhStandardv(Mat *src, int treshold, vector<Vec2f> &linesr){
    Mat gray;
    cvtColor(*src, gray, COLOR_BGR2GRAY);
    Mat edges;
    Canny(gray, edges, 100, 241);

    vector<Vec2f> lines;
    HoughLines(edges, lines, 1, CV_PI/180, treshold);

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

    linesr = lines;
    return hough;
}

Mat mhProbabilistic(Mat *src, int treshold1, int treshold2){
    Mat gray;
    cvtColor(*src, gray, COLOR_BGR2GRAY);
    Mat edges;
    Canny(gray, edges, 100, 241);

    vector<Vec4i> lines;
    HoughLinesP(edges, lines, 1, CV_PI/180, 80, treshold1, treshold2);

    Mat hough = src->clone();
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line( hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 10, CV_AA);
    }
    return hough;
}

Mat mhProbabilisticv(Mat *src, int treshold1, int treshold2, vector<Vec4i> &linesr){
    Mat gray;
    cvtColor(*src, gray, COLOR_BGR2GRAY);
    Mat edges;
    Canny(gray, edges, 100, 241);

    vector<Vec4i> lines;
    HoughLinesP(edges, lines, 1, CV_PI/180, 80, treshold1, treshold2);

    Mat hough = src->clone();
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line( hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 10, CV_AA);
    }
    linesr = lines;
    return hough;
}

void floodfillGreenPixels(Mat *img, Mat *interm, Mat *dst, int lo, int up, int h1, int h2){

    if (h1 == -1 and h2 == -1){
        h1 = 0;
        h2 = img->rows;
    }

    int w = img->cols;
    int connectivity = 4;
    int newMaskVal = 255;
    int ffillMode = 1;
    int flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);
    Rect ccomp;

    unsigned char *data = dst->data;
    unsigned char *output = interm->data;
    unsigned short b, g, r;
    unsigned short oob, oog, oor;
    int index1 = h1*w*3;

    for(int j = h1;j < h2;j++){
        for(int i = 0;i < w;i++){

//            b = data[(w * j * 3) + i*3 + 0] ;
//            g = data[(w * j * 3) + i*3 + 1];
//            r = data[(w * j * 3) + i*3 + 2];

            b = data[index1];
            oob = output[index1];
            index1++;
            g = data[index1];
            oog = output[index1];
            index1++;
            r = data[index1];
            oor = output[index1];
            index1++;

            if ((r == 0 and g == 255 and b == 0)and(!(oob == 0 and oog == 255 and oor == 0))){
                floodFill(*interm, Point(i,j), Scalar(0,255,0), &ccomp, Scalar(lo,lo,lo), Scalar(up,up,up), flags);
            }
        }
    }
}

void floodfillGreenPixelsThreaded(Mat *src, Mat *modified, int lo, int up){
    int h = src->rows;
    int s[8];
    int e[8];
    int step = h/8;

    for (int i = 0; i < 8; i++){
        s[i] = (i*step);
        e[i] = (i+1)*step;
    }
    e[7] = h;

    Mat *interm = new Mat();
    *interm = src->clone();

    std::thread first(floodfillGreenPixels, src, interm, modified, lo, up, s[0], e[0]);
    std::thread third(floodfillGreenPixels, src, interm, modified, lo, up, s[2], e[2]);
    std::thread fifth(floodfillGreenPixels, src, interm, modified, lo, up, s[4], e[4]);
    std::thread seventh(floodfillGreenPixels, src, interm, modified, lo, up, s[6], e[6]);

    first.join();
    third.join();
    fifth.join();
    seventh.join();

    std::thread second(floodfillGreenPixels, src, interm, modified, lo, up, s[1], e[1]);
    std::thread fourth(floodfillGreenPixels, src, interm, modified, lo, up, s[3], e[3]);
    std::thread sixth(floodfillGreenPixels, src, interm, modified, lo, up, s[5], e[5]);
    std::thread eighth(floodfillGreenPixels, src, interm, modified, lo, up, s[7], e[7]);

    second.join();
    fourth.join();
    sixth.join();
    eighth.join();

    *modified = interm->clone();
    delete interm;
}

void cannyFloodThreaded(Mat *src, Mat *tmpModified)
{

    Mat *scanny = new Mat();
    superCanny(src, scanny);

    int h = src->rows;
    int s[8];
    int e[8];
    int step = h/8;

    for (int i = 0; i < 8; i++){
        s[i] = (i*step);
        e[i] = (i+1)*step;
    }
    e[7] = h;

    std::thread first(cannyFlood, tmpModified, scanny, s[0], e[0]);
    std::thread second(cannyFlood, tmpModified, scanny, s[1], e[1]);
    std::thread third(cannyFlood, tmpModified, scanny, s[2], e[2]);
    std::thread fourth(cannyFlood, tmpModified, scanny, s[3], e[3]);
    std::thread fifth(cannyFlood, tmpModified, scanny, s[4], e[4]);
    std::thread sixth(cannyFlood, tmpModified, scanny, s[5], e[5]);
    std::thread seventh(cannyFlood, tmpModified, scanny, s[6], e[6]);
    std::thread eighth(cannyFlood, tmpModified, scanny, s[7], e[7]);

    first.join();
    second.join();
    third.join();
    fourth.join();
    fifth.join();
    sixth.join();
    seventh.join();
    eighth.join();

//    cannyFlood(tmpModified, scanny);

    delete scanny;
}

void cannyFlood(Mat *src, Mat *scanny, int h1, int h2)
{
    if (h1 == -1 && h2 == -1){
        h1 = 0;
        h2 = src->rows;
    }


    unsigned char *input = (unsigned char*)(src->data);
    unsigned char *output = (unsigned char*)(scanny->data);

    Scalar fcolor = Scalar(0, 255, 0);
    Rect ccomp;
    int connectivity = 4;
    int newMaskVal = 255;
    int ffillMode = 1;
    int flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);

    unsigned short b, g, r;
    unsigned short cb, cg, cr;

    int index1, index2;

    index1 = src->cols * h1 * 3;

    for (int j = h1; j < h2; j++){
        for (int i = 0; i < src->cols; i++){
//            b = input[(src->cols * j * 3) + i*3 ];
//            g = input[(src->cols * j * 3) + i*3 + 1];
//            r = input[(src->cols * j * 3) + i*3 + 2];

//            cb = output[(scanny->cols * j * 3) + i*3 ];
//            cg = output[(scanny->cols * j * 3) + i*3 + 1];
//            cr = output[(scanny->cols * j * 3) + i*3 + 2];

            index2 = index1;


            b = input[index1];
            cb = output[index1];
            index1++;
            g = input[index1];
            cg = output[index1];
            index1++;
            r = input[index1];
            cr = output[index1];
            index1++;


            if (cr < 20 and cg < 20 and cb < 20){
                if (r == 0 and g == 255 and b == 0) {
                    floodFill(*scanny, Point(i,j), fcolor, &ccomp, Scalar(10, 10, 10), Scalar(10, 10, 10), flags);
                }
            }
        }
    }

    index1 = src->cols * h1 * 3;
    for (int j = h1; j < h2; j++){
        for (int i = 0; i < src->cols; i++){

//            cb = output[(scanny->cols * j * 3) + i*3 ];
//            cg = output[(scanny->cols * j * 3) + i*3 + 1];
//            cr = output[(scanny->cols * j * 3) + i*3 + 2];

            index2 = index1;
            cb = output[index1];
            index1++;
            cg = output[index1];
            index1++;
            cr = output[index1];
            index1++;

            if (cr == 0 and cg == 255 and cb == 0){
//                input[(src->cols * j * 3) + i*3 ] = 0;
//                input[(src->cols * j * 3) + i*3 + 1] = 255;
//                input[(src->cols * j * 3) + i*3 + 2] = 0;

                input[index2] = 0;
                index2++;
                input[index2] = 255;
                index2++;
                input[index2] = 0;

            }
        }
    }
}

void superCanny(Mat *src, Mat *scanny)
{
    Mat *canny_m = new Mat();
    cvtColor(*src, *canny_m, COLOR_BGR2GRAY);
    blur(*canny_m, *canny_m, Size(3,3));
    cv::Canny(*canny_m, *canny_m, 0, 0, 3);
    cvtColor(*canny_m, *canny_m, COLOR_GRAY2BGR);


    cvtColor(*src, *scanny, COLOR_BGR2GRAY);
    cv::Canny(*scanny, *scanny, 0, 0, 3);
    cvtColor(*scanny, *scanny, COLOR_GRAY2BGR);

    unsigned char *output = (unsigned char*)(canny_m->data);
    unsigned char *output2 = (unsigned char*)(scanny->data);


    for (int j = 0; j < canny_m->rows; j++){
        for (int i = 0; i < canny_m->cols; i++){

            unsigned short cb = output[(canny_m->cols * j * 3) + i*3 ];
            unsigned short cg = output[(canny_m->cols * j * 3) + i*3 + 1];
            unsigned short cr = output[(canny_m->cols * j * 3) + i*3 + 2];

            if (cb > 125 and cg > 125 and cr > 125) {
                output2[(scanny->cols * j * 3) + i*3 ] = 255;
                output2[(scanny->cols * j * 3) + i*3 + 1] = 255;
                output2[(scanny->cols * j * 3) + i*3 + 2] = 255;
            }
        }
    }

    delete canny_m;
}

void fillHolesThreaded(Mat *tmpModified, int sqs)
{
    Mat *zones = new Mat();
    *zones = tmpModified->clone();

    keepGreenRestBlack(zones);

    int h = tmpModified->rows;
    int s[8];
    int e[8];
    int step = h/8;

    for (int i = 0; i < 8; i++){
        s[i] = (i*step);
        e[i] = (i+1)*step;
    }
    e[7] = h;

    std::thread first(fillHoles, zones, sqs, s[0], e[0]);
    std::thread second(fillHoles, zones, sqs, s[1], e[1]);
    std::thread third(fillHoles, zones, sqs, s[2], e[2]);
    std::thread fourth(fillHoles, zones, sqs, s[3], e[3]);
    std::thread fifth(fillHoles, zones, sqs, s[4], e[4]);
    std::thread sixth(fillHoles, zones, sqs, s[5], e[5]);
    std::thread seventh(fillHoles, zones, sqs, s[6], e[6]);
    std::thread eighth(fillHoles, zones, sqs, s[7], e[7]);

    first.join();
    second.join();
    third.join();
    fourth.join();
    fifth.join();
    sixth.join();
    seventh.join();
    eighth.join();

    //fillHoles(zones, sqs);

    useZonesOnResult(tmpModified, zones);

}

void keepGreenRestBlack(Mat *zones)
{
    unsigned char *input = (unsigned char*)(zones->data);

    unsigned short b, g, r;

    int index1, index2;
    index1 = 0;
    index2 = 0;
    for (int j = 0; j < zones->rows; j++){
        for (int i = 0; i < zones->cols; i++){

            index2 = index1;

//            b = input[(zones->cols * j * 3) + i*3 ];
//            g = input[(zones->cols * j * 3) + i*3 + 1];
//            r = input[(zones->cols * j * 3) + i*3 + 2];

            b = input[index1];
            index1++;
            g = input[index1];
            index1++;
            r = input[index1];
            index1++;

            if (!(r == 0 and g == 255 and b == 0)){
//                input[(zones->cols * j * 3) + i*3 ] = 0;
//                input[(zones->cols * j * 3) + i*3 + 1] = 0;
//                input[(zones->cols * j * 3) + i*3 + 2] = 0;
                input[index2] = 0;
                index2++;
                input[index2] = 0;
                index2++;
                input[index2] = 0;
            }
        }
    }
}

void fillHoles(Mat *zones, int sqsize, int s, int e)
{
    int h = zones->rows;
    int w = zones->cols;
    int h1, h2;
    h1 = s;
    h2 = e;
    if (s == -1){
        h1 = sqsize;
        h2 = h - sqsize;
    }
    if(s < sqsize and s != -1){
        h1 = sqsize;
    }
    if(e > h - sqsize){
        h2 = h-sqsize;
    }

    unsigned char *input = (unsigned char*)(zones->data);

    int racc, gacc, bacc;
    int index;
    Scalar fcolor = Scalar(255, 255, 255);
    Rect ccomp;
    int connectivity = 4;
    int newMaskVal = 255;
    int ffillMode = 1;
    int flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);

    unsigned short b, g, r;
    for (int i = h1; i < h2; i++){
        for (int j = sqsize; j < zones->cols - sqsize; j++){

            racc = 0;
            gacc = 0;
            bacc = 0;

            for (int k = i-sqsize; k <= i+sqsize; k++){
                for (int l = j-sqsize; l <= j+sqsize; l++){
                    index = (k*3*w) + (3*l);
                    b = input[index + 0];
                    g = input[index + 1];
                    r = input[index + 2];

                    racc += r;
                    gacc += g;
                    bacc += b;

                }
            }

            if (racc == 0 and gacc == 0 and bacc == 0){
                floodFill(*zones, Point(j,i), fcolor, &ccomp, Scalar(0, 0, 0), Scalar(0, 0, 0), flags);
            }

        }
    }
}

void useZonesOnResult(Mat *tmpModified, Mat *zones)
{

    unsigned char *input = (unsigned char*)(zones->data);
    unsigned char *output = (unsigned char*)(tmpModified->data);

    int index1 = 0;
    int index2 = 0;
    unsigned short b, g, r;
    for (int j = 0; j < zones->rows; j++){
        for (int i = 0; i < zones->cols; i++){

            //unsigned short b = input[(zones->cols * j * 3) + i*3 ];
            //unsigned short g = input[(zones->cols * j * 3) + i*3 + 1];
            //unsigned short r = input[(zones->cols * j * 3) + i*3 + 2];
            index2 = index1;
            b = input[index1];
            index1++;
            g = input[index1];
            index1++;
            r = input[index1];
            index1++;

            if (!(r == 255 and g == 255 and b == 255)){
//                output[(zones->cols * j * 3) + i*3 ] = 0;
//                output[(zones->cols * j * 3) + i*3 + 1] = 255;
//                output[(zones->cols * j * 3) + i*3 + 2] = 0;

                output[index2] = 0;
                index2++;
                output[index2] = 255;
                index2++;
                output[index2] = 0;

            }
        }
    }
}


void getDefinedZones(Mat *imsrc, Mat *tmpModified, Mat *modified)
{
    unsigned char *input = (unsigned char*)(imsrc->data);
    unsigned char *green = (unsigned char*)(tmpModified->data);
    unsigned char *output = (unsigned char*)(modified->data);

    int index1 = 0;
    int index2 = 0;
    unsigned short b, g, r;
    for (int j = 0; j < imsrc->rows; j++){
        for (int i = 0; i < imsrc->cols; i++){

            index2 = index1;
            b = green[index1];
            index1++;
            g = green[index1];
            index1++;
            r = green[index1];
            index1++;

            if (r == 0 and g == 255 and b == 0){
                output[index2] = input[index2];
                index2++;
                output[index2] = input[index2];
                index2++;
                output[index2] = input[index2];
            }
            else{
                output[index2] = 255;
                index2++;
                output[index2] = 255;
                index2++;
                output[index2] = 255;
            }
        }
    }
}

void combineResultsThreaded(Mat *tmpModified, Mat *cannyflo, Mat *powerWa)
{
    int h = tmpModified->rows;
    int s[8];
    int e[8];
    int step = h/8;

    for (int i = 0; i < 8; i++){
        s[i] = (i*step);
        e[i] = (i+1)*step;
    }
    e[7] = h;

    std::thread first(combineResults, tmpModified, cannyflo, powerWa, s[0], e[0]);
    std::thread second(combineResults, tmpModified, cannyflo, powerWa, s[1], e[1]);
    std::thread third(combineResults, tmpModified, cannyflo, powerWa, s[2], e[2]);
    std::thread fourth(combineResults, tmpModified, cannyflo, powerWa, s[3], e[3]);
    std::thread fifth(combineResults, tmpModified, cannyflo, powerWa, s[4], e[4]);
    std::thread sixth(combineResults, tmpModified, cannyflo, powerWa, s[5], e[5]);
    std::thread seventh(combineResults, tmpModified, cannyflo, powerWa, s[6], e[6]);
    std::thread eighth(combineResults, tmpModified, cannyflo, powerWa, s[7], e[7]);

    first.join();
    second.join();
    third.join();
    fourth.join();
    fifth.join();
    sixth.join();
    seventh.join();
    eighth.join();
}


void combineResults(Mat *tmpModified, Mat *cannyflo, Mat *powerWa, int h1, int h2)
{

    int w = tmpModified->cols;
    int h = tmpModified->rows;

    if (h1 == -1 && h2 == -1){
        h1 = 0;
        h2 = h;
    }

    unsigned char *mo = (unsigned char*)(tmpModified->data);
    unsigned char *ca = (unsigned char*)(cannyflo->data);
    unsigned char *po = (unsigned char*)(powerWa->data);

    int index1, index2;
    index1 = 0;
    index2 = 0;

    for (int j = h1; j < h2; j++){
        for (int i = 0; i < w; i++){
            if (po[index1] == 0
                and ca[index2 + 0] == 0
                and ca[index2 + 1] == 255
                and ca[index2 + 2] == 0){

                mo[index2 + 0] = 0;
                mo[index2 + 1] = 255;
                mo[index2 + 2] = 0;

            }

            index1++;
            index2 += 3;
        }
    }
}

void cannyFloodBorderThreaded(Mat *src, Mat *tmpModified, Mat *borders)
{

    Mat *scanny = new Mat();
    superCanny(src, scanny);

    int h = src->rows;
    int s[8];
    int e[8];
    int step = h/8;

    for (int i = 0; i < 8; i++){
        s[i] = (i*step);
        e[i] = (i+1)*step;
    }
    e[7] = h;

    std::thread first(cannyFloodBorder, tmpModified, scanny, borders, s[0], e[0]);
    std::thread second(cannyFloodBorder, tmpModified, scanny, borders, s[1], e[1]);
    std::thread third(cannyFloodBorder, tmpModified, scanny, borders, s[2], e[2]);
    std::thread fourth(cannyFloodBorder, tmpModified, scanny, borders, s[3], e[3]);
    std::thread fifth(cannyFloodBorder, tmpModified, scanny, borders, s[4], e[4]);
    std::thread sixth(cannyFloodBorder, tmpModified, scanny, borders, s[5], e[5]);
    std::thread seventh(cannyFloodBorder, tmpModified, scanny, borders, s[6], e[6]);
    std::thread eighth(cannyFloodBorder, tmpModified, scanny, borders, s[7], e[7]);

    first.join();
    second.join();
    third.join();
    fourth.join();
    fifth.join();
    sixth.join();
    seventh.join();
    eighth.join();

    unsigned char *input = (unsigned char*)(tmpModified->data);
    unsigned char *output = (unsigned char*)(scanny->data);

    unsigned char cb, cg, cr;

    int index1 = 0;
    int index2 = 0;
    for (int j = 0; j < src->rows; j++){
        for (int i = 0; i < src->cols; i++){

            index2 = index1;
            cb = output[index1];
            index1++;
            cg = output[index1];
            index1++;
            cr = output[index1];
            index1++;

            if (cr == 0 and cg == 255 and cb == 0){

                input[index2] = 0;
                index2++;
                input[index2] = 255;
                index2++;
                input[index2] = 0;

            }
        }
    }

    delete scanny;
}

void cannyFloodBorder(Mat *src, Mat *scanny, Mat *borders, int h1, int h2)
{
    if (h1 == -1 && h2 == -1){
        h1 = 0;
        h2 = src->rows;
    }

    unsigned char *input = (unsigned char*)(src->data);
    unsigned char *output = (unsigned char*)(scanny->data);
    unsigned char *bo = (unsigned char*)(borders->data);

    Scalar fcolor = Scalar(0, 255, 0);
    Rect ccomp;
    int connectivity = 4;
    int newMaskVal = 255;
    int ffillMode = 1;
    int flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);

    unsigned short b, g, r;
    unsigned short cb, cg, cr;
    unsigned short gr;

    int index1, index3;

    index1 = src->cols * h1 * 3;
    index3 = src->cols * h1;

    for (int j = h1; j < h2; j++){
        for (int i = 0; i < src->cols; i++){



            b = input[index1];
            cb = output[index1];
            index1++;
            g = input[index1];
            cg = output[index1];
            index1++;
            r = input[index1];
            cr = output[index1];
            index1++;

            gr = bo[index3];


            if (cr < 20 and cg < 20 and cb < 20){
                if (r == 0 and g == 255 and b == 0 and gr == 125) {
                    floodFill(*scanny, Point(i,j), fcolor, &ccomp, Scalar(10, 10, 10), Scalar(10, 10, 10), flags);
                }
            }
            index3++;
        }
    }

}

