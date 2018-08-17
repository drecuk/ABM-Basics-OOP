//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Application
//	Inheritance and Polymorphism with virtual functions
//  ----------------------------------------------------------
//  How to compile:
//  sudo g++ <filename>.cpp -o <output file>
//	##########################################################

#include <iostream>
#include <string>
#include "Object.h"
#include "Vegetation.h"
#include "Animal.h"
using namespace std;

int main()
{
  /********************** SINGLE OBJECT INSTANTIATION **********************/
    // instantiate the objects
    Animal animal(0);
    Vegetation vegetation(1);

    // Assign living1 & 2 with addresses of animal and vegetation
    // so that derived class member "Move()" can be accessed
    Object * living1 = &animal;
    Object * living2 = &vegetation;

    // here, because of the virtual keyword in Object
    // the derived class is called even though the
    // instantiation is object. This polymorphism becomes useful
    // when we have many different species and types within a single
    // array
    living1->move();
    living2->move();

    /********************** ARRAY OBJECT INSTANTIATION **********************/
    cout<<"---------- ARRAY IMPLEMENTATION"<<endl;
    // typedef a pointer of type life - reduce code complexity
    // otherwise, double pointers are required to create an array
    typedef Life *Lifeptr;

    // declare an array of Lifeptr
    Lifeptr * life = new Lifeptr[4];

    // call constructor of different types
    cout<<"------ creating new species"<<endl;
    life[0] = new Animal(0);
    life[1] = new Vegetation(1);
    life[2] = new Animal(2);
    life[3] = new Vegetation(3);

    // call derived functions within array
    // very easy to manage multiple species now!
    cout<<"------ calling derived functions"<<endl;
    for(int i=0; i<4; i++)
      life[i]->move();

    /********************** CLEAR MEMORY **********************/

    // clear memory
    cout<<"------ destroying objects"<<endl;
    for(int i=0; i<4; i++)
      delete life[i];

    // delete array from heap memory
    delete life;

    return 0;
}
