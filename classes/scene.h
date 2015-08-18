#ifndef SCENE_H
#define SCENE_H

#include "view/floodfill.h"

#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <QGraphicsSceneMouseEvent>

#include <iostream>


//#include "view/floodfill.h"

//class FloodFill;

/**
 * @brief The Scene class
 */
class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    //explicit Scene(FloodFill *parent = 0);
    explicit Scene(cv::Mat &src, cv::Mat &tmpLines, cv::Mat &tmpModified, cv::Mat &modified, std::vector<cv::Point> &positions);
    ~Scene();

    /**
     * @brief Display original image.
     *
     * Display the original image. The original image does not have the "in progress"
     * transformations, as lines drawn or image treatments.
     */
    void displayImageSrc();

    /**
     * @brief Display temporary modified image.
     *
     * Display the modified image by the "in progress" treatments. The temporary
     * modified show the results of the different treatments without the drawn
     * lines to build them, and can hold new lines to make futher treatments.
     */
    void displayImageTempModified();


    /**
     * @brief Display modified image.
     *
     * Display the modified image. The modified image image show the results of
     * the treatments, without the lines drawn.
     */
    void displayImageModified();

    /**
     * @brief Set the drawing color.
     *
     * Set the color of the drawing tool with a [R,G,B] vector, which values
     * does not exceed 255 per element.
     * @param [in]  color color of the drawing tool [R,G,B].
     */
    void setColor(short *color);

    void setGetPositions();

    void unsetGetPositions();

    std::vector<cv::Point> getClickedPos();


private:
    short *color;
    short *counter;
    cv::Mat *src;
    cv::Mat *tmpLines;
    cv::Mat *tempModified;
    cv::Mat *modified;
    std::vector<cv::Point> *positions;
    std::vector<cv::Point> *clickedPos;
    QPointF *startPoint;
    QPointF *endPoint;
    cv::Scalar *fcolor;
    bool returnPositions;


protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // SCENE_H
