 #ifndef ZONESEXTRACTOR_H
#define ZONESEXTRACTOR_H

#include "classes/scene.h"

#include <QDialog>
#include <QString>

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

namespace Ui {
class ZonesExtractor;
}

class ZonesExtractor : public QDialog
{
    Q_OBJECT

public:
    explicit ZonesExtractor(QString im, QWidget *parent = 0);
    ~ZonesExtractor();

private:
    Ui::ZonesExtractor *ui;
    QString im;
    Scene *scene;
    cv::Mat *src;
    cv::Mat *imsrc;
    cv::Mat *tmpLines;
    cv::Mat *tmpModified;
    cv::Mat *modified;
    cv::Mat *monoseed;
    std::vector<cv::Point> *positions;

    double *mean;
    double *meanRGB;
    double *stdev;

    short* color;

    bool created;
    bool notdeleted;

    void displayImage(cv::Mat &m);
    void displayImage8UC1(cv::Mat &m);
    void texturesMatching();

private slots:
    virtual void resizeEvent(QResizeEvent *);

    void on_texturePreviewButton_clicked();
    void on_resetButton_clicked();
    void on_floodPreviewButton_clicked();
    void on_cannyFloodPreviewButton_clicked();
    void on_prespectiveAndCuttingButton_clicked();

    void on_fillHolesButton_clicked();
    void on_pushButton_clicked();
    void on_powerWatershedMaskpushButton_clicked();
    void on_powerWatershedpushButton_clicked();
};

#endif // ZONESEXTRACTOR_H
