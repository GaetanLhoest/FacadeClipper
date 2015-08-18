#ifndef HOUGH_H
#define HOUGH_H

#include <QString>
#include <QDialog>
#include <QGraphicsScene>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>


namespace Ui {
class Hough;
}

/**
 * @brief The Hough class
 *
 * The Hough class contain a GUI to show the results of the Hough method with
 * different parameters.
 */
class Hough : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Hough constructor
     *
     * Constructor of the Hough class.
     *
     * @param im Name of the image file to work on
     * @param parent QWidget parent
     */
    explicit Hough(QString im, QWidget *parent = 0);
    ~Hough();

private slots:

    void on_pushButton_clicked();
    void resizeEvent(QResizeEvent *);

private:
    Ui::Hough *ui;
    cv::Mat *src;
    QGraphicsScene *scene;

    /**
     * @brief Standard Hough method
     *
     * Apply the standard Hough method and display it in the QDialog
     */
    void hStandard();

    /**
     * @brief Probabilistic Hough method
     *
     * Apply the probabilistic Hough method and display it in the QDialog
     */
    void hProbabilistic();

    /**
     * @brief Display Image
     *
     * Display the given opencv Mat in the QDialog
     *
     * @param m Opencv Mat to display
     */
    void displayImage(cv::Mat &m);
};

#endif // HOUGH_H
