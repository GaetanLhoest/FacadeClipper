#include <QImage>

#include "scene.h"

#include "methods/asmopencv.h"



using namespace std;
using namespace cv;
using namespace ASM;


Scene::Scene(Mat &src, Mat &tmpLines, Mat &tempModified, Mat &modified, vector<Point> &positions)
{

    this->src = &src;
    this->tmpLines = &tmpLines;
    this->tempModified = &tempModified;
    this->modified = &modified;
    this->positions = &positions;
    this->returnPositions = false;

    this->startPoint = new QPointF();
    this->clickedPos = new vector<Point>;
    this->endPoint = new QPointF();
    this->counter = new short;
    this->fcolor = new Scalar();
    *counter = 0;

}

Scene::~Scene()
{
    delete startPoint;
    delete clickedPos;
    delete endPoint;
    delete counter;
    delete fcolor;
}

void Scene::displayImageSrc(){
    QImage qi = mat_to_qimage_ref(*src, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    this->clear();
    this->addPixmap(im);
    this->update();
}

void Scene::displayImageTempModified(){
    QImage qi = mat_to_qimage_ref(*tempModified, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    this->clear();
    this->addPixmap(im);
    this->update();
}

void Scene::displayImageModified(){
    QImage qi = mat_to_qimage_ref(*modified, QImage::Format_RGB888);
    qi = qi.rgbSwapped();
    QPixmap im = QPixmap::fromImage(qi);
    this->clear();
    this->addPixmap(im);
    this->update();
}

void Scene::setColor(short *color){
    *fcolor = Scalar(color[0], color[1], color[2]);
}

void Scene::setGetPositions(){
    returnPositions = true;
}

void Scene::unsetGetPositions(){
    returnPositions = false;
}

vector<Point> Scene::getClickedPos()
{
    return *clickedPos;
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
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

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!returnPositions)
        *counter = *counter + 1;

    if (*counter == 10){
        QPointF pos = event->scenePos();
        *endPoint = pos;
        *counter = 0;
        Point s(int(startPoint->x()), int(startPoint->y()));
        Point e(int(endPoint->x()), int(endPoint->y()));
        line(*tmpLines, s, e,  *fcolor, 30);
        line(*tempModified, s, e,  *fcolor, 30);
        positions->push_back(e);
        positions->push_back(s);
        displayImageTempModified();
        *startPoint = *endPoint;
    }

}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!returnPositions){
        QPointF pos = event->scenePos();
        *endPoint = pos;
        *counter = 0;
        Point s(int(startPoint->x()), int(startPoint->y()));
        Point e(int(endPoint->x()), int(endPoint->y()));
        line(*tmpLines, s, e,  *fcolor, 30);
        line(*tempModified, s, e,  *fcolor, 30);
        positions->push_back(e);
        positions->push_back(s);
        displayImageTempModified();
        *startPoint = *endPoint;
    }
}
