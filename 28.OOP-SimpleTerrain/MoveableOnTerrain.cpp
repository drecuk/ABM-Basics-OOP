//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  The MoveableOnTerrain is an updated version from the Moveable class
//	Allowing the avatar to skate on the surface of a simple terrain
//
//  The MoveableOnTerrain class prototype derives from Object
//  Prototype details are decoupled in MoveableOnTerrain.h
//  Decoupling codes make it very easy to manage programmatically
//	##########################################################

#include "OGLUtil.h"
#include "MoveableOnTerrain.h"

MoveableOnTerrain::MoveableOnTerrain(): Object(-1)
{
  // setting standard movement, rotation speed
	fAngle = 0;
	fCurrAngle = 0;
	fAngleSpeed = 0.1f;
	fMaxAngle = 2.0f;
	fSpeed = 0.001f; //ideal 0.001f
	fMaxSpeed = 0.1f;
	fMovement = 0.0f;
	fFriction = 0.99f;

	fScale = 1.0f;

	isForward = isBackward = isRight = isLeft = false;

	vPos.x = 0.0f;
	vPos.y = 0.0f;
	vPos.z = 0.0f;
}

MoveableOnTerrain::MoveableOnTerrain(int _id, float origX, float origY, float origZ, float speed): Object(_id)
{
  // setting standard movement, rotation speed
	fAngle = 0;
	fCurrAngle = 0;
	fAngleSpeed = 0.1f;
	fMaxAngle = 2.0f;
	fSpeed = speed; //ideal 0.001f
	fMaxSpeed = 0.1f;
	fMovement = 0.0f;
	fFriction = 0.99f;

	fScale = 2.0f;

	isForward = isBackward = isRight = isLeft = false;

	vPos.x = origX;
	vPos.y = origY;
	vPos.z = origZ;
}

MoveableOnTerrain::~MoveableOnTerrain()
{
	_terrain = NULL;
  cout<<"MoveableOnTerrain destroyed!"<<endl;
}

void MoveableOnTerrain::render()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Reset the projection matrix (loading identity)
	matPos.identity();

	// set translation and rotation matrix
	matPos.translate(vPos.x, vPos.y, vPos.z);
	//fCurrAngle += 0.11f;
	matRot.rotateY(fCurrAngle);

	// load position matrix and add rotation
	//glLoadMatrixf(matPos.matrix);
	glMultMatrixf(matPos.matrix);
	glMultMatrixf(matRot.matrix);
		DrawObject(1.0f, 0.0f, 0.7f);

  // set flag to true for moving forward
	//isForward = true;
}

void MoveableOnTerrain::update()
{
	if(isForward)
	{
		fMovement -= fSpeed;
		if (fMovement <= -fMaxSpeed) fMovement = -fMaxSpeed;
		if (fMovement >= -0.0001) { fMovement = 0.0f; isForward = false; }
	}

	if(isBackward)
	{
		fMovement += fSpeed;
		if (fMovement >= fMaxSpeed) fMovement = fMaxSpeed;
		if (fMovement <= 0.0001) { fMovement = 0.0f; isBackward = false; }
	}
	fMovement *= fFriction; // friction has to be outside to reduce the movement to a halt

	if(isRight)
	{
		if (fAngle >= fMaxAngle) fAngle = fMaxAngle;
		fAngle *= fFriction;
		if (fAngle <= 0.001f) fAngle = 0.0f;
	}

	if (isLeft)
	{
		if (fAngle <= -fMaxAngle) fAngle = -fMaxAngle;
		fAngle *= fFriction;
		if (fAngle >= -0.001f) fAngle = 0.0f;
	}

	fCurrAngle += fAngle;

	vPos.x -= fMovement*cos(fCurrAngle * PI/180);
	vPos.z -= fMovement*sin(fCurrAngle * PI/180);
	//cout<<vPos.x<< " "<<vPos.z<<" m:"<<fMovement<<endl;
	//cout<<" isForward:"<<isForward<<" isBackward"<<isBackward<<" isLeft:"<<isLeft<<" isRight:"<<isRight<<endl;
	float dist = _terrain->distanceToPlane(vPos);
	cout<<"distance to plane: "<<dist<<endl;

	if (dist <= 0.0)	// the box is under the plane
		vPos.y = vPos.y + fabs(dist);
	else
		vPos.y = vPos.y - fabs(dist);

}

Vector3f MoveableOnTerrain::getPosition()
{
	return Vector3f(vPos.x, vPos.y, vPos.z);
}

void MoveableOnTerrain::rotateLeft(float fAngleSpeed)
{
	fAngle -= fAngleSpeed;
	isLeft = true;
	isRight = false;
}

void MoveableOnTerrain::rotateRight(float fAngleSpeed)
{
	fAngle += fAngleSpeed;
	isRight = true;
	isLeft = false;
}

void MoveableOnTerrain::moveForward(float speed)
{
  fSpeed = speed;
	isForward = true;
	isBackward = false;
}

void MoveableOnTerrain::moveBackward(float speed)
{
  fSpeed = speed;

	isBackward = true;
	isForward = false;
}

void MoveableOnTerrain::stopForward()
{
	isForward = false;
}

void MoveableOnTerrain::stopBackward()
{
  isBackward = false;
}

void MoveableOnTerrain::notMoving()
{
  isForward = false;
  isBackward = false;
}

void MoveableOnTerrain::getTerrain(SimpleTerrain *terrain)
{
	_terrain = terrain;
}

void MoveableOnTerrain::DrawObject(float red, float green, float blue)
{
	glColor3f(red, green, blue);		// set colour to red

	// head pointing towards the positive x axis at the start
	glLineWidth(3.0f);
	glBegin(GL_TRIANGLES);				// draw the triangle
		// right
		glVertex3f(0.0f, 0.5f*fScale, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.5f*fScale);
		glVertex3f(1.0f*fScale, 0.0f, 0.0f);
		// left
		glVertex3f(0.0f, 0.5f*fScale, 0.0f);
		glVertex3f(1.0f*fScale, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, -0.5f*fScale);
		// back
		glVertex3f(0.0f, 0.5f*fScale, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.5f*fScale);
		glVertex3f(0.0f, 0.0f, -0.5f*fScale);
		// belly
		glVertex3f(0.0f, 0.5f*fScale, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.5f*fScale);
		glVertex3f(0.0f, 0.0f, -0.5f*fScale);
	glEnd();
	glLineWidth(0.2f);
}
