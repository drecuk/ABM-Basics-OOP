//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  This example shows how to integrate OpenGL with Objects
//  encapsulating all internal functions, essentially creating
//  a pattern of an Prey in Prey-based modelling systems
//
//	Autonomy is in the free roaming, and avoidance of boundaries
//	leaving the world
//
//  The Prey class prototype derives from Object
//  Prototype details are decoupled in Prey.h
//  Decoupling codes make it very easy to manage programmatically
//	##########################################################

#include "OGLUtil.h"
#include "Prey.h"

Prey::Prey(int _id, float origX, float origY, float origZ, float speed): Agent(_id, origX, origY, origZ, speed)
{
  // setting standard movement, rotation speed
	fAngle = 0;
	fCurrAngle = 0;
	fAngleSpeed = 0.1f;
	fMaxAngle = 5.0f;		// increased max angle now
	fSpeed = speed;
	fMaxSpeed = 0.1f;
	fMovement = 0.0f;
	fFriction = 0.99f;

	fScale = 1.0f;

	isForward = isBackward = isRight = isLeft = false;

	vPos.x = origX;
	vPos.y = origY;
	vPos.z = origZ;

  // Prey VARIABLES
  _preyID = -1; // started with no prey
	_distanceToTarget = 5.0f;
}

Prey::~Prey()
{
  cout<<"Prey destroyed!"<<endl;
}

// void Prey::setBoundary(float top, float bottom, float left, float right)
// {
// 	_top = top;
// 	_bottom = bottom;
// 	_left = left;
// 	_right = right;
// }

void Prey::render()
{
	// ** glIdentity must be called here so that
	// the matrix transform is reset to identity matrix made to this Prey
	// so that it doesn't affect all other Preys later
	// try removing this and see
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Reset the projection matrix (loading identity)
	matPos.identity();

	// set translation and rotation matrix
	matPos.translate(vPos.x, 0, vPos.z);
	//fCurrAngle += 0.11f;
	matRot.rotateY(fCurrAngle);

	// load position matrix and add rotation
	//glLoadMatrixf(matPos.matrix);
	glMultMatrixf(matPos.matrix);
	glMultMatrixf(matRot.matrix);
		DrawObject(0.0f, 1.0f, 0.0f);

  // set flag to true for moving forward
	//isForward = true;
}

void Prey::update()
{
	autonomy();

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
}

void Prey::autonomy()
{
	// simulating erratic behaviour by randomising decisions

    // generate a random boolean value
  	int r = rand() % 2;

  	if(r == 0)
  	{
  		rotateLeft(2.0f);
  	}
  	else if (r == 1) // just to be explicit
  	{
  		rotateRight(2.0f);
  	}

  	// get another random value for thrust
  	r = rand() % 2;
  	if(r == 0)
  	{
  		moveForward(2.0f);
  	}

  	// check boundary and try not to leave!
  	if (_grid->isInBoundary(vPos, 2.0f) == false)
  	{
  		rotateLeft(5.0f);
  	}

}


void Prey::seek()
{
  // for(int i = 0; i < _noOfAgents; i++)
  // {
  //     Vector3f p = _agents[i]->getPosition();
	// 		//cout<<i<<" "<<p.x<<","<<p.z<<endl;
  //     if(p.distance(getPosition(), p) < _distanceToTarget)
  //     {
  //       cout<<"chasing "<<i<<endl;
  //     }
  // }
}

void Prey::chase()
{
	//cout<<"prey chase: "<<_preyID<<endl;
}

void Prey::DrawObject(float red, float green, float blue)
{
	glColor3f(red, green, blue);		// set colour to red
	//glVertex3f(0.0f, 0.0f, -0.5f);
	//glVertex3f(-0.5f, 0.0f, 0.5f);
	//glVertex3f(0.5f, 0.0f, 0.5f);

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

	// lines
	// glLineWidth(0.5f);
	// glBegin(GL_LINES);
	// 	// vertical
	// 	glVertex3f(-1.0f*fScale, 0.0f, 0.0f);
	// 	glVertex3f(2.0f*fScale, 0.0f, 0.0f);
	// glEnd();
	//
	// glBegin(GL_LINES);
	// 	// horizontal
	// 	glVertex3f(0.0f, 0.0f, -2.0f*fScale);
	// 	glVertex3f(0.0f, 0.0f, 2.0f*fScale);
	// glEnd();

	// lines
	// glLineWidth(0.5f);
	// glBegin(GL_LINES);
	// 	// vertical
	// 	glVertex3f(-1.0f*fScale, 0.0f, 0.0f);
	// 	glVertex3f(2.0f*fScale, 0.0f, 0.0f);
	// glEnd();
	//
	// glBegin(GL_LINES);
	// 	// horizontal
	// 	glVertex3f(0.0f, 0.0f, -2.0f*fScale);
	// 	glVertex3f(0.0f, 0.0f, 2.0f*fScale);
	// glEnd();
}
