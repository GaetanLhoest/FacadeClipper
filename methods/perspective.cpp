#include "perspective.h"
#include <math.h>

#include "methods/treatments.h"

using namespace cv;
using namespace std;

bool isVH(Vec4i line){
    float x1 = (float)line[0];
    float y1 = (float)line[1];
    float x2 = (float)line[2];
    float y2 = (float)line[3];

    if ((x2 - x1) != 0){
        float m = (y2 - y1)/(x2 - x1);
        float theta = atan(m);
        if (theta > MINTHETAV or theta < MAXTHETAV)
            return true;
        else if(theta > MINTHETAH and theta < MAXTHETAH)
            return true;
        else
            return false;
    }
    else
        return true;
}

short isHOrV(Vec4i line){
    int x1 = line[0];
    int y1 = line[1];
    int x2 = line[2];
    int y2 = line[3];

    if ((x2 - x1) != 0){
        float m = (y2 - y1)/(x2 - x1);
        float theta = atan(m);
        if (theta > MINTHETAV or theta < MAXTHETAV)
            return VERTICAL;
        else if(theta > MINTHETAH and theta < MAXTHETAH)
            return HORIZONTAL;
        else
            return SKEW;
    }
    else
        return VERTICAL;
}


Vec2i findIntersection(Vec4i a, Vec4i b){
    long long x1 = (long long)a[0];
    long long y1 = (long long)a[1];
    long long x2 = (long long)a[2];
    long long y2 = (long long)a[3];
    long long x3 = (long long)b[0];
    long long y3 = (long long)b[1];
    long long x4 = (long long)b[2];
    long long y4 = (long long)b[3];

    long double num1 = (((x1*y2)-(y1*x2))*(x3-x4))-((x1-x2)*((x3*y4)-(y3*x4)));
    long double num2 = (((x1*y2)-(y1*x2))*(y3-y4))-((y1-y2)*((x3*y4)-(y3*x4)));
    long double den = ((x1-x2)*(y3-y4))-((y1-y2)*(x3-x4));

    Vec2i inters;
    if (den == 0){
        inters[0] = -1;
        inters[1] = -1;
    }
    else {
        inters[0] = int(num1/den);
        inters[1] = int(num2/den);
    }

    return inters;
}

void sortBorder(Vec4i &border, short code){
    /* Order:
     * lefter point first for horizontal lines
     * upper point first for vertical lines
     */
    if (code == TOP){
        if (border[0] > border[2]){
            Vec4i tmp(border[2], border[3], border[0], border[1]);
            border = tmp;
        }
    }
    else if (code == RIGHT){
        if (border[1] > border[3]){
            Vec4i tmp(border[2], border[3], border[0], border[1]);
            border = tmp;
        }
    }
    else if (code == BOT){
        if (border[0] > border[2]){
            Vec4i tmp(border[2], border[3], border[0], border[1]);
            border = tmp;
        }
    }
    else{
        if (border[1] > border[3]){
            Vec4i tmp(border[2], border[3], border[0], border[1]);
            border = tmp;
        }
    }
}

bool isQuadrilateral(Vec4i topquad, Vec4i rightquad, Vec4i botquad, Vec4i leftquad){

    if ((topquad[0] != leftquad[0]) or (topquad[1] != leftquad[1]))
        return false;
    if ((topquad[2] != rightquad[0]) or (topquad[3] != rightquad[1]))
        return false;
    if ((botquad[0] != leftquad[2]) or (botquad[1] != leftquad[3]))
        return false;
    if ((botquad[2] != rightquad[2]) or (botquad[3] != rightquad[3]))
        return false;
    return true;
}

long distClosestCorner(Vec2i point, int w, int h, short &corner){
    long shortestdist;
    long tmp;

    shortestdist = distance2p(point, Vec2i(0,0));
    corner = TOPLEFT;

    tmp = distance2p(point, Vec2i(w,0));
    if (tmp < shortestdist){
        shortestdist = tmp;
        corner = TOPRIGHT;
    }

    tmp = distance2p(point, Vec2i(0,h));
    if (tmp < shortestdist){
        shortestdist = tmp;
        corner = BOTLEFT;
    }

    tmp = distance2p(point, Vec2i(w,h));
    if (tmp < shortestdist){
        shortestdist = tmp;
        corner = BOTRIGHT;
    }

    return shortestdist;

}

