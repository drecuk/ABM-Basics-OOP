//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  A simple terrain using 2D arrays to represent a small terrain
//	described by GL_TRIANGLE_STRIP and normals for lighting and
//	the calculation of an agent skirting on the surface of each face
//
//	##########################################################

#include "SimpleTerrain.h"
using namespace std;

SimpleTerrain::SimpleTerrain(int width, int height, float _scaleHeight, float terrainSize)
{
  cout<<"---------------------------------->> Creating Simple 4x4 Terrain"<<endl;

  // width and height of terrain (also vertex grids)
	dWidth = width;
	dHeight = height;

	// scaling factor
	scaleHeight = _scaleHeight;
	terrainScale = terrainSize;

	// adjustment for setting terrain centre at origin
	adjFromOrig = (terrainScale*dWidth)/2;

	// set terrain boundary
	boundary.top = -adjFromOrig;
	boundary.bottom = adjFromOrig;
	boundary.left = -adjFromOrig;
	boundary.right = adjFromOrig;

  // random height threshold of terrain
  int max = 20;
  int min = 0;

  // initialising 2D array with 0 vectors
  for(int x = 0; x<width+1; x++)    // row x axis
	{
		for(int z = 0; z<height+1; z++) // column z axis
		{
      float pointHeight = (rand()%max)-min; // randomise the height
      terrainData[x][z] = Vector3f(	x*terrainScale - adjFromOrig,
									pointHeight*scaleHeight,	// random height
									z*terrainScale - adjFromOrig
								);
		}
	}

  _flag = NORMAL_SMOOTH;

  calculateCellBoundary();
  calculateNormals(NORMAL_FLAT);

  printTerrainData();
}

void SimpleTerrain::printTerrainData() // print out the file
{
	cout<<">> Print Terrain Data Points"<<endl;

	// now print the file out
	for(int x=0; x < dWidth; x++)		// x
	{
		for(int z=0; z<dHeight; z++)	// z
		{
				cout<<"["<<terrainData[x][z].x<<","<<terrainData[x][z].y<<","<<terrainData[x][z].z<<"]";
		}
		cout<<" \n"<<endl;
	}
}

void SimpleTerrain::render()
{
  glLineWidth(0.1f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

  // flat or smooth shading
  // smooth shading interpolates between vertices/edges
  if(_flag == NORMAL_SMOOTH)
  {
    glEnable(GL_SMOOTH);
		glShadeModel(GL_SMOOTH);
  }
  else if (_flag == NORMAL_FLAT)
  {
    glEnable(GL_FLAT);
    glShadeModel(GL_FLAT);
  }

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT, GL_FILL);
  //glDisable(GL_POLYGON_OFFSET_FILL);

	//glShadeModel(GL_SMOOTH);


	GLfloat mShininess[] = {8};

	GLfloat DiffuseMaterial[] = {1.0, 1.0, 1.0};
	GLfloat AmbientMaterial[] = {0.5, 0.5, 0.5};
	GLfloat SpecularMaterial[] = {1.0, 1.0, 1.0};

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, DiffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, AmbientMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SpecularMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

  glColor3f(1.0f, 1.0f, 1.0f);		// set colour
  glPolygonMode(GL_FRONT, GL_FILL);
  for(int x=0; x<dWidth; x++)
  {
    // this needs to be in the first loop so that the 'strip' is drawn properly
    glBegin(GL_TRIANGLE_STRIP);
    for(int z=0; z<dHeight; z++)
    {
        // vertex 0
        glNormal3f(terrainNormals[x][z].x, terrainNormals[x][z].y, terrainNormals[x][z].z);
        glVertex3f(terrainData[x][z].x, terrainData[x][z].y, terrainData[x][z].z);

        // vertex 1
        glNormal3f(terrainNormals[x][z+1].x, terrainNormals[x][z+1].y, terrainNormals[x][z+1].z);
        glVertex3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);

        // vertex 2
        glNormal3f(terrainNormals[x+1][z].x, terrainNormals[x+1][z].y, terrainNormals[x+1][z].z);
        glVertex3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);

        // vertex 3
        glNormal3f(terrainNormals[x+1][z+1].x, terrainNormals[x+1][z+1].y, terrainNormals[x+1][z+1].z);
        glVertex3f(terrainData[x+1][z+1].x, terrainData[x+1][z+1].y, terrainData[x+1][z+1].z);
    }
    glEnd();  // ending the strip after the creation of each row

  }

  glColor3f(0.0f, 0.0f, 0.0f);		// set colour
  glLineWidth(0.5f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  for(int x=0; x<dWidth; x++)
  {
    // this needs to be in the first loop so that the 'strip' is drawn properly
    glBegin(GL_TRIANGLE_STRIP);
    for(int z=0; z<dHeight; z++)
    {
        // vertex 0
        glNormal3f(terrainNormals[x][z].x, terrainNormals[x][z].y, terrainNormals[x][z].z);
        glVertex3f(terrainData[x][z].x, terrainData[x][z].y, terrainData[x][z].z);

        // vertex 1
        glNormal3f(terrainNormals[x][z+1].x, terrainNormals[x][z+1].y, terrainNormals[x][z+1].z);
        glVertex3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);

        // vertex 2
        glNormal3f(terrainNormals[x+1][z].x, terrainNormals[x+1][z].y, terrainNormals[x+1][z].z);
        glVertex3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);

        // vertex 3
        glNormal3f(terrainNormals[x+1][z+1].x, terrainNormals[x+1][z+1].y, terrainNormals[x+1][z+1].z);
        glVertex3f(terrainData[x+1][z+1].x, terrainData[x+1][z+1].y, terrainData[x+1][z+1].z);
    }
    glEnd();  // ending the strip after the creation of each row

  }

}

