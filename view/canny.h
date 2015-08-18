#ifndef CANNY_H
#define CANNY_H

#include <QDialog>
#include <QString>
#include <QGraphicsScene>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

namespace Ui {
class Canny;
}

/**
 * @brief The Canny class
 *
 * The canny class contain a GUI to show the results of the Canny method with
 * different parametes
 */
class Canny : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Canny constructor
     *
     * Constructor of the Canny class.
     *
     * @param im Name of the image file to work on
     * @param parent QWidget parent
     */
    explicit Canny(QString im, QWidget *parent = 0);
    ~Canny();

private slots:
    void on_pushButton_clicked();
    void resizeEvent(QResizeEvent *);

private:
    Ui::Canny *ui;
    cv::Mat *src;
    QGraphicsScene *scene;

    /**
     * @brief Display Image
     *
     * Display the given opencv Mat in the QDialog
     *
     * @param m Opencv Mat to display
     */
    void displayImage(cv::Mat &m);

};

#endif // CANNY_H
