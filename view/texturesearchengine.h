#ifndef TEXTURESEARCHENGINE_H
#define TEXTURESEARCHENGINE_H

#include "classes/scene.h"

#include <QDialog>
#include <QString>

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

namespace Ui {
class TextureSearchEngine;
}

class TextureSearchEngine : public QDialog
{
    Q_OBJECT

public:
    explicit TextureSearchEngine(QString im, QWidget *parent = 0);
    ~TextureSearchEngine();

private slots:
    virtual void resizeEvent(QResizeEvent *);

    void on_applyButton_clicked();

    void on_resetButton_clicked();

private:
    Ui::TextureSearchEngine *ui;
    QString im;
    Scene *scene;
    cv::Mat *src;
    cv::Mat *tmpLines;
    cv::Mat *tmpModified;
    cv::Mat *modified;
    std::vector<cv::Point> *positions;

    double *mean;
    double *meanRGB;
    double *stdev;

    bool created;


    short *color;

    void displayImage(cv::Mat &m);
};

#endif // TEXTURESEARCHENGINE_H