long distance2p(Vec2i a, Vec2i b){
    double x1 = (double)a[0];
    double y1 = (double)a[1];
    double x2 = (double)b[0];
    double y2 = (double)b[1];

    double dist = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));

    return (long)dist;

}

void getExtremitiesIntersections(vector<Vec4i>& lines,
                                 int w,
                                 int h,
                                 vector<Vec2i>& extr){

    Vec4i top(0,0,w,0);
    Vec4i right(w,0,w,h);
    Vec4i bot(0,h,w,h);
    Vec4i left(0,0,0,h);

    for (size_t i = 0; i < lines.size(); i++){
        Vec2i x;
        x = findIntersection(lines[i], top);
        if (x[0] > 0 && x[0] < w)
            extr.push_back(x);

        x = findIntersection(lines[i], right);
        if (x[1] > 0 && x[1] < h)
            extr.push_back(x);

        x = findIntersection(lines[i], bot);
        if (x[0] > 0 && x[0] < w)
            extr.push_back(x);

        x = findIntersection(lines[i], left);
        if (x[1] > 0 && x[1] < h)
            extr.push_back(x);
    }
}

void getVHIntersection(vector<Vec4i>& vhlines,
                       Vec2i** inters,
                       vector<short>& vhcodes,
                       int w,
                       int h){

    for (size_t i = 0; i < vhlines.size(); i++){
        for (size_t j = 0; j < vhlines.size(); j++){
            Vec2i x = findIntersection(vhlines[i], vhlines[j]);
            if ((x[0] < 0 || x[0] > w || x[1] < 0 || x[1] > h) || vhcodes[i] == vhcodes[j]){
                x[0] = -1;
                x[1] = -1;
            }
            inters[i][j] = x;
        }
    }
}

void nearestCornerDistances(vector<Vec4i>& vhlines,
                            Vec2i** inters,
                            long** distances,
                            short** cornercodes,
                            int w,
                            int h){

    short cornercode = -1;

    for (size_t i = 0; i < vhlines.size(); i++){
        for (size_t j = 0; j < vhlines.size(); j++){
            if (inters[i][j][0] != -1){
                distances[i][j] = distClosestCorner(inters[i][j], w, h, cornercode);
                cornercodes[i][j] = cornercode;
            }
            else{
                distances[i][j] = -1;
                cornercodes[i][j] = -1;
            }
        }
    }
}


void getExtremitiesData(vector<Vec4i>& vhlines,
                        Vec2i extremities[][2],
                        short extrcornercodes[][2],
                        long extrdistance[],
                        long** distances,
                        short** cornercodes,
                        Vec2i** inters){

    long dist, dist1, dist2;
    short code, code1, code2;
    Vec2i pt1, pt2;
    bool twoPoints;
    for (size_t i = 0; i < vhlines.size(); i++){
        dist = -1;
        dist1 = -1;
        dist2 = -1;
        twoPoints = false;
        for (size_t j = 0; j < vhlines.size(); j++){
            dist = distances[i][j];
            code = cornercodes[i][j];
            if (dist != -1){
                if (dist1 == -1){
                    dist1 = dist;
                    pt1 = inters[i][j];
                    code1 = code;
                }
                else if ((dist < dist1) && (code == code1)){
                    dist1 = dist;
                    pt1 = inters[i][j];
                }
                else if (((dist < dist2) or (dist2 == -1)) && (code != code1)){
                    dist2 = dist;
                    pt2 = inters[i][j];
                    code2 = code;
                    twoPoints = true;
                }
            }
        }
        if (twoPoints){
            extremities[i][0] = pt1;
            extremities[i][1] = pt2;
            extrcornercodes[i][0] = code1;
            extrcornercodes[i][1] = code2;
            extrdistance[i] = dist1 + dist2;
        }
        else{
            extremities[i][0] = Vec2i(-1,-1);
            extremities[i][1] = Vec2i(-1,-1);
            extrcornercodes[i][0] = NOVALUE;
            extrcornercodes[i][1] = NOVALUE;
            extrdistance[i] = -1;
        }

    }

}

