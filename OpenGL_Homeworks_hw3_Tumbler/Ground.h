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
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 rotate = glm::mat4(1.0f);
	Texture groundTex;
	Mesh groundMesh;
	std::vector<Vertex> vert;
	std::vector<GLuint> indices;
	std::vector<Texture> tex;
	void Draw(Shader& shader, Camera& cam);
	Ground(float width = 10.0f, float height = 10.0f, float xLimit = 1000.0f, float yLimit = 1000.0f);
	Ground(const Ground& g);
	void setTex(Texture tex);
	void setTex(std::string img);
	void setPosiAndRotation(glm::vec3 posi, glm::vec3 rotateAxis, float angle);
};

#endif