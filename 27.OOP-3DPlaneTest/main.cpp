//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Application
//	OOP + OpenGL integration
//
//  Putting agents on a three-dimensional plane
//  constructing a plane and ray-casting a line to get the
//  height and normals of the 3D plane
//  No boundary checks, cube does not fall off over the edge of the terrain
//
//  Normals: obtained from the cross product of two vectors
//  Height: obtained from the dot product of the normals of the plane
//          with its position
//
//  ----------------------------------------------------------
//  How to compile:
//  note that we are now using both SDL2 and OpenGL, thus the -l for all libraries
//  we are also using multiple cpp files
//  sudo g++ -I/usr/include/ main.cpp Camera.cpp Grid.cpp -o main -L/usr/lib -lSDL2 -lGL -lGLU
//
// -I define the path to the includes folder
// -L define the path to the library folder
// -l ask the compiler to use the library
//	##########################################################

#include <iostream>
#include <string>
#include "OGLUtil.h"
#include "Grid.h"
#include "Camera.h"
using namespace std;

/****************************** PROTOTYPES ******************************/
void checkKeyPress();
void initOpenGL();
int setViewport( int width, int height );
void renderScene();
void DrawSquare(float xPos, float yPos, float zPos, float red, float green, float blue);

void createPlane(float scale, float height);
Vector3f calculatePlaneNormals();
float distanceToPlane(Vector3f pos);
void placeBoxOnPlane();
void drawCube(Vector3f pos, float red, float green, float blue);

void setupAmbientLight();


// ----------------------- Light Variables
GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat diffuseLight[] = {1.0f,1.0f, 1.0f, 1.0f,};

GLfloat matAmbient[] = {1.0f,1.0f, 1.0f, 1.0f,};
GLfloat matDiffuse[] = {1.0f,1.0f, 1.0f, 1.0f,};
GLfloat matSpecular[] = {1.0f,1.0f, 1.0f, 1.0f,};

/****************************** GLOBAL VARIABLES ******************************/
SDL_Event event;                // declare an SDL event
bool isRunning;                 // main loop state

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;
SDL_RendererInfo displayRendererInfo;

Camera *camera;     // CAMERA
Grid *grid;			    // declare grid

// background colour starts with black
float r, g, b = 0.0f;

// ----------------------- plane VARIABLES
float pY0, pY1, pY2, pY3;

Matrix4x4 mPoint;            // matrices
Matrix4x4 mROTPoint;

Matrix4x4 mCube;
Vector3f vCubePos;

// ----------------------- Plane
Vector3f planeNormals;
Vector3f planeVertices[2][2];

