#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <set>
#include <algorithm>
#include "point2d.h"
#include "town.h"
using namespace std;



#ifndef srcPath
#define srcPath "."
#endif
// The path and name of the town file
string townFile = srcPath "/villes2.txt";

/*
 * Study French isotoponyms
 */
void espace(){
    cout << endl;
    cout << "##################################" << endl;
    cout << endl;
}

int main()
{
    /////
    ///// Read town file
    /////
    // Create an empty list of towns
    vector<Town> towns;
    // Read file
    cout << "Reading town file: " << townFile << endl;
    clock_t time1 = clock();
    int nb_towns = Town::read_file(townFile, towns);
    clock_t time2 = clock();
    // Sanity check
    if (nb_towns < 0) {
	cout << "Error while reading file" << endl;
	return 2;
    }
    if (nb_towns == 0) {
	cout << "No town found" << endl;
	return 1;
    }
    // Initialize random seed
    srand(time(NULL));
    // Say how many towns have been read
    cout << "File read in: "
	 << ((float)(time2-time1)/CLOCKS_PER_SEC) << " s" << endl;
    cout << "Number of towns in file: "
	 << towns.size() /* same as nb_towns */ << endl;
    cout << "A random town, using format \"name[lat,lon](x,y)\": "
	 << towns[rand() % towns.size()] << endl;

    // Question 1 : Creating an associative tab
    map<string, int> names_occurrence;
    map<int,int> hist_names;
    map<string, int>::iterator it2;
    for(auto it1=towns.begin(); it1!=towns.end(); ++it1){
        // We find the iterator in our map if it is already in it
            it2 = names_occurrence.find(it1->name());
            if (it2 == names_occurrence.end())
                // If it's not in the map, we insert it with an occurrence of 1
                names_occurrence.insert(pair<string, int>(it1->name(), 1));
            else
                // If it's in the map, we add 1 to its occurrence; which means we already met a town with
                // the same name.
                names_occurrence[(*it2).first] += 1;
    }

    // Creating names' histogram
    map<int, int>::iterator itt;
    for(auto it=names_occurrence.begin(); it!=names_occurrence.end(); it++){
        // We take the occurrence and check if it's greater than 1
        itt = hist_names.find((*it).second);
        if (itt == hist_names.end())
            hist_names[(*it).second] = 1;

        else
            hist_names[(*it).second] += 1;
    }

    // Displaying the histogram
    for(auto it3=hist_names.begin(); it3!=hist_names.end(); it3++)
        cout << " Number of towns with a name occurrence of " << it3->first << " is " << it3->second << endl;

    espace();

    // Question 2 : Number of towns' same coords histogram
    map<Point2D, int> coords_occurrence;
    map<Point2D, int >::iterator it;

    for(auto it1=towns.begin(); it1!=towns.end(); ++it1){
        it = coords_occurrence.find(Point2D(it1->x(),it1->y()));
        if (it == coords_occurrence.end()){
            coords_occurrence.insert(pair<Point2D, int>(Point2D(it1->x(),it1->y()), 1));
        }
        else{
            coords_occurrence[(*it).first] += 1;
        }
    }

    // Creating coords' histogram
    map<int,int> hist_coords;
    for(auto it=names_occurrence.begin(); it!=names_occurrence.end(); it++){
        itt = hist_coords.find((*it).second);
        if (itt == hist_coords.end()){
            hist_coords[(*it).second] = 1;
        }
        else
            hist_coords[(*it).second] += 1;
    }

    for(auto it3=hist_coords.begin(); it3!=hist_coords.end(); it3++)
        cout << " Number of towns with a coords' occurrence of " << it3->first << " is " << it3->second << endl;

    espace();

    // Question 3 : Creating sets N and C :

    time1 = clock();
    // Creating N :
    set<Town> N;
    map<string, int>::iterator it_N;
    for(auto it1=towns.begin(); it1!=towns.end(); it1++) {
        it_N = names_occurrence.find((*it1).name());
        if (names_occurrence[it_N->first]>1)
            N.insert(*it1);
    }
    cout << "Number of Towns that have same name as other Towns : "<< N.size() << endl;

    espace();

    // Creating C :
    set<Town> C;
    map<Point2D, int>::iterator it_C;
    for(auto it1=towns.begin(); it1!=towns.end(); it1++) {
        it_C = coords_occurrence.find(Point2D(it1->x(),it1->y()));
        if (coords_occurrence[it_C->first]>1)
            C.insert(*it1);
    }
    cout << "Number of Towns that have same coordinates as other Towns : " << C.size() << endl;

    espace();

    // Creating N intersected with C set :
    vector<Town> N_C(min(N.size(), C.size()));
    vector<Town>::iterator end;
    end = set_intersection(N.begin(), N.end(), C.begin(), C.end(), N_C.begin());
    while (end!=N_C.end()){
        N_C.pop_back();
    }

    cout << "Number of Towns that have same name as another Town and same coordinates as another one : "
         << N_C.size() << endl;

    espace();

    //Question 4 : Tuples of Towns that we may be wrong about (having a town of same name and
    // another one of same coords, according the proposition of v2, v3 and v4's existence)
    int isotoponyms = 0;
    for(auto it1=N_C.begin();it1!=N_C.end();it1++){
        for(auto it2=N_C.begin();it2!=N_C.end();it2++){
            // If town 1 and town 2 have same coords
            if (it2!=it1 && Point2D(it1->x(), it1->y())==Point2D(it2->x(), it2->y())){
                for(auto it3=N_C.begin(); it3!=N_C.end(); it3++){
                    for(auto it4=N_C.begin(); it4!=N_C.end();it4++){
                        // If town 4 is different from town 3 and 2
                        if (it4 !=it3 && it4 != it2){
                            // If town 1 has same name as town 3, town 3 has same coords as town 4 and town 4 has same
                            // name as town 2 :
                            if (it1->name() == it3->name() && Point2D(it3->x(),it3->y())==Point2D(it4->x(),it4->y()) &&
                                    it2->name()==it4->name())
                                isotoponyms ++; // We add 1 to the number of our isotoponyms
                        }
                    }
                }
            }
        }
    }
    time2 = clock();
    cout << "Number of town we can be wrong about we are comparing names and coords is " << isotoponyms << endl;
    cout << "Found tuples of Towns in N intersected with C in : " << (float)(time2-time1)/CLOCKS_PER_SEC << " "
            "seconds." << endl;
    espace();
    cout << "End of program !" << endl;

    return 0;
}
