#ifndef PERSPECTIVECORRECTION_H
#define PERSPECTIVECORRECTION_H



#include <QDialog>
#include <QString>
#include <QGraphicsScene>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

namespace Ui {
class PerspectiveCorrection;
}

/**
 * @brief The PerspectiveCorrection class
 *
 * The PerspectiveCorrection class contain a GUI to show the results of a method
 * designed to rectify deformation induced by perspective on facades images.
 */
class PerspectiveCorrection : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief PerspectiveCorrection constructor
     *
     * Constructor of the PerspectiveCorrection class.
     *
     * @param im Name of the image file to work on
     * @param parent QWidget parent
     */
    explicit PerspectiveCorrection(QString im, QWidget *parent = 0);
    ~PerspectiveCorrection();

private slots:
    void on_pushButton_h_clicked();
    void on_pushButton_el_clicked();
    void on_pushButton_vh_clicked();
    void on_pushButton_apply_clicked();
    void on_pushButton_4ip_clicked();
    void resizeEvent(QResizeEvent *);

private:
    Ui::PerspectiveCorrection *ui;
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

#endif // PERSPECTIVECORRECTION_H
