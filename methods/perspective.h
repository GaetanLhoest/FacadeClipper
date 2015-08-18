#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#define VERTICAL 0
#define HORIZONTAL 1
#define SKEW 2

#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTLEFT 2
#define BOTRIGHT 3

#define TOP 0
#define RIGHT 1
#define BOT 2
#define LEFT 3

#define NOVALUE 13

#define MINTHETAV 1.43116999 // 82 degrees
#define MAXTHETAV -1.43116999
#define MINTHETAH -0.13962634
#define MAXTHETAH 0.13962634

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <iostream>

/**
 * @brief Find intersection
 *
 * Find the intersection point between two lines. If the lines are parallel,
 * the point returned have the coordinates (-1,-1).
 *
 * @param a [in]    First line defined by two points
 * @param b [in]    Second line defined by two points
 * @return The intersection point
 */
cv::Vec2i findIntersection(cv::Vec4i a, cv::Vec4i b);

/**
 * @brief Is vertical or horizontal
 *
 * Check if the line have a vertical or horizontal properties. If the line is
 * vertical or horizontal with a 10° treshold, the function return true.
 * Otherwise, the function return false.
 *
 * @param line  [in]    The line we want to check, defined by two points.
 * @return True if the line have vertical or horizontal properties, false
 *         otherwise.
 */
bool isVH(cv::Vec4i line);

/**
 * @brief Is horizontal or vertical
 *
 * Return HORIZONTAL if the line have horizontal properties, VERTICAL if the
 * line have vertical properties, SKEW otherwise.
 *
 * @param line  [in]    The line we want to know the propertie, defined by two
 *                      points.
 * @return The code HORIZONTAL, VERTICAL or SKEW
 */
short isHOrV(cv::Vec4i line);

/**
 * @brief Distance to closest corner
 *
 * Compute the distance of a point to the nearest corner of the image
 *
 * @param point     [in]    The point we are searching the distance to the
 *                          closest corner
 * @param w         [in]    The width of the image
 * @param h         [in]    The height of the image
 * @param corner    [in]    The closest corner, designed by the code TOPLEFT,
 *                          TOPRIGHT, BOTLEFT or BOTRIGHT
 * @return The distance of the point to the closest corner
 */
long distClosestCorner(cv::Vec2i point, int w, int h, short &corner);

/**
 * @brief Distance between two points
 *
 * Compute the distance between two points.
 *
 * @param a     [in]    First point
 * @param b     [in]    Second point
 * @return The distance between the two points
 */
long distance2p(cv::Vec2i a, cv::Vec2i b);

/**
 * @brief Sort borders
 *
 * Take a vector containing two points, representing the border of a
 * quadrilateral. If the vector have a vertical property, the upper point is
 * placed first in the vector. If the vector have an horisontal property, the
 * lefter point is placed first in the vector.
 *
 * @param border    [in, out]   The vector containing the two points
 * @param code      [in]        Position of the border in the quadrilateral,
 *                              designed by the code TOP, BOT, LEFT or RIGHT
 */
void sortBorder(cv::Vec4i &border, short code);

/**
 * @brief Is quadrilateral
 *
 * Check if the 4 segments given form a quadrilateral. If it is the cas, true is
 * returned, else, false is returned.
 *
 * @param topquad   [in]    Top border of the quadrilateral
 * @param rightquad [in]    Right border of the quadrilateral
 * @param botquad   [in]    Bottom border of the quadrilateral
 * @param leftquad  [in]    Left border of the quadrilateral
 * @return True if it is a quadrilateral, false otherwise
 */
bool isQuadrilateral(cv::Vec4i topquad,
                     cv::Vec4i rightquad,
                     cv::Vec4i botquad,
                     cv::Vec4i leftquad);

/**
 * @brief Get extremities intersections
 *
 * Return the intersections points of the lines with the borders of the image.
 *
 * @param lines [in]    List of all the lines
 * @param w     [in]    The width of the image
 * @param h     [in]    The height of the image
 * @param extr  [out]   List of the intersections of the lines with the image
 *                      borders
 */
void getExtremitiesIntersections(std::vector<cv::Vec4i>& lines,
                                 int w,
                                 int h,
                                 std::vector<cv::Vec2i>& extr);

