#include "pw.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <mccodimage.h>
#include <mcimage.h>
#include <cccodimage.h>
#include <lMSF.h>
#include <MSF_RW.h>
#include <powerwatsegm.h>
#include <image_toolbox.h>
#include <unistd.h>
#include <argv.h>
#include <iostream>

#include <thread>

using namespace cv;

unsigned char * applyPowerWatershed(Mat &imageM, Mat &seedsM, short multipleseed){
    unsigned char * image = imageM.data;
    int rs, cs, ds, N, M;
    struct xvimage * seeds = readimagecustom(seedsM.data, seedsM.rows, seedsM.cols);
    if (seeds == NULL) { fprintf(stderr, "msf_rw: readimage failed\n"); exit(1); }

    unsigned char * s = UCHARDATA(seeds);


    rs = rowsize(seeds);
    cs = colsize(seeds);
    ds = 1;//colsize(seeds);
    N = rs * cs * ds;
    M = ds*rs*(cs-1)+ds*(rs-1)*cs+(ds-1)*cs*rs;  //number of edges

    uint32_t * weights = (uint32_t *)malloc(sizeof(uint32_t)*M);


    int ** edges;
    edges =  (int**)malloc(2*sizeof(int*));
    for(short i=0;i<2;i++)
        edges[i] = (int*)malloc(M*sizeof(int));
    compute_edges(edges,rs, cs, ds);


    int * index_seeds = (int*)malloc(N*sizeof(int));
    uint8_t * index_labels = (uint8_t*)malloc(N*sizeof(uint8_t));


    int32_t nblabels, i,j;
    bool mult;
    if (multipleseed == 0)
        mult = false;
    else
        mult = true;
    //multilabel seed image

    j = 0;
    if (mult == true)
    {
        nblabels = 0;
        for (i=0;i<rs*cs*ds;i++)
            if(s[i]>0)
            {
                index_seeds[j]=i;
                index_labels[j]=s[i];
                j++;
                if(s[i]>nblabels) nblabels = s[i];
            }
    }
    else
    {
        nblabels=2;
        for (i=0;i<rs*cs*ds;i++)
        {
            if(s[i]>155)
            {
                index_seeds[j] = i;
                index_labels[j]=1;
                j++;
            }
            else if(s[i]<100)
            {
                index_seeds[j] = i;
                index_labels[j]=2;
                j++;
            }
        }
    }
    int size_seeds = j;
    freeimage(seeds);

    uint32_t * normal_weights ;
    uint32_t max_weight = 255;

    bool quicksort = false;
    int rows = imageM.rows;
    int cols = imageM.cols;
    normal_weights = color_standard_weights_PW_custom(image, weights, edges, index_seeds, size_seeds, &max_weight, quicksort, rows, cols);
    struct xvimage * img_proba;
#ifndef SPEEDY
      img_proba = allocimage(NULL, rs, cs, ds, VFF_TYP_1_BYTE);
#endif
    struct xvimage * outputcv = NULL;
    outputcv = PowerWatershed_q2(edges, weights, normal_weights,max_weight,index_seeds, index_labels, size_seeds,rs, cs, ds, nblabels, quicksort, img_proba);
    unsigned char *outputc = UCHARDATA(outputcv);

    return outputc;

}

void findPWMaskThreaded(Mat *tmpModified, Mat *MaskSeed)
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

    std::thread first(findPWMask, tmpModified, MaskSeed, s[0], e[0]);
    std::thread second(findPWMask, tmpModified, MaskSeed, s[1], e[1]);
    std::thread third(findPWMask, tmpModified, MaskSeed, s[2], e[2]);
    std::thread fourth(findPWMask, tmpModified, MaskSeed, s[3], e[3]);
    std::thread fifth(findPWMask, tmpModified, MaskSeed, s[4], e[4]);
    std::thread sixth(findPWMask, tmpModified, MaskSeed, s[5], e[5]);
    std::thread seventh(findPWMask, tmpModified, MaskSeed, s[6], e[6]);
    std::thread eighth(findPWMask, tmpModified, MaskSeed, s[7], e[7]);

    first.join();
    second.join();
    third.join();
    fourth.join();
    fifth.join();
    sixth.join();
    seventh.join();
    eighth.join();
}


void findPWMask(Mat *tmpModified, Mat *MaskSeed, int s, int e)
{

    int sqsize = 15;
    int w = tmpModified->cols;
    int h = tmpModified->rows;

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



    bool writeW, notwriteB;

    int index;

    unsigned short b, g, r;

    unsigned char *data = tmpModified->data;
    unsigned char *grey = MaskSeed->data;

    for (int i = h1; i < h2; i++){
        for (int j = sqsize; j < w-sqsize; j++){
            writeW = true;
            notwriteB = false;

            for (int k = i-sqsize; k <= i+sqsize; k++){
                for (int l = j-sqsize; l <= j+sqsize; l++){
                    index = (k*w*3) + (3*l);
                    b = data[index + 0];
                    g = data[index + 1];
                    r = data[index + 2];

                    if (!(b == 0 and g == 255 and r == 0)){
                        writeW = false;
                    }
                    if (b == 0 and g == 255 and r == 0){
                        notwriteB = true;
                    }

                }
            }

            if (writeW){
                grey[(w*i)+j] = 255;
            }
            if (!notwriteB){
                grey[(w*i)+j] = 0;
            }

        }
    }

}
