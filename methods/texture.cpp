#include "texture.h"

#include <iostream>
#include <thread>
#include <fstream>
#include <string>

using namespace cv;



void getSquaresStatDataRGB(int sqsize, int w, int h, unsigned char *image, long *mean, long *stdev){

    long r, g, b; // double plutot
    long mer, meg, meb;  // Mean for r, g and b
                         // Standard Deviation for r, g and b
    long totr, totg, totb; // For standard deviation computations
    long nbr = ((2*sqsize)+1)*((2*sqsize)+1); // Nbr of pixels in a square
    int index1, index2;
    index2 = 0;
    for (int i = sqsize; i < h-sqsize; i++){
        for (int j = sqsize; j < w-sqsize; j++){
            mer = 0;
            meg = 0;
            meb = 0;
            totr = 0;
            totg = 0;
            totb = 0;


            // Work on a square
            for (int k = i-sqsize; k <= i+sqsize; k++){
                for (int l = j-sqsize; l <= j+sqsize; l++){
                    index1 = (k*3*w) + (3*l);
                    b = (long)image[index1 + 0];
                    g = (long)image[index1 + 1];
                    r = (long)image[index1 + 2];

                    mer += r/nbr; // Divide by nbr at the end
                    meg += g/nbr;
                    meb += b/nbr;
                }
            }


            for (int k = i-sqsize; k <= i+sqsize; k++){
                for (int l = j-sqsize; l <= j+sqsize; l++){
                    index1 = (k*3*w) + (3*l);
                    b = (long)image[index1 + 0];
                    g = (long)image[index1 + 1];
                    r = (long)image[index1 + 2];

                    totr += r*r/nbr;
                    totg += g*g/nbr;
                    totb += b*b/nbr;
                }
            }

            mean[index2+0] = mer;
            mean[index2+1] = meg;
            mean[index2+2] = meb;

            stdev[index2+0] = sqrt((totr)-(mer*mer));
            stdev[index2+1] = sqrt((totg)-(meg*meg));
            stdev[index2+2] = sqrt((totb)-(meb*meb));

            index2 += 3;
        }
    }
}

void getSquaresStatData(Mat *img, Mat *grimg, int sqsize, double *mean, double *meanRGB, double *stdev, int s, int e){
    int w = grimg->cols;
    int h = grimg->rows;

    vector<int> positions;

    double totgr = 0, totr = 0, totg = 0, totb = 0;
    double totdiff = 0;

    double nbr = ((2*sqsize) + 1)*((2*sqsize) + 1);

    unsigned char *grdata = grimg->data;
    unsigned char *data = img->data;

    int index2 = 0;

    int h1, h2;
    h1 = s;
    h2 = e;
    if (s == -1){
        h1 = sqsize;
        h2 = h - sqsize;
    }
    if(s < sqsize and s != -1){
        h1 = sqsize;
        h2 = e;
    }
    if(e > h - sqsize){
        h2 = h-sqsize;
    }
    int index1 = ((w-(2*sqsize))*(h1-sqsize));

    for (int i = h1; i < h2; i++){
        for (int j = sqsize; j < w-sqsize; j++){
            totgr = 0;
            totr = 0;
            totg = 0;
            totb = 0;

            for (int k = i-sqsize; k <= i+sqsize; k++){
                for (int l = j-sqsize; l <= j+sqsize; l++){
                    index2 = (k*w)+l;
                    totgr += (double)grdata[index2];
                    totb += (double)data[(index2*3) + 0];
                    totg += (double)data[(index2*3) + 1];
                    totr += (double)data[(index2*3) + 2];
                }
            }

            mean[index1] = totgr / nbr;
            meanRGB[(index1*3) + 0] = totr / nbr;
            meanRGB[(index1*3) + 1] = totg / nbr;
            meanRGB[(index1*3) + 2] = totb / nbr;

            totdiff = 0;

            for (int k = i-sqsize; k <= i+sqsize; k++){
                for (int l = j-sqsize; l <= j+sqsize; l++){
                    index2 = (k*w)+l;
                    totdiff += ((double)grdata[index2]-mean[index1])*((double)grdata[index2]-mean[index1]);
                }
            }

            stdev[index1] = totdiff / nbr;
            stdev[index1] = sqrt(stdev[index1]);


            index1++;

        }
    }

}

void writeStats(int size, long *mean, long *stdev){

    std::ofstream mf("mean");

    std::ofstream sf("stdev");

    for (int i = 0; i < size; i+=3){
        mf << mean[i+0] << "\t\t" << mean[i+1] << "\t\t" << mean[i+2] << "\n";
        sf << stdev[i+0] << "\t\t" << stdev[i+1] << "\t\t" << stdev[i+2] << "\n";
    }
}