// this calculates each cell's boundary (each cell is made up of 4 vertices)
// the rationale for this is so that we can determine which 3 vertices to calculate the normals
// for getting the height map of the terrain
void SimpleTerrain::calculateCellBoundary()
{
	cout<<">> Calculating cell boundary..."<<endl;

	for(int x=0; x<dWidth; x++)								// x
	{
		//cout<<"----  x :: "<<x<<"---------------"<<endl;
		for(int z=0; z<dHeight; z++)								// z
		{
			cellinfo[x][z].top = z * terrainScale - adjFromOrig;         // top
			cellinfo[x][z].bottom = (z+1) * terrainScale - adjFromOrig;  // bottom
			cellinfo[x][z].left = x * terrainScale - adjFromOrig;			   // left
			cellinfo[x][z].right = (x+1) * terrainScale - adjFromOrig;   // right

			//cout<<"z:"<<z<<"    top:"<<cellinfo[x][z].top<<" bottom:"<<cellinfo[x][z].bottom<<" left:"<<cellinfo[x][z].left<<" right:"<<cellinfo[x][z].right<<endl;
		}
	}

}

float SimpleTerrain::getHeight(Vector3f pos)
{
	float terrainHeight = 0.0f;

	// check if the position is within the terrain boundary
	if(withinBoundary(pos, boundary))
	{

		// check the distance to the plane
		float dist = distanceToPlane(pos);

		// now calculate the height of the terrain
		if (dist <= 0.0)						// the pos is under the plane
			terrainHeight = pos.y + fabs(dist);
		else
			terrainHeight = pos.y - fabs(dist);

		//cout<<"position is within terrain boundary: "<<terrainHeight<<endl;
	}
	else
	{
		//cout<<"position is NOT within terrain boundary"<<endl;
	}

	return terrainHeight;
}

void SimpleTerrain::posToArrayIndex(Vector3f &pos, int &inX, int &inZ)
{
	// precalculate the halfWidth and halfHeight
	float halfWidth = dWidth/2;
	float halfHeight = dHeight/2;

	// dereference inX and inZ
	// convert the position to the index of the terrainData[x][z]
	// "pos.x/adjFromOrig" is the ratio of the pos in relation to the terrain.
	inX = floor((pos.x/adjFromOrig) * halfWidth + halfWidth);
	inZ = floor((pos.z/adjFromOrig) * halfHeight + halfHeight);
}