void getBorderCodes(vector<Vec4i>& vhlines,
                    short bordercodes[],
                    short extrcornercodes[][2]){

    for (size_t i = 0; i < vhlines.size(); i++){
        if (extrcornercodes[i][0] != NOVALUE){

            if (extrcornercodes[i][0] == TOPLEFT){
                if (extrcornercodes[i][1] == TOPRIGHT)
                    bordercodes[i] = TOP;
                else if( extrcornercodes[i][1] == BOTLEFT)
                    bordercodes[i] = LEFT;
            }

            else if (extrcornercodes[i][0] == TOPRIGHT){
                if (extrcornercodes[i][1] == TOPLEFT)
                    bordercodes[i] = TOP;
                else if( extrcornercodes[i][1] == BOTRIGHT)
                    bordercodes[i] = RIGHT;
            }

            else if (extrcornercodes[i][0] == BOTLEFT){
                if (extrcornercodes[i][1] == BOTRIGHT)
                    bordercodes[i] = BOT;
                else if( extrcornercodes[i][1] == TOPLEFT)
                    bordercodes[i] = LEFT;
            }

            else if (extrcornercodes[i][0] == BOTRIGHT){
                if (extrcornercodes[i][1] == TOPRIGHT)
                    bordercodes[i] = RIGHT;
                else if( extrcornercodes[i][1] == BOTLEFT)
                    bordercodes[i] = BOT;
            }

            else
                bordercodes[i] = NOVALUE;
        }
    }
}

void getNearestBordersLines(vector<Vec4i>& vhlines,
                            Vec2i extremities[][2],
                            Vec4i quadborders[4],
                            long borderdistances[4],
                            long extrdistance[],
                            short bordercodes[]){
    Vec4i topquad;
    Vec4i rightquad;
    Vec4i botquad;
    Vec4i leftquad;

    long topdist =  std::numeric_limits<long>::max();
    long botdist = std::numeric_limits<long>::max();
    long leftdist = std::numeric_limits<long>::max();
    long rightdist = std::numeric_limits<long>::max();
    for (size_t i = 0; i < vhlines.size(); i++){

        if ((bordercodes[i] == TOP) and
            (extrdistance[i] > 0) and
            (extrdistance[i] < topdist)){
            topquad = Vec4i(extremities[i][0][0],
                        extremities[i][0][1],
                        extremities[i][1][0],
                        extremities[i][1][1]);
            topdist = extrdistance[i];
        }

        else if ((bordercodes[i] == RIGHT) and
            (extrdistance[i] > 0) and
            (extrdistance[i] < rightdist)){
            rightquad = Vec4i(extremities[i][0][0],
                        extremities[i][0][1],
                        extremities[i][1][0],
                        extremities[i][1][1]);
            rightdist = extrdistance[i];
        }

        else if ((bordercodes[i] == BOT) and
            (extrdistance[i] > 0) and
            (extrdistance[i] < botdist)){
            botquad = Vec4i(extremities[i][0][0],
                        extremities[i][0][1],
                        extremities[i][1][0],
                        extremities[i][1][1]);
            botdist = extrdistance[i];
        }

        else if ((bordercodes[i] == LEFT) and
            (extrdistance[i] > 0) and
            (extrdistance[i] < leftdist)){
            leftquad = Vec4i(extremities[i][0][0],
                        extremities[i][0][1],
                        extremities[i][1][0],
                        extremities[i][1][1]);
            leftdist = extrdistance[i];
        }
    }

    quadborders[0] = topquad;
    quadborders[1] = rightquad;
    quadborders[2] = botquad;
    quadborders[3] = leftquad;

    borderdistances[0] = topdist;
    borderdistances[1] = rightdist;
    borderdistances[2] = botdist;
    borderdistances[3] = leftdist;
}

