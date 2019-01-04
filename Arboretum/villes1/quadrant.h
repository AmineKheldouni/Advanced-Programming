/*
 * A simple class to operate on quadrants
 */

#pragma once

#include "square.h"

//
// A quadrant at direction dir, corresponding to a sub-square of some
// larger square
//

class Quadrant {
public:
    // Direction of the quadrant, as defined in quadtree.h
    int dir;
    // Corresponding sub-square
    Square ssq;
public:
    // Construct the quadrant of point p in square s, i.e., the
    // quadrant direction in which p can be found in s, as well as the
    // corresponding sub-square of s
    Quadrant(Square s, Point_2D p);
};