/**
 * @brief Get vertical and horizontal lines intersections
 *
 * Return the intersections between lines having different vertical or
 * horizontal properties. The intersection between two lines having the same
 * property (both having a vertical property or both having an horizontal
 * property) will not be returned.
 *
 * @param vhlines   [in]    Set of lines having vertical or horizontal
 *                          properties
 * @param inters    [out]   Set of intersections between the lines
 * @param vhcodes   [in]    Corresponding code (VERTICAL or HORIZONTAL) for
 *                          "vhlines"
 * @param w         [in]    The width of the image
 * @param h         [in]    The height of the image
 */
void getVHIntersection(std::vector<cv::Vec4i>& vhlines,
                       cv::Vec2i** inters,
                       std::vector<short>& vhcodes,
                       int w,
                       int h);

/**
 * @brief Get nearest corner distance
 *
 * Get the list of corresponding distance to the closest corner givent the list
 * of intersections, and the list of codes of the closest corner.
 *
 * @param vhlines       [in]    The set of lines having vertical and horizontal
 *                              properties
 * @param inters        [in]    The list of intersections
 * @param distances     [out]   The list of distances to the closest corner
 * @param cornercodes   [out]   The list of codes of the closest corner
 * @param w             [in]    The width of the image
 * @param h             [in]    The height of the image
 */
void nearestCornerDistances(std::vector<cv::Vec4i>& vhlines,
                            cv::Vec2i** inters,
                            long** distances,
                            short** cornercodes,
                            int w,
                            int h);

/**
 * @brief Get extremities data
 *
 * For each lines, in the sets of its intersections with other lines, this
 * function return data about two intersections by lines, wich are the nearest
 * to the image borders. The data returned is the positions of the intersections
 * nears the image borders, the code of the closest corners to these points and
 * the sum of the distances of the two selected intersections with their nearest
 * corners.
 *
 * @param vhlines           [in]    The set of lines having vertical and
 *                                  horizontal properties
 * @param extremities       [out]   The set of intersections near the image
 *                                  borders
 * @param extrcornercodes   [out]   The set of codes of the nearest corners
 *                                  of the selected intersections
 * @param extrdistance      [out]   The set of sums of the distances of the
 *                                  intersections to their nearest corners
 * @param distances         [out]   The set of distances of the intersections
 *                                  to their nearest corners
 * @param cornercodes       [in]    The set of codes of the nearest corners of
 *                                  the intersections
 * @param inters            [in]    The set of intersections
 */
void getExtremitiesData(std::vector<cv::Vec4i>& vhlines,
                        cv::Vec2i extremities[][2],
                        short extrcornercodes[][2],
                        long extrdistance[],
                        long** distances,
                        short** cornercodes,
                        cv::Vec2i** inters);

/**
 * @brief Get borders codes
 *
 * Get the borders of a quadrilateral code (designed by TOP, RIGHT, BOT, LEFT)
 *
 * @param vhlines           [in]    The set of lines having vertical and
 *                                  horizontal properties
 * @param bordercodes       [out]   The set of codes of the borders
 * @param extrcornercodes   [in]    The set of codes of the extremities
 *                                  intersections of a line
 */
void getBorderCodes(std::vector<cv::Vec4i>& vhlines,
                    short bordercodes[],
                    short extrcornercodes[][2]);

/**
 * @brief Get nearest borders lines
 *
 * Get the lines that are the closest to the image border (one for the top,
 * one for the right, one for the bottom and one for the right)
 *
 * @param vhlines           [in]    The set of lines having vertical and
 *                                  horizontal properties
 * @param extremities       [in]    The set of intersections near the image
 *                                  border
 * @param quadborders       [out]   The four lines nearest the image borders
 * @param borderdistances   [out]   The distance of the selected lines to the
 *                                  image borders
 * @param extrdistance      [in]    The distance of the lines to the image
 *                                  borders
 * @param bordercodes       [in]    The set of code of the lines related to the
 *                                  nearest image border (designed by TOP,
 *                                  RIGHT, BOT and LEFT)
 */
void getNearestBordersLines(std::vector<cv::Vec4i>& vhlines,
                            cv::Vec2i extremities[][2],
                            cv::Vec4i quadborders[4],
                            long borderdistances[4],
                            long extrdistance[],
                            short bordercodes[]);

