#include <QImage>

#include "scene2.h"

#include "methods/asmopencv.h"

using namespace std;
using namespace cv;
using namespace ASM;

scene2::scene2(Mat &tmpModified)
{
    this->tmpModified = &tmpModified;

    this->startPoint = new QPointF();
    this->clickedPos = new vector<Point>;
    this->endPoint = new QPointF();
    this->counter = new short;
    this->fcolor = new Scalar();
    this->positions = new vector<Point>;
    *counter = 0;
    returnPositions = false;
}

scene2::~scene2()
{
    delete startPoint;
    delete clickedPos;
    delete endPoint;
    delete counter;
    delete fcolor;
}

void scene2::setTmpModified(Mat &tmpModified)
{
    this->tmpModified = &tmpModified;
}

void scene2::displayImageTmpModified()
{
    QImage qi = mat_to_qimage_ref(*tmpModified, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    this->clear();
    this->addPixmap(im);
    this->update();
}

void scene2::setGetPositions()
{
    returnPositions = true;
}

void scene2::unsetGetPositions()
{
    returnPositions = false;
}

void scene2::setColor(short *color)
{
    *fcolor = Scalar(color[0], color[1], color[2]);
}

vector<Point> scene2::getClickedPos()
{
    return *clickedPos;
}

void scene2::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (returnPositions){
        QPointF pos = event->scenePos();
        Point p(int(pos.x()), int(pos.y()));
        clickedPos->push_back(p);
    }
    else{
        QPointF pos = event->scenePos();
        *startPoint = pos;
        *counter = 0;
    }
}

void scene2::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!returnPositions)
        *counter = *counter + 1;

    if (*counter == 10){
        QPointF pos = event->scenePos();
        *endPoint = pos;
        *counter = 0;
        Point s(int(startPoint->x()), int(startPoint->y()));
        Point e(int(endPoint->x()), int(endPoint->y()));
        line(*tmpModified, s, e,  *fcolor, 30);
        positions->push_back(e);
        positions->push_back(s);
        displayImageTmpModified();
        *startPoint = *endPoint;
    }
}

void scene2::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!returnPositions){
        QPointF pos = event->scenePos();
        *endPoint = pos;
        *counter = 0;
        Point s(int(startPoint->x()), int(startPoint->y()));
        Point e(int(endPoint->x()), int(endPoint->y()));
        line(*tmpModified, s, e,  *fcolor, 30);
        positions->push_back(e);
        positions->push_back(s);
        displayImageTmpModified();
        *startPoint = *endPoint;
    }
}
