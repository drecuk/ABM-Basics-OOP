//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented class
//  a non-instantiable object
//	##########################################################
#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <string>
using namespace std;

class Object
{
protected:
  // all objects has an id
  int id;
public:
  // constructor
  Object(int id): id(id) { cout<< "Object created!" << endl; }

  // all inherited members will have the getID() function
  int getID() { return id; }

  // virtual function for implementation in derived classes
  virtual void move () = 0;

  // destructor
  ~Object(){ cout<< "Object destroyed: " <<id<<endl; }
};
#endif
