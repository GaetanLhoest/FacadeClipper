#ifndef TREATMENTS_H
#define TREATMENTS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

/**
 * @brief mhStandard
 * @param src
 * @param treshold
 * @return
 */
cv::Mat mhStandard(cv::Mat *src, int treshold);

/**
 * @brief mhStandardv
 * @param src
 * @param treshold
 * @param linesr
 * @return
 */
cv::Mat mhStandardv(cv::Mat *src, int treshold, std::vector<cv::Vec2f> &linesr);

/**
 * @brief mhProbabilistic
 * @param src
 * @param treshold1
 * @param treshold2
 * @return
 */
cv::Mat mhProbabilistic(cv::Mat *src, int treshold1, int treshold2);

/**
 * @brief mhProbabilisticv
 * @param src
 * @param treshold1
 * @param treshold2
 * @param linesr
 * @return
 */
cv::Mat mhProbabilisticv(cv::Mat *src, int treshold1, int treshold2, std::vector<cv::Vec4i> &linesr);

void floodfillGreenPixels(cv::Mat *img, cv::Mat *interm, cv::Mat *dst, int lo, int up, int h1=-1, int e=-1);

void floodfillGreenPixelsThreaded(cv::Mat *src, cv::Mat *modified, int lo, int up);

void cannyFloodThreaded(cv::Mat *src, cv::Mat *tmpModified);

void cannyFlood(cv::Mat *src, cv::Mat *canny, int h1=-1, int h2=-1);

void superCanny(cv::Mat *src, cv::Mat *scanny);

void fillHolesThreaded(cv::Mat *tmpModified, int sqs);

void keepGreenRestBlack(cv::Mat *zones);

void fillHoles(cv::Mat *zones, int sqsize, int s=-1, int e=-1);

void useZonesOnResult(cv::Mat *tmpModified, cv::Mat *zones);

void getDefinedZones(cv::Mat *imsrc, cv::Mat *tmpModified, cv::Mat *modified);

void combineResultsThreaded(cv::Mat *tmpModified, cv::Mat *cannyflo, cv::Mat *powerWa);

void combineResults(cv::Mat *tmpModified, cv::Mat *cannyflo, cv::Mat *powerWa, int h1=-1, int h2=-1);

void cannyFloodBorderThreaded(cv::Mat *src, cv::Mat *tmpModified, cv::Mat *borders);

void cannyFloodBorder(cv::Mat *src, cv::Mat *scanny, cv::Mat *borders, int h1=-1, int h2=-1);

#endif // TREATMENTS_H