/****************************** MAIN METHOD ******************************/
int main(int argc, char**argv)
{
    cout<<"*********************** Initialising Scene Utility ***********************"<<endl;
    //  instantiate grid
    float gridWidth = 50.0f;
    float gridLength = 50.0f;
    float gridSpacing = 5.0f;
    grid = new Grid(gridWidth, gridLength, gridSpacing);

    // instantiating the camera
    camera = new Camera(Vector3f(0.0f, 30.0f, 60.0f), Vector3f(0.0f, 0.0f, -1.0f), 0.2f, 3.0f, 20.0f);

    cout<<"*********************** Create a Plane ***********************"<<endl;
    //createPlane(float scale, float height)
    createPlane(30.0f, 10.0f);

    cout<<"*********************** Initialising Agents ***********************"<<endl;


    cout<<"*********************** Begin SDL OpenGL ***********************"<<endl;

    cout<<"-------- Using OpenGL 3.0 core "<<endl;
    // Use OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // set states
    isRunning = true;

    // initialise SDL Video out
    cout<<"-------- Initialise SDL"<<endl;
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        cout<<"Unable to initialise SDL: "<<SDL_GetError()<<endl;
        exit(1);
    }

    SDL_CreateWindowAndRenderer(800, 800, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);

    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
    // Check that we have OpenGL
    if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
        (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
        /*TODO: Handle this. We have no render surface and not accelerated. */
    }

    // Sets title bar
    SDL_SetWindowTitle(displayWindow, "OpenGL OOP Integration");

    // init the OpenGl window
    initOpenGL();

    // setup viewport
    setViewport(800, 800);


    // --------------------- SIMULATION BLOCK
    cout<<"------- SIMULATION BLOCK STARTED"<<endl;
    // Simulation main loop is defined here
    // note that without using GLUT, we are now able to control
    // everything which runs within the loop using our own implementation

    int frameRate = 1000 / 60;
    Uint32 timeStart = SDL_GetTicks();
    float px = 0.0f;
    while (isRunning) {
        checkKeyPress();

        // The while loop runs too quickly on most systems which can hang
        // most machines. The code below keeps the update consistent
        // increase the frameRate above to slow down the simulation
        if (SDL_GetTicks() >= timeStart + frameRate)
        {
          timeStart = SDL_GetTicks();
          //cout<<"timestart:"<<timeStart<<endl;
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen | depth buffer
          glLoadIdentity();

          // ------------------ START ALL UPDATES AND RENDERING HERE
          camera->update();
            //void gluLookAt(	GLdouble eyeX, GLdouble eyeY,	GLdouble eyeZ, GLdouble centerX,GLdouble centerY,	GLdouble centerZ,	GLdouble upX,	GLdouble upY,	GLdouble upZ);
            gluLookAt(camera->x, camera->y, camera->z, camera->tx, camera->ty, camera->tz, 0.0f, 1.0f, 0.0f);

          // we need to draw the components of the world in relation
          // to the grid's matrix stack, therefore the push and pop here to
          // couple all of them together
          glPushMatrix();
            grid->render();

            // adjust the Y axis (height) of the plane
            // based on keyboard input ijkl
          	planeVertices[0][0].y = pY0;
          	planeVertices[0][1].y = pY1;
          	planeVertices[1][0].y = pY2;
          	planeVertices[1][1].y = pY3;

          	glColor3f(0.0f, 1.0f, 0.0f);
          	glEnable(GL_TEXTURE_2D);
          	glEnable(GL_LIGHTING);

          	// ------------------->> draw the plane
          	glEnable(GL_TEXTURE_2D);
          	glEnable(GL_LIGHTING);

            // drawing the polygons
            glColor3f(0.0f, 1.0f, 0.0f);
            glLineWidth(1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
          	glBegin(GL_TRIANGLE_STRIP);				// draw the plane with triangles
          		for(int x=0; x<2; x++)
          			for(int z=0; z<2; z++)
          				glVertex3f(planeVertices[x][z].x, planeVertices[x][z].y, planeVertices[x][z].z);
          	glEnd();


            // drawing the meshline
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(0.5f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_TRIANGLE_STRIP);				// draw the plane with triangles
          		for(int x=0; x<2; x++)
          			for(int z=0; z<2; z++)
          				glVertex3f(planeVertices[x][z].x, planeVertices[x][z].y, planeVertices[x][z].z);
          	glEnd();

            // ------------------->> draw the cube
          	placeBoxOnPlane();
          	drawCube(vCubePos, 1,0,0);
          glPopMatrix();

          //bool isAbove = Vector3f::isAboveLine(planeVertices[0][1], planeVertices[1][0], vCubePos);
          //cout<<"isAbove: "<<isAbove<<endl;

          // Update window with OpenGL rendering
          SDL_GL_SwapWindow(displayWindow);

          // ------------------ END ALL UPDATES AND RENDERING HERE
        }
    }

    cout<<"------- SIMULATION BLOCK ENDED"<<endl;

    // clear the screen to default
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    cout<<"------- Cleaning Up Memory"<<endl;


    cout<<"---- deleting grid"<<endl;
    delete grid;

    cout<<"---- deleting camera"<<endl;
    delete camera;

    // Destroy window
    SDL_DestroyWindow(displayWindow);

    // delete displayRenderer;
    // delete displayWindow;

    displayRenderer = NULL;
    displayWindow = NULL;
    cout<<"------- Objects in Memory ALL Destroyed"<<endl;

    SDL_Quit();

   return 0;
}

