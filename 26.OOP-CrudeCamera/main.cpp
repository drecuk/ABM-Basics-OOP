//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Application
//	OOP + OpenGL integration
//
//  Introducing a simple, and crude :P Camera class for slightly
//  easier visualisation in larger simulation landscapes. A proper
//  camera system can be implemented/introduced much later :D
//
//  Note that glLoadIdentity() is no longer called before
//  the transformation of Predator, Prey and Snacks
//  so that they will not be fixed to the global origin
//  glPushMatrix and glPopMatrix is used for coupling
//  all agents with the grid. The simple camera moves in
//  relation to the grid.
//
//  ----------------------------------------------------------
//  How to compile:
//  note that we are now using both SDL2 and OpenGL, thus the -l for all libraries
//  we are also using multiple cpp files
//  sudo g++ -I/usr/include/ main.cpp Camera.cpp Agent.cpp Predator.cpp Prey.cpp Snack.cpp Grid.cpp -o main -L/usr/lib -lSDL2 -lGL -lGLU
//
// -I define the path to the includes folder
// -L define the path to the library folder
// -l ask the compiler to use the library
//	##########################################################

#include <iostream>
#include <string>
#include "OGLUtil.h"
#include "Grid.h"
#include "Agent.h"
#include "Predator.h"
#include "Prey.h"
#include "Snack.h"
#include "Camera.h"
using namespace std;

/****************************** PROTOTYPES ******************************/
void checkKeyPress();
void initOpenGL();
int setViewport( int width, int height );
void renderScene();
void DrawSquare(float xPos, float yPos, float zPos, float red, float green, float blue);

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
    camera->print();

    cout<<"*********************** Initialising Agents ***********************"<<endl;
    // instantiate n agents and assign them arbitrary speed
    int agentNo = 12;

    // this needs not be instantiated
    // the derived types are assigned to the array later
    Agent **agents = new Agent*[agentNo];
    // for(int i=0; i<agentNo; i++)
    //   agents[i] = new Agent(1, 0, 0, 0, 0.001f);

    // derived classes
    Predator **predators = new Predator*[2];
    Prey **preys = new Prey*[4];
    Snack **snacks = new Snack*[6];

    // 1st Loop: create agent types first
    cout<<"----- Creating Types"<< endl;
    for(int i=0; i<agentNo; i++)
    {
      if(i<2)
      {
        // randomise location of snacks
    		int min = grid->getBottom();
    		int max = grid->getBottom() + grid->getBottom();

    		int newX = (rand()%max)-min;
    		int newZ = (rand()%max)-min;
        //cout<<"creating predator "<<i<<endl;
        predators[i] = new Predator(i, newX, 0, newZ, 0.001f);
      }
      else if(i>=2 && i<6)
      {
        // randomise location of snacks
    		int min = grid->getBottom();
    		int max = grid->getBottom() + grid->getBottom();

    		int newX = (rand()%max)-min;
    		int newZ = (rand()%max)-min;
        //cout<<"creating prey "<<i-2<<endl;
        preys[i-2] = new Prey(i, newX, 0, newZ, 0.001f);
      }
      else if(i>=6)
      {
        // randomise location of snacks
        int min = grid->getBottom();
    		int max = grid->getBottom() + grid->getBottom();

    		int newX = (rand()%max)-min;
    		int newZ = (rand()%max)-min;

        //cout<<"creating snacks "<<i-6<<endl;
        snacks[i-6] = new Snack(i, newX, 0, newZ, 0.0f);
      }
    }

    cout << "----- Deriving Types" << endl;
    for(int i=0; i<agentNo; i++)
    {
      if(i<2)
      {
        cout << "-- deriving predators["<<i<<"] "<<predators[i]<<endl;
        agents[i] = predators[i];   // fishes[i] is the address
        agents[i]->speciesType = PREDATOR;
      }
      else if(i>=2 && i<6)
      {
        cout << "-- deriving preys["<<i<<"] "<<preys[i-2]<<endl;
        agents[i] = preys[i-2];
        agents[i]->speciesType = PREY;
      }
      else if(i>=6)
      {
        cout << "-- deriving snacks["<<i<<"] "<<snacks[i-6]<<endl;
        agents[i] = snacks[i-6];
        agents[i]->speciesType = SNACK;
      }
    }

    cout << "----- Getting grid and agents to be accessible to all agents" << endl;
    for(int i=0; i<agentNo; i++)
    {
      agents[i]->getGrid(grid);
      agents[i]->getAgents(agents, agentNo);
    }

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


          // render the scene
          // renderScene();

          // we need to draw all agents in relation to the grid's
          // matrix stack, therefore the push and pop here to
          // couple all of them together
          glPushMatrix();
            grid->render();

            // run through the agents and call their functions
            for(int i=0; i<agentNo; i++)
            {
              agents[i]->update();
              agents[i]->render();
            }

          glPopMatrix();

          // Update window with OpenGL rendering
          SDL_GL_SwapWindow(displayWindow);

          // ------------------ END ALL UPDATES AND RENDERING HERE
        }
    }

    cout<<"------- SIMULATION BLOCK ENDED"<<endl;

    // clear the screen to default
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    cout<<"------- Cleaning Up Memory"<<endl;

    cout<<"---- deleting agents"<<endl;
    for(int i = 0; i < agentNo; i++)
      delete agents[i]; // calls each ~Agent()

    delete[] agents; // delete the heap memory


    cout<<"---- deleting predators"<<endl;
    for(int i = 0; i < 2; i++)
      predators[i]->~Predator();

    delete[] predators;

    cout<<"---- deleting preys"<<endl;
    for(int i = 0; i < 4; i++)
      preys[i]->~Prey();

    delete[] preys;

    cout<<"---- deleting snacks"<<endl;
    for(int i = 0; i < 6; i++)
      snacks[i]->~Snack();

    delete[] snacks;


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

      // --------------------------- KEYUP HANDLER
      if (event.type == SDL_KEYUP)
      {
        // if ( event.key.keysym.sym == SDLK_UP ) creature->stopForward();
        // if ( event.key.keysym.sym == SDLK_DOWN ) creature->stopBackward();
      }

      camera->print();
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
