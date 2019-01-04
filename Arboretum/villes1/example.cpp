
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
using namespace std;

#include <time.h>

#ifndef srcPath
#define srcPath "."
#endif
// The path and name of the town file
string townFile = srcPath "/villes.txt";

#include "town.h"
#include "neighbors.h"


/*
 * Example of use of "town.h" and "villes.txt":
 * - read file of metropolitan French towns with coordinates
 * - display reading time and number of towns read
 * - ask for town name
 * - lookup for town coordinates with this name and display them
 * - display distance between two random towns
 */

template <typename T>
QuadTree< Point2D<T> >* create_qtree(std::vector< Point2D<T> > vect_points, float x,float y,float w){
    QuadTree< Point2D<T> >* qtree = nullptr;
    for(typename std::vector< Point2D<T> >::const_iterator it= vect_points.begin(); it!=vect_points.end();it++){
        Point2D<T> point_insert = *it;
        insert(qtree, Square(x, y, w), point_insert);
    }
    return qtree;
}

int main()
{
    srand (time(NULL));
    /////
    ///// Read town file
    /////
    // Create an empty list of towns
    vector<Town> towns;
    // Prepare limits
    float xmin = std::numeric_limits<float>::max();
    float ymin = std::numeric_limits<float>::max();
    float xmax = std::numeric_limits<float>::min();
    float ymax = std::numeric_limits<float>::min();
    // Read file
    cout << "Reading town file: " << townFile << endl;
    clock_t t1 = clock();
    int nb_towns = Town::read_file(townFile, towns, xmin, ymin, xmax, ymax);
    clock_t t2 = clock();
    // Say how many towns have been read
    if (nb_towns < 0) {
        cout << "No town found" << endl;
        return 1;
    }
    cout << "File read in " << ((float)(t2-t1)/CLOCKS_PER_SEC) << " s" << endl;
    cout << "Number of towns in file: " << nb_towns << endl;
    cout << "Bounding box: (" << xmin<< "," << ymin << ")-("
         << xmax << "," << ymax << ")" << endl;

    /////
    ///// Operate on quadtree
    /////

    // Ask for a town to look for
    string name= "Ponts";
    //cout << "What town are you interested in ? ";
    // Read town name, including spaces
    //getline(cin, name); // Read whole line including spaces
    // Note that "cin >> name;" would read only the first word, stopping
    // at the first blank (space or tab)
    //
    // Look for towns with this name
    vector<Town> ntowns = Town::towns_with_name(towns, name);
    // Say how many towns were found with this name
    cout << "Number of towns with this name: " << ntowns.size() << endl;
    // Display their coordinates
    for (vector<Town>::iterator it_ntown = ntowns.begin();
         it_ntown != ntowns.end(); ++it_ntown)
        cout << "  (lat=" << it_ntown->lat()
             << ", lon=" << it_ntown->lon() << ")" << endl;

    Town target_town=*(ntowns.begin());
    cout << "Looking for nearest neighbor of : " << target_town.name() << endl;
    cout << "Our town's coords are (" << target_town.x() << ", " << target_town.y() << ")" << endl;


    // Creating towns vector of points
    std::vector< Point2D<string> > points_towns;
    vector<Town>::iterator it = towns.begin();
    for (;it!=towns.end();it++){
        Town ville=*it;
        points_towns.push_back(Point2D<string>(ville.x(),ville.y(),ville.name()));
    }
    // Creating the quadtree and computing the time taken
    clock_t time = clock();

    QuadTree< Point2D<string> >* qtree = create_qtree(points_towns,xmin,ymin,max(xmax-xmin,ymax-ymin));
    float time_buildQT = ((float)clock()-time)/CLOCKS_PER_SEC;
    cout << "Time taken to build the quadtree " << time_buildQT << endl;

    display(qtree);
    // Print the quadtree's parameters
    cout << "QuadTree length : "<< qtree->nbLeaves() + qtree->nbNodes() << endl;
    cout << "QuadTree depth : " << depth(qtree) << endl;

    // Nearest neighbor algorithm
    std::vector< Point2D<string> > neighbors;
    int nb_visited = search(neighbors,qtree,Square(xmin,ymin, max(xmax-xmin,ymax-ymin)),Point2D<string>(target_town.x()
            ,target_town.y(),target_town.name()));
    cout<<"Number of town checked in the QuadTree : " << nb_visited << endl;
    if (neighbors.size() == 1)
        cout<<"The nearest neighbor is : " << endl;
    else
        cout<<"The nearest neighbors are : " << endl;

    for(typename std::vector< Point2D<string> >::const_iterator it_neighbors=neighbors.begin();
        it_neighbors!=neighbors.end(); it_neighbors++) {
        Point2D<string> neighbor = *it_neighbors;
        cout << neighbor.info() << " which coords are (" << neighbor.x() << ", " << neighbor.y() << ")" << endl;
    }

    // Mean Time - QuadTree
    const int towns_for_mean = 1000;

    time = clock();
    for(int i=0; i<towns_for_mean; i++){
        neighbors.clear();
        unsigned long p = rand() % points_towns.size();
        Point2D<string> p_town = Point2D<string>(points_towns[p].x(), points_towns[p].y());
        search(neighbors, qtree, Square(xmin,ymin, max(xmax-xmin,ymax-ymin)), p_town);
        time += clock() - time;
    }

    float t = (float)time/CLOCKS_PER_SEC;
    cout << "For QuadTree nearest neighbor method, we have a mean time of " << t/((float)towns_for_mean) << endl;
    // Mean Time - Vector
    int nearest_index;
    time = clock();
    for(int i=0; i<towns_for_mean; i++){
        nearest_index = 0;
        unsigned long p = rand() % points_towns.size();
        float dist = distance2(points_towns[p].x(), points_towns[p].y(), points_towns[0].x(), points_towns[0].y());
        for(int j=1; j<points_towns.size(); j++){
            float dist2 = distance2(points_towns[p].x(), points_towns[p].y(), points_towns[j].x(), points_towns[j].y());
            if (dist2 <= dist && dist2>0.00001){
                dist = dist2;
                nearest_index = j;
            }
        }
        time += clock()-time;
    }

    t = (float)time/CLOCKS_PER_SEC;
    cout << "For a linear vector method : " << t/((float)towns_for_mean) << endl;

    delete qtree;
    return 0;
}
