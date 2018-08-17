//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class
//
//  The MoveableOnTerrain is an updated version from the Moveable class
//	Allowing the avatar to skate on the surface of a simple terrain
//
//  The MoveableOnTerrain class prototype derives from Object
//  Implementation details are decoupled in MoveableOnTerrain.cpp
//  Decoupling codes make it very easy to manage programmatically
//	##########################################################
#ifndef MoveableOnTerrainONTERRAIN_H
#define MoveableOnTerrainONTERRAIN_H

#include "OGLUtil.h"
#include "Object.h"
#include "SimpleTerrain.h"

/****************************** PROTOTYPES ******************************/
class MoveableOnTerrain: public Object
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

  SimpleTerrain *_terrain;

public:
  // constructors
  MoveableOnTerrain();
  MoveableOnTerrain(int _id, float origX, float origY, float origZ, float speed);
  ~MoveableOnTerrain();

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

  void getTerrain(SimpleTerrain *terrain);

  // visual representation function
  void DrawObject(float red, float green, float blue);
};

#endif