/**
 * @brief Find shortest quadrilateral
 *
 * Find the shortest quadrilateral in wich containing the house
 *
 * @param vhlines           [in]        The set of lines having vertical and
 *                                      horizontal properties
 * @param extremities       [in]        The set of intersections near the image
 *                                      borders
 * @param quadborders       [in, out]   The four lines that are the closest to
 *                                      the image border
 * @param extrdistance      [in]        The distances of the lines to the image
 *                                      borders
 * @param borderdistances   [in, out]   The distances of the quadrilateral
 *                                      borders to the image borders
 * @param w                 [in]        The width of the image
 * @param h                 [in]        The height of the image
 * @param bordercodes       [in]        The set of code of the lines related to
 *                                      the nearest image border (designed by
 *                                      TOP, RIGHT, BOT and LEFT)
 */
void findShorterQuadrilateral(std::vector<cv::Vec4i>& vhlines,
                              cv::Vec2i extremities[][2],
                              cv::Vec4i quadborders[4],
                              long extrdistance[],
                              long borderdistances[4],
                              int w,
                              int h,
                              short bordercodes[]);

/**
 * @brief Is in picture
 *
 * Check if a point is located in the picture or not
 *
 * @param p     [in]    The point we want to check if it is in the image
 * @param w     [in]    The width of the image
 * @param h     [in]    The height of the image
 * @return Return true if the point is in the image, false otherwise
 */
bool isInPicture(cv::Vec2i& p, int w, int h);

/**
 * @brief Redefine quadrilateral
 *
 * Redefine the points defining the lines defining the quadrilateral, making
 * the points matching the corners of the quadrilateral
 *
 * @param quadborders   [in, out]   The lines defining the quadrilateral
 * @param w             [in]        The width of the image
 * @param h             [in]        The height of the image
 */
void redefineQuadrilateral(cv::Vec4i quadborders[4], int w, int h);

/**
 * @brief Replace quadrilateral border
 *
 * Replace a given line defining the quadrilateral by the next nearest line
 * closest to the same image border.
 *
 * @param vhlines           [in]        The set of lines having vertical and
 *                                      horizontal properties
 * @param extremities       [in]        The set of intersections near the image
 *                                      borders
 * @param quadborders       [in, out]   The lines defining the quadrilateral
 * @param extrdistance      [in]        The set of distances of the lines to the
 *                                      closest image border
 * @param borderdistances   [in]        The distances of the lines defining the
 *                                      quadrilateral to the closes image border
 * @param code              [in]        The code of the border we want to
 *                                      replace (defined by TOP, RIGHT, BOT or
 *                                      LEFT)
 * @param bordercodes       [in]        The set of codes of the nearest image
 *                                      border for each line
 */
void replaceQuadrilateralBorder(std::vector<cv::Vec4i>& vhlines,
                                cv::Vec2i extremities[][2],
                                cv::Vec4i quadborders[4],
                                long extrdistance[],
                                long borderdistances[4],
                                short code,
                                short bordercodes[]);

/**
 * @brief Find new quadrilateral border
 *
 * Find a new lines replacing and existing line of the quadrilateral with the
 * shortest addition of distance to the corresponding border of the image.
 *
 * @param vhlines       [in]    The set of lines having vertical and horizontal
 *                              properties
 * @param extremities   [in]    The set of intersections near the image borders
 * @param code          [in]    The code of the border we want to replace
 * @param oldDist       [in]    The distance of the previous border to the
 *                              nearest image border
 * @param newdist       [out]   The distance of the new border to the nearest
 *                              image border
 * @param bordercodes   [in]    The set of codes of the nearest image border for
 *                              each line
 * @param extrdistance  [in]    The set of distance of the lines to the closest
 *                              image border
 * @return The new border
 */
cv::Vec4i findNewQuadrilateralBorder(std::vector<cv::Vec4i> &vhlines,
                                     cv::Vec2i extremities[][2],
                                     short code,
                                     long oldDist,
                                     long& newdist,
                                     short bordercodes[],
                                     long extrdistance[]);

/**
 * @brief Find shortest rectangle
 *
 * Find the biggest rectangle fitting inside a given quadrilateral
 *
 * @param quadcorners   [in]    The corners of the quadrilateral
 * @param rectangle     [out]   The corners of the rectangle
 * @param w             [in]    The width of the image
 * @param h             [in]    The height of the image
 */
void findShortestRectangle(cv::Vec2i quadcorners[4],
                           cv::Vec2i rectangle[4],
                           int w,
                           int h);

void verticalCut2Points(cv::Mat *src, cv::Point p1, cv::Point p2);

#endif // PERSPECTIVE_H