void findShorterQuadrilateral(vector<Vec4i>& vhlines,
                              Vec2i extremities[][2],
                              Vec4i quadborders[4],
                              long extrdistance[],
                              long borderdistances[4],
                              int w,
                              int h,
                              short bordercodes[]){


    Vec2i corner;

    corner = findIntersection(quadborders[0], quadborders[3]);
    if (corner[0] < 0 or corner[1] < 0)
        replaceQuadrilateralBorder(vhlines,
                                   extremities,
                                   quadborders,
                                   extrdistance,
                                   borderdistances,
                                   TOPLEFT,
                                   bordercodes);

    corner = findIntersection(quadborders[0], quadborders[1]);
    if (corner[0] > w or corner[1] < 0)
        replaceQuadrilateralBorder(vhlines,
                                   extremities,
                                   quadborders,
                                   extrdistance,
                                   borderdistances,
                                   TOPRIGHT,
                                   bordercodes);

    corner = findIntersection(quadborders[1], quadborders[2]);
    if (corner[0] > w or corner[1] > h)
        replaceQuadrilateralBorder(vhlines,
                                   extremities,
                                   quadborders,
                                   extrdistance,
                                   borderdistances,
                                   BOTRIGHT,
                                   bordercodes);

    corner = findIntersection(quadborders[2], quadborders[3]);
    if (corner[0] < 0 or corner[1] > h)
        replaceQuadrilateralBorder(vhlines,
                                   extremities,
                                   quadborders,
                                   extrdistance,
                                   borderdistances,
                                   BOTLEFT,
                                   bordercodes);

    redefineQuadrilateral(quadborders, w, h);



}

void redefineQuadrilateral(Vec4i quadborders[4],
                           int w,
                           int h){
    Vec2i tmp1, tmp2;
    Vec4i tmptop, tmpright, tmpbot, tmpleft;

    tmp1 = findIntersection(quadborders[TOP], quadborders[LEFT]);
    tmp2 = findIntersection(quadborders[TOP], quadborders[RIGHT]);

    if (isInPicture(tmp1, w, h) and isInPicture(tmp2, w, h)){
        tmptop = Vec4i(tmp1[0], tmp1[1], tmp2[0], tmp2[1]);
    }

    tmp1 = findIntersection(quadborders[RIGHT], quadborders[TOP]);
    tmp2 = findIntersection(quadborders[RIGHT], quadborders[BOT]);

    if (isInPicture(tmp1, w, h) and isInPicture(tmp2, w, h)){
        tmpright = Vec4i(tmp1[0], tmp1[1], tmp2[0], tmp2[1]);
    }

    tmp1 = findIntersection(quadborders[BOT], quadborders[LEFT]);
    tmp2 = findIntersection(quadborders[BOT], quadborders[RIGHT]);

    if (isInPicture(tmp1, w, h) and isInPicture(tmp2, w, h)){
        tmpbot = Vec4i(tmp1[0], tmp1[1], tmp2[0], tmp2[1]);
    }

    tmp1 = findIntersection(quadborders[LEFT], quadborders[TOP]);
    tmp2 = findIntersection(quadborders[LEFT], quadborders[BOT]);

    if (isInPicture(tmp1, w, h) and isInPicture(tmp2, w, h)){
        tmpleft = Vec4i(tmp1[0], tmp1[1], tmp2[0], tmp2[1]);
    }

    quadborders[TOP] = tmptop;
    quadborders[RIGHT] = tmpright;
    quadborders[BOT] = tmpbot;
    quadborders[LEFT] = tmpleft;

}

bool isInPicture(Vec2i& p, int w, int h){
    if (p[0] < 0 or p[0] > w)
        return false;

    if (p[1] < 0 or p[1] > h)
        return false;

    return true;
}