// img is in RGB, grimg is in greyscale
void getLinesStatData(Mat *img, Mat *src, Mat *grimg, double *mean, double *meanRGB, double *stdev){
    int w = img->cols;
    int h = img->rows;

    vector<int> positions;

    double r, g, b;
    double totgr = 0, totr = 0, totg = 0, totb = 0;

    double nbr = 0;

    unsigned char *data = img->data;
    unsigned char *datasrc = src->data;
    unsigned char *grdata = grimg->data;
    int index1 = 0, index2 = 0;

    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){


            b = (double)data[index1 + 0];
            g = (double)data[index1 + 1];
            r = (double)data[index1 + 2];

            if (r == 0 && g == 255 && b == 0){
                nbr++;
                positions.push_back(j);
                positions.push_back(i);

                b = (double)datasrc[index1 + 0];
                g = (double)datasrc[index1 + 1];
                r = (double)datasrc[index1 + 2];

                totgr += (double)grdata[index2];
                totr += r;
                totg += g;
                totb += b;

            }
            index1 += 3;
            index2 += 1;
        }
    }

    if (nbr == 0){
        *mean = 0;
        *stdev = 0;
    }
    else{
        *mean = totgr / nbr;
        meanRGB[0] = totr / nbr;
        meanRGB[1] = totg / nbr;
        meanRGB[2] = totb / nbr;

        double totdiff = 0;

        int x, y;
        for (size_t i = 0; i < positions.size(); i+=2){
            x = positions.at(i);
            y = positions.at(i+1);

            index1 = (y*w) + x;

            totdiff += ((double)grdata[index1]-*mean)*((double)grdata[index1]-*mean);

        }

        *stdev = totdiff / nbr;
        *stdev = sqrt(*stdev);
    }

}

void getMatchingPositions(double meant, double *meantRGB, double stdevt, double meantreshold, double meanRGBtreshold, double stdevtreshold, double *mean, double *meanRGB, double *stdev, int w, int h, int sqsize, vector<int> *positions){


    double meantl = meant - meantreshold;
    double meantu = meant + meantreshold;

    double meantrl = meantRGB[0] - meanRGBtreshold;
    double meantru = meantRGB[0] + meanRGBtreshold;
    double meantgl = meantRGB[1] - meanRGBtreshold;
    double meantgu = meantRGB[1] + meanRGBtreshold;
    double meantbl = meantRGB[2] - meanRGBtreshold;
    double meantbu = meantRGB[2] + meanRGBtreshold;


    double stdevtl = stdevt - stdevtreshold;
    double stdevtu = stdevt + stdevtreshold;



    int ws = (w-(2*sqsize));
    int hs = (h-(2*sqsize));
    int size = ws * hs;

    int x, y;

    for (int i = 0; i < size; i++){
        if (mean[i] > meantl && mean[i] < meantu
            && meanRGB[i*3 + 0] > meantrl && meanRGB[i*3 + 0] < meantru
            && meanRGB[i*3 + 1] > meantgl && meanRGB[i*3 + 1] < meantgu
            && meanRGB[i*3 + 2] > meantbl && meanRGB[i*3 + 2] < meantbu
            && stdev[i] > stdevtl && stdev[i] < stdevtu){

            x = i % ws;
            x += sqsize;
            y = i / ws;
            y += sqsize;
            positions->push_back(x);
            positions->push_back(y);

        }
    }
}

void setPositionsToGreen(Mat *img, vector<int> *positions){

    int x, y;

    int w = img->cols;

    int index;

    unsigned char *data = img->data;

    for (size_t i = 0; i <  positions->size(); i+=2){

        x = positions->at(i);
        y = positions->at(i+1);

        index = 3*y*(w) + 3*x;

        data[index+0] = 0;
        data[index+1] = 255;
        data[index+2] = 0;



    }

}

void getSameTextureAsGreenZone(Mat *src, Mat *tmpModified, int sqs, int meanT, int meanRGBT, int stdT)
{
    int w = src->cols;
    int h = src->rows;

    double *mean;
    double *meanRGB;
    double *stdev;

    mean = new double[(w-(2*sqs))*(h-(2*sqs))];
    meanRGB = new double[(w-(2*sqs))*(h-(2*sqs))*3];
    stdev = new double[(w-(2*sqs))*(h-(2*sqs))];



    double meant, stdevt;
    double meantRGB[3];

    Mat gsrc;
    cvtColor(*src, gsrc, CV_BGR2GRAY);

    getLinesStatData(tmpModified, src, &gsrc, &meant, meantRGB, &stdevt);

    int s[8];
    int e[8];
    int step = h/8;

    for (int i = 0; i < 8; i++){
        s[i] = (i*step);
        e[i] = (i+1)*step;
    }
    e[7] = h;

    std::thread first(getSquaresStatData, src, &gsrc, sqs, mean, meanRGB, stdev, s[0], e[0]);
    std::thread second(getSquaresStatData, src, &gsrc, sqs, mean, meanRGB, stdev, s[1], e[1]);
    std::thread third(getSquaresStatData, src, &gsrc, sqs, mean, meanRGB, stdev, s[2], e[2]);
    std::thread fourth(getSquaresStatData, src, &gsrc, sqs, mean, meanRGB, stdev, s[3], e[3]);
    std::thread fifth(getSquaresStatData, src, &gsrc, sqs, mean, meanRGB, stdev, s[4], e[4]);
    std::thread sixth(getSquaresStatData, src, &gsrc, sqs, mean, meanRGB, stdev, s[5], e[5]);
    std::thread seventh(getSquaresStatData, src, &gsrc, sqs, mean, meanRGB, stdev, s[6], e[6]);
    std::thread eighth(getSquaresStatData, src, &gsrc, sqs, mean, meanRGB, stdev, s[7], e[7]);

    first.join();
    second.join();
    third.join();
    fourth.join();
    fifth.join();
    sixth.join();
    seventh.join();
    eighth.join();

    vector<int> positions;
    getMatchingPositions(meant, meantRGB, stdevt, meanT, meanRGBT, stdT, mean, meanRGB, stdev, w, h, sqs, &positions);

    //*tmpModified = src->clone();
    setPositionsToGreen(tmpModified, &positions);
    delete mean;
    delete meanRGB;
    delete stdev;
}
