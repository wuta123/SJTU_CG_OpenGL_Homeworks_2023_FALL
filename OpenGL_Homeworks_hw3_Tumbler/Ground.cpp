#include "Ground.h"

void Ground::Draw(Shader& shader, Camera& cam)
{
	shader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(trans * rotate));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate));
	groundMesh.Draw(shader, cam);
}

Ground::Ground(float width, float height, float xLimit, float yLimit)
{
	this->width = width;
	this->height = height;
	this->xLimit = xLimit;
	this->yLimit = yLimit;
	int xNum = 2.0f * xLimit / width;
	int yNum = 2.0f * yLimit / height;
	float currentX = -xLimit, currentY = -yLimit;
	for (unsigned p = 0; p < xNum; p++) {
		for (unsigned q = 0; q < yNum; q++) {
			Vertex newVertex0 = Vertex(glm::vec3(currentX, 0.0f, currentY), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f));
			Vertex newVertex1 = Vertex(glm::vec3(currentX, 0.0f, currentY + height), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));
			Vertex newVertex2 = Vertex(glm::vec3(currentX + width, 0.0f, currentY), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f));
			Vertex newVertex3 = Vertex(glm::vec3(currentX + width, 0.0f, currentY + height), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f));
			vert.push_back(newVertex0), vert.push_back(newVertex1), vert.push_back(newVertex2), vert.push_back(newVertex3);
			GLuint size = (p * yNum + q) * 4;
			GLuint size1 = size + 1, size2 = size + 2, size3 = size + 3;
			indices.push_back(size), indices.push_back(size1), indices.push_back(size2);
			indices.push_back(size1), indices.push_back(size3), indices.push_back(size2);
			currentY += height;
		}
		currentX += width, currentY = -yLimit;
	}
	groundMesh = Mesh(vert, indices, tex);
}

Ground::Ground(const Ground& g)
{
	this->width = g.width;
	this->height = g.height;
	this->xLimit = g.xLimit;
	this->yLimit = g.yLimit;
	this->trans = g.trans;
	this->rotate = g.rotate;
	this->groundTex = g.groundTex;
	this->groundMesh = g.groundMesh;
	this->indices = g.indices;
	this->vert = g.vert;
}

void Ground::setTex(Texture texture)
{
	this->tex.clear();
	this->tex.push_back(texture);
	groundMesh = Mesh(vert, indices, tex);
}

void Ground::setTex(std::string img)
{
	Texture tex(img.c_str(), "texture_diffuse", 0);
	setTex(tex);
}

void Ground::setPosiAndRotation(glm::vec3 posi, glm::vec3 rotateAxis, float angle)
{
	trans = glm::translate(glm::mat4(1.0f), posi);
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(angle), rotateAxis);
}