void replaceQuadrilateralBorder(vector<Vec4i>& vhlines,
                                Vec2i extremities[][2],
                                Vec4i quadborders[4],
                                long extrdistance[],
                                long borderdistances[4],
                                short code,
                                short bordercodes[]){

    long tmpDist1;
    long tmpDist2;
    if (code == TOPLEFT){
        Vec4i tmp1 = findNewQuadrilateralBorder(vhlines,
                                                extremities,
                                                TOP,
                                                borderdistances[TOP],
                                                tmpDist1,
                                                bordercodes,
                                                extrdistance);
        Vec4i tmp2 = findNewQuadrilateralBorder(vhlines,
                                                extremities,
                                                LEFT,
                                                borderdistances[LEFT],
                                                tmpDist2,
                                                bordercodes,
                                                extrdistance);
        if (tmpDist1 < tmpDist2){
            quadborders[TOP] = tmp1;
            borderdistances[TOP] = tmpDist1;
        }
        else{
            quadborders[LEFT] = tmp2;
            borderdistances[LEFT] = tmpDist2;
        }

    }
    else if (code == TOPRIGHT){
        Vec4i tmp1 = findNewQuadrilateralBorder(vhlines,
                                                extremities,
                                                TOP,
                                                borderdistances[TOP],
                                                tmpDist1,
                                                bordercodes,
                                                extrdistance);
        Vec4i tmp2 = findNewQuadrilateralBorder(vhlines,
                                                extremities,
                                                RIGHT,
                                                borderdistances[RIGHT],
                                                tmpDist2,
                                                bordercodes,
                                                extrdistance);
        if (tmpDist1 < tmpDist2){
            quadborders[TOP] = tmp1;
            borderdistances[TOP] = tmpDist1;
        }
        else{
            quadborders[RIGHT] = tmp2;
            borderdistances[RIGHT] = tmpDist2;
        }
    }
    else if (code == BOTLEFT){
        Vec4i tmp1 = findNewQuadrilateralBorder(vhlines,
                                                extremities,
                                                BOT,
                                                borderdistances[BOT],
                                                tmpDist1,
                                                bordercodes,
                                                extrdistance);
        Vec4i tmp2 = findNewQuadrilateralBorder(vhlines,
                                                extremities,
                                                LEFT,
                                                borderdistances[LEFT],
                                                tmpDist2,
                                                bordercodes,
                                                extrdistance);
        if (tmpDist1 < tmpDist2){
            quadborders[BOT] = tmp1;
            borderdistances[BOT] = tmpDist1;
        }
        else{
            quadborders[LEFT] = tmp2;
            borderdistances[LEFT] = tmpDist2;
        }
    }
    else {
        Vec4i tmp1 = findNewQuadrilateralBorder(vhlines,
                                                extremities,
                                                BOT,
                                                borderdistances[BOT],
                                                tmpDist1,
                                                bordercodes,
                                                extrdistance);
        Vec4i tmp2 = findNewQuadrilateralBorder(vhlines,
                                                extremities,
                                                RIGHT,
                                                borderdistances[RIGHT],
                                                tmpDist2,
                                                bordercodes,
                                                extrdistance);
        if (tmpDist1 < tmpDist2){
            quadborders[BOT] = tmp1;
            borderdistances[BOT] = tmpDist1;
        }
        else{
            quadborders[RIGHT] = tmp2;
            borderdistances[RIGHT] = tmpDist2;
        }
    }

}

Vec4i findNewQuadrilateralBorder(vector<Vec4i>& vhlines,
                                 Vec2i extremities[][2],
                                 short code,
                                 long oldDist,
                                 long& newdist,
                                 short bordercodes[],
                                 long extrdistance[]){
    Vec4i newBorder;

    long tmpDist = std::numeric_limits<long>::max();
    for (size_t i = 0; i < vhlines.size(); i++){

        if ((bordercodes[i] == code) and
            (extrdistance[i] > 0) and
            (extrdistance[i] > oldDist) and
            (extrdistance[i] < tmpDist) ){
            newBorder = Vec4i(extremities[i][0][0],
                        extremities[i][0][1],
                        extremities[i][1][0],
                        extremities[i][1][1]);
            tmpDist = extrdistance[i];
        }

    }
    newdist = tmpDist;

    return newBorder;

}

void findShortestRectangle(Vec2i quadcorners[4], Vec2i rectangle[4], int w, int h){

    int tmpwL;
    int tmpwR;
    int tmphT;
    int tmphB;

    tmpwL = quadcorners[TOPLEFT][0];
    tmphT = quadcorners[TOPLEFT][1];

    tmpwR = w - quadcorners[BOTRIGHT][0];
    tmphB = h - quadcorners[BOTRIGHT][1];

    if (tmpwL < quadcorners[BOTLEFT][0])
        tmpwL = quadcorners[BOTLEFT][0];

    if (tmphT < quadcorners[TOPRIGHT][1])
        tmphT = quadcorners[TOPRIGHT][1];

    if (tmpwR < (w - quadcorners[TOPRIGHT][0]))
        tmpwR = w - quadcorners[TOPRIGHT][0];

    if (tmphB < (h - quadcorners[BOTLEFT][1]))
        tmphB = h - quadcorners[BOTLEFT][1];



    rectangle[TOPLEFT][0] = tmpwL;
    rectangle[TOPLEFT][1] = tmphT;

    rectangle[TOPRIGHT][0] = w - tmpwR;
    rectangle[TOPRIGHT][1] = tmphT;

    rectangle[BOTLEFT][0] = tmpwL;
    rectangle[BOTLEFT][1] = h - tmphB;

    rectangle[BOTRIGHT][0] = w - tmpwR;
    rectangle[BOTRIGHT][1] = h - tmphB;

}

