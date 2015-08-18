#ifndef POWERWATERSHED_H
#define POWERWATERSHED_H

#include <QDialog>
#include <QString>

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include "classes/scene.h"

namespace Ui {
class PowerWatershed;
}

/**
 * @brief The PowerWatershed class
 *
 * The power watershed class contain a GUI to show the results of the power
 * watershed method.
 */
class PowerWatershed : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief PowerWatershed constructor
     *
     * Constructor of the PowerWatershed class.
     *
     * @param im Name of the image file to work on
     * @param parent QWidget parent
     */
    explicit PowerWatershed(QString im, QWidget *parent = 0);
    ~PowerWatershed();

private:
    Ui::PowerWatershed *ui;

    std::vector<cv::Point> *positionsmul;
    std::vector<cv::Point> *positionsmono;

    Scene *scene;
    Scene *scenemul;
    Scene *scenemono;

    cv::Mat *src;

    cv::Mat *tmpLinesMul;
    cv::Mat *tmpModifiedMul;
    cv::Mat *modifiedMul;

    cv::Mat *tmpLinesMono;
    cv::Mat *tmpModifiedMono;
    cv::Mat *modifiedMono;

    cv::Mat *monoseed;
    cv::Mat *multiseeds;

    short museedvalue;
    short moseedvalue;

    short *color;

    /**
     * @brief Display Image
     *
     * Display the given opencv Mat int the QDialog
     *
     * @param m Opencv Mat to display
     */
    void displayImage(cv::Mat &m);

    /**
     * @brief Display Image 8UC1
     *
     * Display the given opencv 8UC1 Mat int the QDialog
     *
     * @param m Opencv 8UC1 Mat to display
     */
    void displayImage8UC1(cv::Mat &m);


private slots:
    void on_applyButton_clicked();
    void on_musRadio_clicked();
    void on_mosRadio_clicked();
    void on_addSButton_clicked();
    void on_objectButton_clicked();
    void on_maskButton_clicked();
    virtual void resizeEvent(QResizeEvent *);
};

#endif // POWERWATERSHED_H
