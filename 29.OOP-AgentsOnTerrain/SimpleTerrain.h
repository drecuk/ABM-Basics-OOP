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
//	The calculation of normals for TRIANGLE is used for GL_TRIANGLE_STRIP
//	therefore, minor error exists when agents skirt on the surface
//
//	##########################################################

#ifndef SIMPLETERRAIN_H
#define SIMPLETERRAIN_H

#include "OGLUtil.h"

struct CELLINFO
{
	float top;
	float bottom;
	float left;
	float right;
};

enum { NORMAL_FLAT, NORMAL_SMOOTH };

class SimpleTerrain
{
private:
	// 4 x 4 terrain, producing 16 quads and 32 triangles
	// -----------------------------------------------------------------------------
	float heightField[5][5];       // the size of the heightfield[x][y]
	Vector3f terrainNormals[5][5];   // the terrain normals for each point
  Vector3f terrainData[5][5];      // terrain data

	CELLINFO cellinfo[5][5];		// each polygon (quad) is a cell (this is its boundary)
	CELLINFO boundary;			// boundary of the entire terrain (2D bounding box)

	float scaleHeight;				// height scaling factor of terrain
	float terrainScale;				// scaling size for terrain
	float adjFromOrig;				// adjustment variable to set terrain centre at origin

	int dWidth, dHeight;			// width and height of terrain (how many pixels)
  // int qWidth, qHeight;

	int _flag; // NORMAL_SMOOTH or NORMAL_FLAT normals and shading

public:
  SimpleTerrain();
	SimpleTerrain(int width, int height, float _scaleHeight, float terrainSize);
	~SimpleTerrain();

  void printTerrainData();
	void render();
	void generateTerrainPoints();
	void calculateCellBoundary();
	float getHeight(Vector3f pos);
	void posToArrayIndex(Vector3f &pos, int &inX, int &inZ);
	bool withinBoundary(Vector3f pos, CELLINFO bounds);
	float distanceToPlane(Vector3f pos);
	Vector3f calculateFaceNormal(Vector3f p0, Vector3f p1, Vector3f p2);
	void calculateNormals(int flag);
};

#endif
