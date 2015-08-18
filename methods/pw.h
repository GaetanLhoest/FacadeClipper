#ifndef PW_H
#define PW_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

/**
 * @brief Apply PowerWatershed
 *
 * Apply the power watershed method on a given image with the given 2-label or
 * multi-label seeds.
 *
 * @param   m               Source image to work on
 * @param   s               Seeds (2-label or multi-label)
 * @param   multipleseed    0 if 2-label or 1 if multi-label
 * @return  A data buffer containing the result. The format a grayscale image with
 * values from 0 to 255
 */
extern unsigned char * applyPowerWatershed(cv::Mat &m, cv::Mat &s, short multipleseed);

void findPWMask(cv::Mat *tmpModified, cv::Mat *MaskSeed, int s=-1, int e=-1);

void findPWMaskThreaded(cv::Mat *tmpModified, cv::Mat *MaskSeed);


#endif // PW_H
