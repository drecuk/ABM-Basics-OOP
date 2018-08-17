//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class
//
//  This example shows how to integrate OpenGL with Objects
//  encapsulating all internal functions, essentially creating
//  a pattern of agents in Agent-based modelling systems
//
//  The Moveable class prototype derives from Object
//  Implementation details are decoupled in Moveable.cpp
//  Decoupling codes make it very easy to manage programmatically
//	##########################################################
#ifndef MOVEABLE_H
#define MOVEABLE_H

#include "OGLUtil.h"
#include "Object.h"

/****************************** PROTOTYPES ******************************/
class Moveable: public Object
{
private:
  // movement variables
  float fAngle, fCurrAngle, fAngleSpeed, fMaxAngle, fScale;
	float fSpeed, fMaxSpeed;
	float fMovement, fFriction;

  // Matrix and Vector transforms
	Matrix4x4 matPos;  // position Matrix
	Matrix4x4 matRot;  // rotation matrix
	Vector3f 	vPos;    // position of the object

  // movement flags
	bool isForward, isBackward, isRight, isLeft, isMoving;

public:
  // constructors
  Moveable();
  Moveable(int _id, float origX, float origY, float origZ, float speed);
  ~Moveable();

  // update functions
  void render();
  void update();

  // movement functions
  Vector3f getPosition();
  void rotateLeft(float fAngleSpeed);
  void rotateRight(float fAngleSpeed);
  void moveForward(float speed);
  void moveBackward(float speed);
  void stopForward();
  void stopBackward();
  void notMoving();

  // visual representation function
  void DrawObject(float red, float green, float blue);
};

#endif
