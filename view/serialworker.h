#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <iostream>

#include "classes/scene2.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <QDialog>
#include <QStringList>

namespace Ui {
class SerialWorker;
}

class SerialWorker : public QDialog
{
    Q_OBJECT

public:
    explicit SerialWorker(QStringList namelist, std::string *directory, QWidget *parent = 0);
    ~SerialWorker();

private slots:
    void resizeEvent(QResizeEvent *);

    void on_cutButton_clicked();

    void on_doNotCutButton_clicked();

    void on_resetButton_clicked();

    void on_nextButton_clicked();

    void checkAndCreateTmpDir();

    void on_applyButton_clicked();

private:
    Ui::SerialWorker *ui;
    int nameListIndex;

    cv::Mat *src;
    cv::Mat *imsrc;
    cv::Mat *tmpModified;
    cv::Mat *modified;
    std::string *savedirectory;
    QStringList nameList;
    std::string tmpDir;

    scene2 *scene;
    std::vector<cv::Point> *positions;

    void displayImage(cv::Mat &m);
    void formatSaveDirectory();
    std::string getTmpName();
    std::string getTmpSrcName();
    std::string getSaveName();
    void saveResults();

};

#endif // SERIALWORKER_H
