#include "Ground.h"

void Ground::Draw(Shader& shader, Camera& cam)
{
	groundMesh.Draw(shader, cam);
}

Ground::Ground(float width, float height, float xLimit, float yLimit)
{
	this->width = width;
	this->height = height;
	this->xLimit = xLimit;
	this->yLimit = yLimit;
	std::vector<Vertex> vert;
	std::vector<GLuint> indices;
	std::vector<Texture> tex;
	int xNum = 2.0f * xLimit / width;
	int yNum = 2.0f * yLimit / height;
	float currentX = -xLimit, currentY = -yLimit;
	for (unsigned p = 0; p < xNum; p++) {
		for (unsigned q = 0; q < yNum; q++) {
			vert.push_back(Vertex(glm::vec3(currentX, 0.0f, currentY += height)));
		}
		currentX += width;
		currentY = -yLimit;
	}

	for (unsigned p = 0; p < xNum-1; p++) {
		for (unsigned q = 1; q < yNum; q++) {
			indices.push_back(q + p * yNum);
			indices.push_back(q - 1 + p * yNum);
			indices.push_back(q + (p + 1) * yNum);
			indices.push_back(q - 1 + p * yNum);
			indices.push_back(q + (p + 1) * yNum);
			indices.push_back(q - 1 + (p + 1) * yNum);
		}
	}

	groundMesh = Mesh(vert, indices, tex);

}