void createPlane(float scale, float height)
{
	// create x z vertices
	cout<<"---------------------->> Plane Vertices: "<<endl;
	for(int x=0; x<2; x++)
	{
		for(int z=0; z<2; z++)
		{
			planeVertices[x][z] = Vector3f(x*scale, 0, z*scale);
			cout<<"x:"<<planeVertices[x][z].x<<" z:"<<planeVertices[x][z].z<<endl;
		}
	}

	pY0 = 1;
	pY1 = 1;
	pY2 = 1;
	pY3 = 1;

	// assign height to the vertices
	cout<<"---------------------->> Plane Height: "<<endl;
	planeVertices[0][0].y = pY0*height;
	planeVertices[0][1].y = pY1*height;
	planeVertices[1][0].y = pY2*height;
	planeVertices[1][1].y = pY3*height;
	for(int x=0; x<2; x++)
		for(int z=0; z<2; z++)
			cout<<"x:"<<planeVertices[x][z].x<<" y: "<<planeVertices[x][z].y<<" z:"<<planeVertices[x][z].z<<endl;

	// calculate plane normals
	calculatePlaneNormals();

	// calculate size of array
	float arSize = sizeof(planeVertices)/sizeof(Vector3f);
	cout<<"size of planeVertices:"<<arSize<<endl;

	vCubePos.x = 0.0f;
	vCubePos.z = 0.0f;
}

Vector3f calculatePlaneNormals()
{
	Vector3f vecNorm;

	// ------------------------------------------------ calculate normals
	// get points
	Vector3f p0 = planeVertices[0][0];
	Vector3f p1 =	planeVertices[0][1];
	Vector3f p2 =	planeVertices[1][0];

	// get vectors between points
	Vector3f vA = p1-p0;
	Vector3f vB = p2-p0;

	// cross product of vectors
	Vector3f vN = vA.crossProduct(vB);
	vN.normalise();
	planeNormals = vN;
	//cout<<"---------------------->> Plane Normal Vector: ";
	//vN.print();

	return vecNorm;
}

float distanceToPlane(Vector3f pos)
{
	//cout<<"---------------------->> Distance of position to Plane: "<<endl;
	float dist = 0.0f;

	// recalculate normals
	calculatePlaneNormals();

	// dot product of plane normal and plane position
	float D = planeNormals.dotProduct(planeVertices[0][0]);
	//cout<<"D of plane:"<<D<<endl;

	//cout<<"Position:";	pos.print();

	// test the pos against the plane normals
	dist = planeNormals.dotProduct(pos) - D;
	//cout<<"The cube in relation to the plane is:"<<dist<<endl;

	return  dist;
}

void placeBoxOnPlane()
{
	float theY;	// the y position of the box
	float boxdist = distanceToPlane(vCubePos);	// calculate the distance of the box to the plane

	if (boxdist <= 0.0)	// the box is under the plane
		theY = vCubePos.y + fabs(boxdist);
	else
		theY = vCubePos.y - fabs(boxdist);

	// assign the new position to the box.
	vCubePos.y = theY;

	cout<<"Placed box on plane.y:"<<vCubePos.y<<" boxdist:"<<boxdist<<endl;
}