bool SimpleTerrain::withinBoundary(Vector3f pos, CELLINFO bounds)
{
	if ((pos.x > bounds.left) && (pos.x < bounds.right))
		if ((pos.z > bounds.top) && (pos.z < bounds.bottom))
			return true;
		else
			return false;
}

float SimpleTerrain::distanceToPlane(Vector3f pos)
{
	// plane equation = ax + by + cz + d = 0
	float dist = 0.0f;

	// get the normals of the plane where pos is
	// this requires conversion of posToArrayIndex
	int inX;
	int inZ;
  Vector3f faceNormal;

	posToArrayIndex(pos, inX, inZ);
	cout<<"CELL["<<inX<<"]["<<inZ<<"] T:"<<cellinfo[inX][inZ].top<<" B:"<<cellinfo[inX][inZ].bottom<<" L:"<<cellinfo[inX][inZ].left<<" R:"<<cellinfo[inX][inZ].right<<endl;
  //
  // // which triangle on a plane is the pos on?
  bool isAbove = Vector3f::isAboveLine(
    Vector3f(terrainData[inX][inZ+1].x, terrainData[inX][inZ+1].y, terrainData[inX][inZ+1].z),
    Vector3f(terrainData[inX+1][inZ].x, terrainData[inX+1][inZ].y, terrainData[inX+1][inZ].z),
    pos);

  float D;
  if(isAbove) // top triangle
  {
	  // calculate the normals for the top pair of triangle
    faceNormal = calculateFaceNormal(terrainData[inX][inZ], terrainData[inX][inZ+1], terrainData[inX+1][inZ]);

    // dot product of plane normal and plane position
  	D = faceNormal.dotProduct(terrainData[inX][inZ]);
    //faceNormal.print();
  }
  else  // bottom triangle
  {
    faceNormal = calculateFaceNormal(terrainData[inX+1][inZ+1], terrainData[inX+1][inZ], terrainData[inX][inZ+1]);
    //faceNormal = calculateFaceNormal(terrainData[inX+1][inZ], terrainData[inX][inZ+1], terrainData[inX+1][inZ+1]);

    // dot product of plane normal and plane position
  	D = faceNormal.dotProduct(terrainData[inX+1][inZ+1]);
  }
	//cout<<"D of plane:"<<D<<endl;

	// test the pos against the plane normals
	dist = faceNormal.dotProduct(pos) - D;
	//cout<<"The cube in relation to the plane is:"<<dist<<endl;

	return  dist;
}

Vector3f SimpleTerrain::calculateFaceNormal(Vector3f p0, Vector3f p1, Vector3f p2)
{
	// ------------------------------------------------ calculate normals

	// get vectors between points
	Vector3f vA = p1-p0;
	Vector3f vB = p2-p0;

	// cross product of vectors
	Vector3f vN = vA.crossProduct(vB);
	vN.normalise();

	//cout<<"---------------------->> Plane Normal Vector: ";
	//vN.print();

	return vN;
}

