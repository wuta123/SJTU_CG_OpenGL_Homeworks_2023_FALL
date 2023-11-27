#include "Mesh.h"
#ifndef GROUND_CLASS_H
#define GROUND_CLASS_H

class Ground
{
public:
	float width;
	float height;
	float xLimit;
	float yLimit;
	Mesh groundMesh;
	void Draw(Shader& shader, Camera& cam);
	Ground(float width = 10.0f, float height = 10.0f, float xLimit = 1000.0f, float yLimit = 1000.0f);
};

#endif