#ifndef SEMIAUTOMATICCLIPPER_H
#define SEMIAUTOMATICCLIPPER_H

#include <iostream>

#include "classes/scene2.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <QDialog>
#include <QString>

namespace Ui {
class SemiAutomaticClipper;
}

class SemiAutomaticClipper : public QDialog
{
    Q_OBJECT

public:
    explicit SemiAutomaticClipper(QString im, std::string *directory, QWidget *parent = 0);
    ~SemiAutomaticClipper();

private slots:
    void on_applyButton_clicked();
    void resizeEvent(QResizeEvent *);

    void on_cutButton_clicked();

    void on_doNotCutButton_clicked();

    void on_resetButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::SemiAutomaticClipper *ui;
    QString im;
    cv::Mat *src;
    cv::Mat *imsrc;
    cv::Mat *tmpModified;
    cv::Mat *modified;
    std::string *savedirectory;

    scene2 *scene;
    std::vector<cv::Point> *positions;

    void displayImage(cv::Mat &m);


};

#endif // SEMIAUTOMATICCLIPPER_H