void SimpleTerrain::calculateNormals(int flag)
{
	Vector3f vN;	// the normal vector for each vertex

	if(flag == NORMAL_FLAT)
	{
		cout<<">> Calculating Terrain Normals: FLAT..."<<endl;

		// loop through all vertices
		for(int x=0; x<dWidth; x++)		// x
		{
			for(int z=0; z<dWidth; z++)	// z
			{
				// get the 3 points for computing the 2 vectors
				Vector3f p0 = Vector3f(terrainData[x][z].x, terrainData[x][z].y, terrainData[x][z].z);			// original point
				Vector3f p1 = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);	// the x line
				Vector3f p2 = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);	// the z line

				// compute the 2 vectors
				Vector3f vA = p1 - p0;	// x line vector
				Vector3f vB = p2 - p0;	// z line vector

				// get the crossproduct of the two vectors vA and vB
				vN = vA.crossProduct(vB);

				// normalise the vector vN
				vN.normalise();

				//cout<<"vN:"<<vN.x<<" "<<vN.y<<" "<<vN.z<<endl;

				// assign the normal to terrainNormal array
				terrainNormals[x][z] = vN;
			}
		}
	}
	else if (flag == NORMAL_SMOOTH)
	{
		/**** Calculate Average of 4 quad normals for the vertex ****/
		cout<<">> Calculating Terrain Normals: SMOOTH..."<<endl;


		// loop through all vertices
		for(int x=0; x < dWidth; x++)			// x
		{
			for(int z=0; z<dHeight; z++)		// z
			{
				Vector3f pOr = Vector3f(terrainData[x][z].x, terrainData[x][z].y, terrainData[x][z].z);			// original point

				if ((x==0) && (z==0))	// origin (upper left)
				{
					// only East and South Point is computed
					//cout<<"Origin: x="<<x<<" z="<<z<<endl;

					Vector3f pE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);		// east point
					Vector3f pS = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);		// south point

					// compute the 2 vectors
					Vector3f vE =	pE - pOr;	// east vector
					Vector3f vS = pS - pOr;	// south vector

					// get the crossproduct of the two vectors vE and vS
					vN = vE.crossProduct(vS);

					// normalise the vector vN
					vN.normalise();

					//cout<<"vN:"<<vN.x<<" "<<vN.y<<" "<<vN.z<<endl;

					// assign the normal to terrainNormal array
					terrainNormals[x][z] = -vN;

				}
				else if((x==0) && (z==dHeight))	// bottom left
				{
					// only North and East Point
					//cout<<"Bottom Left: x="<<x<<" z="<<z<<endl;

					Vector3f pN = Vector3f(terrainData[x][z-1].x, terrainData[x][z-1].y, terrainData[x][z-1].z);		// north point
					Vector3f pE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);		// east point

					// compute the 2 vectors
					Vector3f vE =	pE - pOr;		// east vector
					Vector3f vNr = pN - pOr;		// north vector

					// get the crossproduct of the two vectors vE and vS
					vN = vE.crossProduct(vNr);

					// normalise the vector vN
					vN.normalise();

					// assign the normal to terrainNormal array
					terrainNormals[x][z] = -vN;

				}
				else if((x==dWidth) && (z==0))	// upper right
				{
					// only compute west and south vector
					//cout<<"Upper Right: x="<<x<<" z="<<z<<endl;

					Vector3f pS = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);		// south point
					Vector3f pW = Vector3f(terrainData[x-1][z].x, terrainData[x-1][z].y, terrainData[x-1][z].z);		// east point

					// compute the 2 vectors
					Vector3f vW = pW - pOr;	// west vector
					Vector3f vS = pS - pOr;		// south vector

					// get the crossproduct of the two vectors vE and vS
					vN = vW.crossProduct(vS);

					// normalise the vector vN
					vN.normalise();

					// assign the normal to terrainNormal array
					terrainNormals[x][z] = -vN;

				}
				else	if((x==dWidth) && (z==dHeight))	// bottom right
				{
					// only the north and west vector
					//cout<<"Bottom Right: x="<<x<<" z="<<z<<endl;

					Vector3f pN = Vector3f(terrainData[x][z-1].x, terrainData[x][z-1].y, terrainData[x][z-1].z);		// north point
					Vector3f pW = Vector3f(terrainData[x-1][z].x, terrainData[x-1][z].y, terrainData[x-1][z].z);		// east point

					// compute the 2 vectors
					Vector3f vNr = pN - pOr;	// north vector
					Vector3f vW = pW - pOr;	// west vector

					// get the crossproduct of the two vectors vE and vS
					vN = vNr.crossProduct(vW);

					// normalise the vector vN
					vN.normalise();

					// assign the normal to terrainNormal array
					terrainNormals[x][z] = -vN;
				}
				else if ( (x==0) && ( (z>0) && (z<dHeight) ) )	// left cell span
				{
					// only north, south, NE, east
					//cout<<"Left Cell Span: x="<<x<<" z="<<z<<endl;

					// NE quadrant
					Vector3f pN = Vector3f(terrainData[x][z-1].x, terrainData[x][z-1].y, terrainData[x][z-1].z);			// north point
					Vector3f pNE = Vector3f(terrainData[x+1][z-1].x, terrainData[x+1][z-1].y, terrainData[x+1][z-1].z);	// NE point

					Vector3f vTR = pNE - pN;	// top to the right vector (A - B is B points to A)
					Vector3f vTB = pOr - pN;	// top to the bottom vector

					Vector3f vNENorm = vTR.crossProduct(vTB);

					// SE quadrant
					Vector3f pS = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);	// south point
					Vector3f pE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);	// east point

					Vector3f vOR = pE - pOr;	// origin to right vector
					Vector3f vOB = pS - pOr;	// origin to bottom vector

					Vector3f vSENorm = vOR.crossProduct(vOB);

					// sum the vector
					vN = vSENorm + vNENorm;

					// normalise the two vectors
					vN.normalise();

					// the vertex average normal is then assigned to the terrainNormals
					terrainNormals[x][z] = -vN;

				}
				else if ( (x==dWidth-1) && ( (z>0) && (z<dHeight) ) )	// **right cell span
				{
					//cout<<"Right Cell Span: x="<<x<<" z="<<z<<endl;
				}
				else if ( (z==0) && ( (x>0) && (x<dWidth) ) )	// top cell span
				{
					//cout<<"Top Cell Span: x="<<x<<" z="<<z<<endl;
				}
				else if ( (z==dHeight-1) && ( (x>0) && (x<dWidth) ) )	// **bottom cell span
				{
					//cout<<"Bottom Cell Span: x="<<x<<" z="<<z<<endl;
				}
				else	// all centre cells
				{
					//cout<<"Centre Cells: x="<<x<<" z="<<z<<endl;
					pOr = Vector3f(terrainData[x][z].x, terrainData[x][z].y, terrainData[x][z].z);			// original point
					Vector3f pN = Vector3f(terrainData[x][z-1].x, terrainData[x][z-1].y, terrainData[x][z-1].z);			// north point
					Vector3f pNE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z-1].y, terrainData[x+1][z-1].z);	// NE point
					Vector3f pE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);		// east point
					Vector3f pS = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);		// south point
					Vector3f pSW = Vector3f(terrainData[x-1][z+1].x, terrainData[x-1][z+1].y, terrainData[x-1][z+1].z);	// southwest point
					Vector3f pW = Vector3f(terrainData[x-1][z].x, terrainData[x-1][z].y, terrainData[x-1][z].z);			// west point

					// NE quadrant
					Vector3f vO_N = pN - pOr;	// origin to northvector
					Vector3f vO_E = pE - pOr;	// origin to east vector

					Vector3f vNENorm = vO_N.crossProduct(vO_E);
					vNENorm.normalise();

					// NW quadrant
					Vector3f vO_W = pW - pOr;	// origin to west vector

					Vector3f vNWNorm = vO_W.crossProduct(vO_N);
					vNWNorm.normalise();

					// SE quadrant
					Vector3f vO_S = pS - pOr;	// origin to south vector

					Vector3f vSENorm = vO_S.crossProduct(vO_E);
					vSENorm.normalise();

					// SW quadrant
					Vector3f vSWNorm = vO_S.crossProduct(vO_W);
					vSWNorm.normalise();

					// sum the vector
					vN = vNENorm + vNWNorm + vSENorm +  vSWNorm;

					//vNENorm.printVector();
					//vNWNorm.printVector();
					//vSENorm.printVector();
					//vSWNorm.printVector();

					//cout<<"vN of all:";
					//vN.printVector();

					// normalise the two vectors
					vN.normalise();

					//cout<<"vN Normalised:";
					//vN.printVector();

					// the vertex average normal is then assigned to the terrainNormals
					terrainNormals[x][z] = -vN;
				}

			}
		}

		cout<<">> Terrain Normals calculated successfully"<<endl;

	}

}

SimpleTerrain::~SimpleTerrain()
{
  cout<<"Simple Terrain Destroyed"<<endl;
}