void verticalCut2Points(Mat *src, Point p1, Point p2)
{
    Mat edges;
    vector<Vec4i> lines;



    edges = mhProbabilisticv(src, 200, 40, lines);


    vector<Vec2i> extr;
    int w = edges.cols;
    int h = edges.rows;

    getExtremitiesIntersections(lines, w, h, extr);

    vector<Vec4i> vectLines;

    Mat fullLines = src->clone();
    for( size_t i = 0; i < extr.size(); i+=2 )
    {
        Vec4i e(extr[i][0], extr[i][1], extr[i+1][0], extr[i+1][1]);

        if (isHOrV(e) == VERTICAL){
            vectLines.push_back(e);
        }
    }


    vector<Vec2i> vanPoint;
    double counter = 0;

    for (size_t i = 0; i < vectLines.size(); i++){
        for (size_t j = i + 1; j < vectLines.size(); j++){
            vanPoint.push_back(findIntersection(vectLines[i], vectLines[j]));
            counter += 1;
        }
    }

    Vec2d vaniPoint = Vec2d(0,0);
    for (size_t i = 0; i < vanPoint.size(); i++){
        vaniPoint[0] += (float)vanPoint[i][0] / counter;
        vaniPoint[1] += (float)vanPoint[i][1] / counter;
    }



    Point va;

    va.x = vaniPoint[0];
    va.y = vaniPoint[1];

    vector<Vec4i> newBorders;
    Vec4i border;
    border[0] = p1.x;
    border[1] = p1.y;
    border[2] = va.x;
    border[3] = va.y;

    newBorders.push_back(border);

    border[0] = p2.x;
    border[1] = p2.y;

    newBorders.push_back(border);

    vector<Vec2i> extr2;
    getExtremitiesIntersections(newBorders, w, h, extr2);

    Point left, right;
    if (p1.x < p2.x){
        left = p1;
        right = p2;
    }
    else {
        left = p2;
        right = p1;
    }

    Mat mask = Mat::zeros(src->size(), src->type());
    int ind = extr2.size() - 2;
    Vec4i e(extr2[ind][0], extr2[ind][1], extr2[ind+1][0], extr2[ind+1][1]);
    line( mask, Point(e[0], e[1]), Point(e[2], e[3]), Scalar(0,0,255), 10, CV_AA);
    ind = extr2.size() - 4;
    Vec4i f(extr2[ind][0], extr2[ind][1], extr2[ind+1][0], extr2[ind+1][1]);
    line( mask, Point(f[0], f[1]), Point(f[2], f[3]), Scalar(0,0,255), 10, CV_AA);

    Point flleft, flright;
    flleft.x = left.x / 2;
    flleft.y = left.y;
    flright.x = ((mask.cols - right.x) / 2) + right.x;
    flright.y = right.y;

    Scalar fcolor = Scalar(255, 255, 255);
    Rect ccomp;
    int connectivity = 4;
    int newMaskVal = 255;
    int ffillMode = 1;
    int flags = connectivity + (newMaskVal << 8) + (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);
    floodFill(mask, flleft, fcolor, &ccomp, Scalar(0, 0, 0), Scalar(0, 0, 0), flags);
    floodFill(mask, flright, fcolor, &ccomp, Scalar(0, 0, 0), Scalar(0, 0, 0), flags);


    unsigned char *input = (unsigned char*)(mask.data);
    unsigned char *output = (unsigned char*)(src->data);


    for (int j = 0; j < src->rows; j++){
        for (int i = 0; i < src->cols; i++){
            unsigned short b = input[(mask.cols * j * 3) + i*3 ];
            unsigned short g = input[(mask.cols * j * 3) + i*3 + 1];
            unsigned short r = input[(mask.cols * j * 3) + i*3 + 2];

            if (r == 255 and g == 255 and b == 255){
                output[(src->cols * j * 3) + i*3 ] = 255 ;
                output[(src->cols * j * 3) + i*3 + 1] = 255;
                output[(src->cols * j * 3) + i*3 + 2] = 255;
            }
        }
    }

}
