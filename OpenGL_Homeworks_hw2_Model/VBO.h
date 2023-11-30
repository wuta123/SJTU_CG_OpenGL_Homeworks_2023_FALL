#pragma once
#ifndef		 VBO_CLASS_H
#define	 VBO_CLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
	Vertex(glm::vec3 posi) {
		position = posi;
		normal = glm::vec3(0.0f, 0.0f, 0.0f);
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		texUV = glm::vec2(0.0f, 0.0f);
	}
	Vertex() = default;
};

class VBO {
public:
	GLuint ID;

	VBO(std::vector<Vertex>& vertices);
	//VBO(float* vertices, GLsizeiptr size);

	void Bind();

	void Unbind();

	void Delete();
};


#endif // !VBO_CLASS_H
