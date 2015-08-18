#ifndef SCENE2_H
#define SCENE2_H

#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <QGraphicsSceneMouseEvent>

#include <iostream>

class scene2 : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit scene2(cv::Mat &tmpModified);
    ~scene2();

    void setTmpModified(cv::Mat &tmpModified);
    void displayImageTmpModified();
    void setGetPositions();
    void unsetGetPositions();
    void setColor(short *color);
    std::vector<cv::Point> getClickedPos();

private:
    cv::Scalar *fcolor;
    short *counter;
    cv::Mat *tmpModified;
    std::vector<cv::Point> *positions;
    std::vector<cv::Point> *clickedPos;
    QPointF *startPoint;
    QPointF *endPoint;
    bool returnPositions;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // SCENE2_H
