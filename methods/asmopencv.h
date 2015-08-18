#ifndef __ASM_OPENCV_H__
#define __ASM_OPENCV_H__

/*
    Functions to convert between OpenCV's cv::Mat and Qt's QImage and QPixmap.s
 */

#include <QImage>
#include <QPixmap>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"


namespace ASM {

/**
 * @brief Convert Mat to QImage
 *
 * Takes 3 kind of cv::Mat (CV_8UC4, CV_8UC3, CV_8UC1) and return a QImage corresponding.
 *
 * @param inMat The opencv Mat we want the corresponding QImage
 * @return QImage corresponding to the opencv Mat
 */
inline QImage  cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );
        
        return image;
    }
        
        // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );
        
        return image.rgbSwapped();
    }
        
        // 8-bit, 1 channel
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;
        
        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
            for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
        }
        
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );
        
        image.setColorTable( sColorTable );
        
        return image;
    }
        return QImage();
    }
    
    
    return QImage();
}

/**
 * @brief Convert Mat to QPixmap
 *
 * Takes 3 kind of cv::Mat (CV_8UC4, CV_8UC3, CV_8UC1) and return a QPixmap corresponding.
 *
 * @param inMat The opencv Mat which we want the corresponding QPixmap
 * @return QPixmap corresponding to the opencv Mat
 */
inline QPixmap cvMatToQPixmap( const cv::Mat &inMat )
{
    return QPixmap::fromImage( cvMatToQImage( inMat ) );
}

/**
 * @brief Mat to QImage reference
 *
 * Return a reference to a QImage sharing the same data buffer as the Opencv Mat.
 *
 * @param mat The opencv Mat which we want to share the data buffer with the QImage.
 * @param format The format of the QImage.
 * @return A QImage reference of format "format" with the same data buffer as "mat".
 */
inline QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows, mat.step, format);
}

/**
 * @brief QImage to Mat reference
 *
 * Return a reference to an opencv Mat sharing the same data buffer as the QImage.
 *
 * @param img The QImage which we want to share the data buffer with the opencv Mat.
 * @param format The format of the opencv Mat.
 * @return An opencv Mat reference of format "format" with the same data buffer as "img".
 */
inline cv::Mat qimage_to_mat_ref(QImage &img, int format)
{
    return cv::Mat(img.height(), img.width(),
                   format, img.bits(), img.bytesPerLine());
}

/**
 * @brief Mat to QImage copy.
 *
 * Return a QImage owning a data buffer which is a copy of the data buffer an opencv Mat.
 *
 * @param mat The opencv Mat owning the data buffer we want to copy.
 * @param format The format of the returning QImage.
 * @return A QImage of format "format" owning a copy of the data buffer of "mat".
 */
inline QImage mat_to_qimage_cpy(cv::Mat const &mat, QImage::Format format)
{
    return QImage(mat.data, mat.cols, mat.rows,
                  mat.step, format).copy();
}

}

#endif // __ASM_OPENCV_H__
