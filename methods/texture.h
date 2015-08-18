#ifndef TEXTURE_H
#define TEXTURE_H

#include <QString>

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

void getSquaresStatDataRGB(int sqsize, int w, int h, unsigned char *image, long *mean, long *stdev);

void getSquaresStatData(cv::Mat *img, cv::Mat *grimg, int sqsize, double *mean, double *meanRGB, double *stdev, int s = -1, int e = -1);

void writeStats(int size, long *mean, long *vari, long *stdev);

void getLinesStatData(cv::Mat *img, cv::Mat *src, cv::Mat *gimg, double *mean, double *meanRGB, double *stdev);

void getMatchingPositions(double meant, double *meantRGB, double stdevt, double meantreshold, double meanRGBtreshold, double stdevtreshold, double *mean, double *meanRGB, double *stdev, int w, int h, int sqsize, std::vector<int> *positions);

void setPositionsToGreen(cv::Mat *img, std::vector<int> *positions);

void getSameTextureAsGreenZone(cv::Mat *src, cv::Mat *tmpModified, int sqs, int meanT, int meanRGBT, int stdT);

#endif // TEXTURE_H