void drawCube(Vector3f pos, float red, float green, float blue)
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	mCube.identity();
	mCube.translate(pos.x, pos.y, pos.z);
	//glLoadMatrixf(mCube.matrix);
	glMultMatrixf(mCube.matrix);
	//mCube.print();

	glColor3f(red, green, blue);

		glBegin(GL_QUADS);
		// Front Face
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		glNormal3f( 0.0f, 0.0f,-1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		// Top Face
		glNormal3f( 0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		// Bottom Face
		glNormal3f( 0.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		// Right Face
		glNormal3f( 1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		// Left Face
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();
}

void setupAmbientLight()
{

	glEnable(GL_LIGHTING);			// enable lighting

	// setup ambient light
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

	// set the diffuse light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

	// enable light0
	glEnable(GL_LIGHT0);

	// enable material
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// set the material
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
}

void checkKeyPress()
{
  // SDL events polling here
  while(SDL_PollEvent(&event) != 0)
  {
      // Close the window X
      if(event.type == SDL_QUIT) isRunning = false;

      // ** we have removed all controls for the agent now
      if ( event.type == SDL_KEYDOWN )
      {
        // if escape key is pressed
        if ( event.key.keysym.sym == SDLK_ESCAPE ) isRunning = false;

        // --------------------------- KEYDOWN HANDLER
        if ( event.key.keysym.sym == SDLK_RIGHT )
        {
          // cout<<"camera rotate right"<<endl;
          camera->rotateRight();
        }

        if ( event.key.keysym.sym == SDLK_LEFT )
        {
          // cout<<"camera rotate left"<<endl;
          camera->rotateLeft();
        }

        if ( event.key.keysym.sym == SDLK_UP )
        {
          // cout<<"camera forward"<<endl;
          camera->moveForward();
        }

        if ( event.key.keysym.sym == SDLK_DOWN )
        {
          // cout<<"camera backward"<<endl;
          camera->moveBackward();
        }
        if ( event.key.keysym.sym == SDLK_a )
        {
          // cout<<"camera ascend"<<endl;
          camera->ascend();
        }
         if ( event.key.keysym.sym == SDLK_z )
        {
          // cout<<"camera descend"<<endl;
          camera->descend();
        }
         if ( event.key.keysym.sym == SDLK_s )
        {
          // cout<<"camera pitchup"<<endl;
          camera->pitchDown();
        }
         if ( event.key.keysym.sym == SDLK_x )
        {
          // cout<<"camera pitch down"<<endl;
          camera->pitchUp();
        }
      }

      // MOVE THE PLANE
      if ( event.key.keysym.sym == SDLK_g )
      {
        pY2 += 0.2f;
        pY3 += 0.2f;
      }
      if ( event.key.keysym.sym == SDLK_b )
      {
        pY2 -= 0.2f;
        pY3 -= 0.2f;
      }

      // MOVE THE BOX
  	  if ( event.key.keysym.sym == SDLK_i )
      {
        vCubePos.z -= 0.1f;
      }
  	  if ( event.key.keysym.sym == SDLK_k )
      {
    		vCubePos.z += 0.1f;
      }
  	  if ( event.key.keysym.sym == SDLK_j )
      {
        vCubePos.x -= 0.1f;
      }
  	  if ( event.key.keysym.sym == SDLK_l )
      {
  		    vCubePos.x += 0.1f;
      }

      // --------------------------- KEYUP HANDLER
      if (event.type == SDL_KEYUP)
      {
        // if ( event.key.keysym.sym == SDLK_UP ) creature->stopForward();
        // if ( event.key.keysym.sym == SDLK_DOWN ) creature->stopBackward();
      }

  }
}

// A general OpenGL initialization function that sets all initial parameters
void initOpenGL()
{
  cout<<"-------- Initialise OpenGL"<<endl;
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);   // This Will Clear The Background Color To Black
  glClearDepth(1.0);                      // Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LEQUAL);                   // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);                // Enables Depth Testing
  glShadeModel(GL_SMOOTH);                // Enables Smooth Color Shading
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST ); // Nicest perspective calculations

  setupAmbientLight();
}

// Reset our viewport after a window resize
int setViewport( int width, int height )
{
    cout<<"-------- Setting OpenGL Viewport"<<endl;
    GLfloat ratio;

    // prevent divide by zero
    if ( height == 0 ) { height = 1; }

    // calculate ratio
    ratio = ( GLfloat )width / ( GLfloat )height;

    // Setup viewport
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    // specify which matrix is the current matrix
    // GL_PROJECTION means that we are applying all matrix to the PROJECTION MATRIX
    glMatrixMode(GL_PROJECTION);

    // Reset The Projection Matrix, generally done before we do anything to the matrix
    glLoadIdentity();

    // Calculate The Aspect Ratio Of The Window
    // gluPerspective(	GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
    //gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 5000.0f);    // Calculate The Aspect Ratio Of The Window

    // use glu function to set a camera looking at
    // void gluLookAt(	GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ,
   	//                  GLdouble centerX, GLdouble centerY, GLdouble centerZ,
   	//                  GLdouble upX, GLdouble upY, GLdouble upZ);
    // the units below essentially looks down on a map view
    // putting the eye 50 units above the origin
    // looking at target at -10 units below the origin (looking down)
    // up vector is Y

    // gluLookAt(0.0f, 80.0f, 0.0f, 0.0f, -20.0f, -0.01f, 0.0f, 1.0f, 0.0f);
    gluLookAt(camera->x, camera->y, camera->z, camera->tx, camera->ty, camera->tz, 0.0f, 1.0f, 0.0f);

    // now switch to the MODELVIEW MATRIX so that we can control (transform)
    // everything we draw (rectangles, etc.)
    glMatrixMode(GL_MODELVIEW);

    /* Reset The View */
    // glLoadIdentity();

    return 1;
}

void renderScene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen | depth buffer
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


  /* ------------ START DRAW ALL OBJECTS HERE */

	// DrawSquare(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f); // white

  /* ------------ END DRAW ALL OBJECTS HERE */

}
