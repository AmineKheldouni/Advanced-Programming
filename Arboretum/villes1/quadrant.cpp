#include "quadrant.h"

// Square methods
Square Square::subsquare(int dir){
      if (dir == NW)
          return Square(x, y+w/2, w/2);
      if (dir == NE)
          return Square(x+w/2, y+w/2, w/2);
      if (dir == SE)
          return Square(x+w/2, y, w/2);
      if (dir == SW)
          return Square(x, y, w/2);
  }

bool Square::intersects_disk(Point_2D p, float r){
    Point_2D middle(x+w/2, y+w/2);
    float distance_x = abs(p.x() - middle.x());
    float distance_y = abs(p.y() - middle.y());

    if (distance_x > (w/2. + r))
        return false;
    if (distance_y > (w/2. + r))
        return false;

    if (distance_x <= (w/2.))
        return true;
    if (distance_y <= (w/2.))
        return true;

    float corner_distance = (distance_x - w/2)*(distance_x - w/2) + (distance_y - w/2)*(distance_y - w/2);
    return (corner_distance <= (r*r));
}


// Quadrant methods

Quadrant::Quadrant(Square s, Point_2D p){
    float middle_x = s.x+s.w/2;
    float middle_y = s.y+s.w/2;
    if (p.x() <= middle_x){
        if (p.y()<=middle_y)
            dir=SW;
        else
            dir=NW;
    }
    else{
        if (p.y()<=middle_y)
            dir=SE;
        else
            dir=NE;
    }
    ssq=s.subsquare(dir);
}
