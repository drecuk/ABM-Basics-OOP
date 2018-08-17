//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented class
//  The Life class derives from Object
//	##########################################################
#ifndef LIFE_H
#define LIFE_H

#include <iostream>
#include <string>
using namespace std;

class Life: public Object
{
public:
  Life(int id): Object(id) { cout<< "Life created!" << endl; }

  void grow()
  {
      cout<< "Life grows!" << endl;
  }

  // Object.move() has no implementation here

  // destructor
  ~Life(){ cout<< "Life destroyed: " <<id<<endl; }

};
#endif
