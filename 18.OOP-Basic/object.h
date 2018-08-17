//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class
//	##########################################################

#include <iostream>
#include <string>
using namespace std;

class Object
{
private:
  // all private variables and functions here
  string words;

public:
  // all constructors, public variables and functions here
  Object()
  {
    cout<<"Object created"<<endl;
  }

  // constructor with parameter
  Object( string s ): words(s) // setter
  {
    cout<<"Object created with parameter " << s <<endl;
  }

  void Talk()
  {
      cout << "Object talks " << words << endl;
  }

  // destructor
  ~Object(){ cout<<"Animal destroyed"<<endl; }
};
