#pragma once

#include <vector>
using std::vector;
#include <cmath>
#include <assert.h>
#include "quadrant.h"

//
// Add point p in the quadtree representing the points in square s
//
template <typename T>

void insert(QuadTree< Point2D<T> >* &qtree, Square s, Point2D<T> p) {
    // If the qtree is empty, we put the point p we want to insert in it
    if (qtree == nullptr) {
        qtree = new QuadLeaf<Point2D<T> >(p);
    }
    // If the qtree is a node, we need to solve the problem by recursion, given the quadrant where p belongs
    if (qtree->isNode()){
        Quadrant Qp = Quadrant(s, p);
        insert(qtree->son(Qp.dir), Qp.ssq, p);
    }
    // If the qtree is a leaf that has a point different than the one we want to insert
    if (qtree->isLeaf()) {
        if (qtree->value() != p) {
            // We split the leaf into quadrants trying to separate the points
            Quadrant Qp = Quadrant(s, p);
            Quadrant Qp2 = Quadrant(s, qtree->value());
            // Our quadtree becomes a node where we already have at least one point different from the one we insert
            QuadLeaf<Point2D<T> > *new_leaf[4];
            for (int i = 0; i < 4; i++) {
                if (i == Qp2.dir)
                    new_leaf[i] = new QuadLeaf<Point2D<T> >(qtree->value());
                else
                    new_leaf[i] = nullptr;
            }
            qtree = new QuadNode<Point2D<T> >(new_leaf[0], new_leaf[1], new_leaf[2], new_leaf[3]);
            // By Recursion, we see if we can insert the point p in the quadrant where he belongs
            insert(qtree->son(Qp.dir), Qp.ssq, p);
        }
    }
}
//
// Add in vector neighbors the points q in quadtree qtree representing
// points in square s at euclidean distance less than r from point p
//
// Set only the nearest point if nearest is true
//
// Return the number of nodes visited
//
template <typename T>

int search(vector< Point2D<T> > &neighbors, QuadTree< Point2D<T> >* qtree, Square s,
           Point2D<T> p, float& r, bool nearest = false){
    // If the qtree is empty, we have no neighbors
    if (qtree == nullptr)
        return 0;
    // If the qtree is a leaf, we check the distance and add the point if he is "near enough" to p
    if (qtree->isLeaf()){
        Point2D<T> visited = qtree->value();
        if (visited.dist2(p) <= r*r)
            neighbors.push_back(visited);
        return 0;
    }
    else{
        int nb_visited = 0;
        // We subdivise the qtree into 4 sons
        for (int i=0; i<4; i++){
            QuadTree< Point2D<T> >* sub_quadtree = qtree->son(i);

            if (sub_quadtree != nullptr){
                // If the son i of qtree is a leaf we compute his distance to p
                if (sub_quadtree->isLeaf()){
                    Point2D<T> visited = sub_quadtree->value();
                    float distance2 = visited.dist2(p);
                    // If we want the nearest point, we exclude point p (distance equals 0) and we
                    if (nearest){
                        if (distance2<r*r && distance2 > 0.000001){
                            // We take a smaller ray r
                            r=sqrt(distance2);
                            // We clear our list of neighbors so that we get the nearest neighbor possible
                            neighbors.clear();
                            neighbors.push_back(visited);
                        }

                        if (distance2==r*r)
                            neighbors.push_back(visited);

                    }
                        // If we look for all neighbors, we add every neighbor which is near the point p
                    else{
                        if (distance2<=r*r)
                            neighbors.push_back(visited);

                    }
                    nb_visited+=1;
                }
                    // If the son i of qtree isn't a leaf
                else{
                    // We call for the function (by recursion) if our subsquare is near the point p
                    Square ssq_visited =s.subsquare(i);
                    if (ssq_visited.intersects_disk(p, r))
                        nb_visited += search(neighbors, sub_quadtree, ssq_visited, p, r, nearest);

                }
            }
        }
        return nb_visited;
    }
}


//
// Add in vector neighbors the nearest point of p in the quadtree qtree
// representing the points in square s
//
// Return the number of nodes visited
//
template <typename T>

int search(vector< Point2D<T> > &neighbors,
           QuadTree< Point2D<T> >* qtree, Square s,
           Point2D<T> p){
    float r = 1000000000.f;
    // We just compute here the nearest neighbor's algorithm given a high ray r, which will decrease progressively until
    // we get our nearest neighbor
    return search(neighbors, qtree, s, p, r, true);

}
