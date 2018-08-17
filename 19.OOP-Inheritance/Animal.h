//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class
//  The Animal class derives from Life
//	##########################################################
#ifndef ANIMAL_H
#define ANIMAL_H
#include <iostream>
#include <string>
#include "Life.h"
using namespace std;

class Animal: public Life
{
protected:
    int id;
public:
    Animal(int id): Life(id)
    {
      cout << "Animal created: " << getID() <<endl;
    }

    // Object.move() implementation in Animal
    void move()
    {
      cout << "Animal moves!" <<endl;
    }

    // destructor
    ~Animal(){ cout<<"Animal destroyed: "<<id<<endl; }
};
#endif
