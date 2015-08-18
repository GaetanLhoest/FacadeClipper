#ifndef FLOODFILL_H
#define FLOODFILL_H

#include <QDialog>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <iostream>

#include "classes/scene.h"

class Scene;

namespace Ui {
class FloodFill;
}

/**
 * @brief The FloodFill class
 *
 * The FloodFill class contain a GUI to show the results of the floodfill method
 * with different parameters.
 */
class FloodFill : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief FloodFill constructor
     *
     * Constructor of the FloodFill class.
     *
     * @param im Name of the image file to work on
     * @param parent QWidget parent
     */
    explicit FloodFill(QString im, QWidget *parent = 0);
    ~FloodFill();

    std::vector<cv::Point> *positions;


private:
    Ui::FloodFill *ui;
    Scene *scene;
    cv::Mat *src;
    cv::Mat *lines;
    cv::Mat *flood;
    cv::Mat *floodLines;
    QImage *qisrc;
    cv::Scalar *fcolor;

    short* color;

    /**
     * @brief Display Image
     *
     * Display the given file designated by th QString in the QDialog
     *
     * @param m Opencv Mat to display
     */
    void displayImage(const QString s);

    /**
     * @brief Display Image
     *
     * Display the given opencv Mat in the QDialog
     *
     * @param m Opencv Mat to display
     */
    void displayImage(cv::Mat &m);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    virtual void resizeEvent(QResizeEvent *);

};



#endif // FLOODFILL_H
