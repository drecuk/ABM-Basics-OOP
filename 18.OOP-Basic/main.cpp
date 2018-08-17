//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Application
//	Basic class specification, instantiation,
//  parameter passing and function calling
//  ----------------------------------------------------------
//  How to compile:
//  sudo g++ <filename>.cpp -o <output file>
//	##########################################################

#include <iostream>
#include <string>
#include "Object.h"
using namespace std;

int main()
{
  /****************************** OBJECT INSTANTIATION ******************************/
    // instantiate an object
    Object myobj;

    // calls the object's function
    myobj.Talk();

    // using pointers (objects are stored in heap memory)
    Object *myObj2 = new Object();
    myObj2->Talk();

    // using parameters
    Object *myObj3 = new Object("I am an object!");
    myObj3->Talk();

    // deletes from heap memory
    delete myObj2;
    delete myObj3;

    return 0;
}
