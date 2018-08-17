//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class
//  The Vegetation class derives from Life
//	##########################################################
#ifndef VEGETATION_H
#define VEGETATION_H

#include <iostream>
#include <string>
#include "Life.h"
using namespace std;

class Vegetation: public Life
{
protected:
    int id;
public:
    Vegetation(int id): Life(id)
    {
      cout << "Vegetation created: " << getID() <<endl;
    }

    // Object.move() implementation in Vegetation
    void move()
    {
      cout << "Vegetation can't move!" <<endl;
    }

    // destructor
    ~Vegetation(){ cout<<"Vegetation destroyed: "<<id<<endl; }
};
#endif
