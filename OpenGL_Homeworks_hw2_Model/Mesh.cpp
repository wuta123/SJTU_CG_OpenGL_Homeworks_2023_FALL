#include "Mesh.h"
#pragma once


glm::vec3 lineAndPlaneConvexPoint(GLfloat a, GLfloat b, GLfloat c, glm::vec3 p1, glm::vec3 p2) {
	// 计算直线的方向向量
	glm::vec3 direction = p2 - p1;

	// 计算直线方程的参数
	GLfloat t = -(a * p1.x + b * p1.y + c * p1.z + 1) / glm::dot(direction, glm::vec3(a, b, c));

	// 计算交点坐标
	glm::vec3 intersection = p1 + t * direction;
	return intersection;
}

glm::vec3 getCutFace(glm::vec3 min, glm::vec3 max) {
	//min: 三维立方体的空间最小点， max：三维立方体的空间最大点
	//返回值：glm::vec3(a, b, c)，满足面ax + by + cz + 1 = 0可以切开以min为空间最小点、max为空间最大点的立方体。
	// 生成随机的平面向量

	//随机生成一个平面的法向量
	GLfloat bottom = rand() % 5;
	GLfloat tmp1 = rand() % 3 - 2.468974132 + bottom, tmp2 = rand() % 3 - 2.6311549 + bottom;
	tmp1 += (tmp1 > 0) ? 0.001f : -0.001f;
	tmp2 += (tmp2 > 0) ? 0.001f : -0.001f;
	glm::vec3 res = glm::vec3(1.0f / (bottom+1.0f), 1.0f / (tmp1), 1.0f / (tmp2));
	res = glm::normalize(res);
	//在Min~Max的体对角线上随机选择一个k
	glm::vec3 k = min + (max - min) * (1.0f / (rand() % 5 + 1.05f));
	//计算可以经过k点的面参数d
	GLfloat d = -glm::dot(k, res);
	d += (d > 0) ?  0.001f : -0.001f;
	//res的每一项除掉d
	res /= d;
	return res;
}

std::vector<Mesh> splitMeshByFace(Mesh mesh, GLfloat a, GLfloat b, GLfloat c)
{
	//将当前这个网格按照面方程：ax + by + cz +1 = 0分为两类：
	// 1. ax + by + cz + 1 >= 0的，将原本的索引和切割后的索引的映射储存在m1中（在原网格中索引的位置->在新的切割后网格的索引位置+1）
	// 2. ax + by + cz + 1<= 0的，将原本的索引和切割后的索引的映射储存在m2中（在原网格中索引的位置->在新的切割后网格的索引位置+1）
	//注意，在分割面上的点应该添加给所有的新网格。
	if (a == 0 && b == 0 && c == 0) {
		std::vector<Mesh> re;
		re.push_back(mesh);
		return re;
	}
	map<GLuint, GLuint> m1, m2;
	std::vector<Vertex> v1, v2;
	std::vector<GLuint> i1, i2;
	std::vector<Texture> t;
	GLuint k1 = 0, k2 = 0;
	GLuint totalCount = mesh.vertices.size();


	//1.处理点列表
	for (GLuint i = 0; i < totalCount; i++) {
		glm::vec3 vector = mesh.vertices[i].position;
		float re = a* vector.x + b * vector.y + c * vector.z + 1;
		if (re >= 0) {
			if (m1[i] == 0) {
				m1[i] = ++k1;
				v1.push_back(mesh.vertices[i]);
			}
		}
		if(re <= 0){
			if (m2[i] == 0) {
				m2[i] = ++k2;
				v2.push_back(mesh.vertices[i]);
			}
		}
	}

	if (v1.size() == 0 || v2.size() == 0) {
		std::vector<Mesh> re;
		re.push_back(mesh);
		return re;
	}

	//2.处理索引列表
	//需要做的事：
	//1. 将原本的索引分别改为在子网格当中的新索引
	//2. 对每一条边，判断是否一边的两点在分割面的两侧，如果在两侧，计算分割面和边的交点，将交点加入两个子网格的顶点数组，并让原来的边分割成两块。

	totalCount = mesh.indices.size();
	for (GLuint i = 0; i < totalCount; i += 3) {
		glm::vec3 p1 = mesh.vertices[mesh.indices[i]].position, p2 = mesh.vertices[mesh.indices[i + 1]].position, p3 = mesh.vertices[mesh.indices[i + 2]].position;
		float p1O = (a * p1.x + b * p1.y + c * p1.z + 1), p2O = (a * p2.x + b * p2.y + c * p2.z + 1), p3O = (a * p3.x + b * p3.y + c * p3.z + 1);
		int res = (p1O == 0.0f) + (p2O == 0.0f) + (p3O == 0.0f);
		if (res == 0) {
			//最普通的情况，面和两条线相交或者不和三角形相交
			if (p1O > 0 && p2O > 0 && p3O > 0) {
				i1.push_back(m1[mesh.indices[i]] - 1);
				i1.push_back(m1[mesh.indices[i + 1]] - 1);
				i1.push_back(m1[mesh.indices[i + 2]] - 1);
				continue;
			}
			else if (p1O < 0 && p2O < 0 && p3O < 0) {
				i2.push_back(m2[mesh.indices[i]] - 1);
				i2.push_back(m2[mesh.indices[i + 1]] - 1);
				i2.push_back(m2[mesh.indices[i + 2]] - 1);
				continue;
			}


			//首先判断是两个正还是两个负
			int res2 = (p1O > 0.0f) + (p2O > 0.0f) + (p3O > 0.0f);
			if (res2 > 1) {
				//两个正
				if (p1O < 0.0f) {
					glm::vec3 convexPoint1 = lineAndPlaneConvexPoint(a, b, c, p1, p2),
						convexPoint2 = lineAndPlaneConvexPoint(a, b, c, p1, p3);
					Vertex newV1 = Vertex(convexPoint1), 
						newV2 = Vertex(convexPoint2);
					v2.push_back(newV1), v2.push_back(newV2);
					i2.push_back(m2[mesh.indices[i]] - 1); i2.push_back(k2++); i2.push_back(k2++);

					v1.push_back(newV1), v1.push_back(newV2);
					i1.push_back(m1[mesh.indices[i + 1]] - 1), i1.push_back(m1[mesh.indices[i + 2]] - 1), i1.push_back(k1);
					i1.push_back(m1[mesh.indices[i + 2]] - 1), i1.push_back(k1 + 1), i1.push_back(k1);
					k1+= 2;
				}
				else if (p2O < 0.0f) {
					glm::vec3 convexPoint1 = lineAndPlaneConvexPoint(a, b, c, p2, p3),
						convexPoint2 = lineAndPlaneConvexPoint(a, b, c, p2, p1);
					Vertex newV1 = Vertex(convexPoint1),
						newV2 = Vertex(convexPoint2);
					v2.push_back(newV1), v2.push_back(newV2);
					i2.push_back(m2[mesh.indices[i + 1]] - 1); i2.push_back(k2++); i2.push_back(k2++);

					v1.push_back(newV1), v1.push_back(newV2);
					i1.push_back(m1[mesh.indices[i + 2]] - 1), i1.push_back(m1[mesh.indices[i]] - 1), i1.push_back(k1);
					i1.push_back(m1[mesh.indices[i]] - 1), i1.push_back(k1 + 1), i1.push_back(k1);
					k1 += 2;
				}
				else if (p3O < 0.0f) {
					glm::vec3 convexPoint1 = lineAndPlaneConvexPoint(a, b, c, p3, p1),
						convexPoint2 = lineAndPlaneConvexPoint(a, b, c, p3, p2);
					Vertex newV1 = Vertex(convexPoint1),
						newV2 = Vertex(convexPoint2);
					v2.push_back(newV1), v2.push_back(newV2);
					i2.push_back(m2[mesh.indices[i + 2]] - 1); i2.push_back(k2++); i2.push_back(k2++);

					v1.push_back(newV1), v1.push_back(newV2);
					i1.push_back(m1[mesh.indices[i]] - 1), i1.push_back(m1[mesh.indices[i + 1]] - 1), i1.push_back(k1);
					i1.push_back(m1[mesh.indices[i + 1]] - 1), i1.push_back(k1 + 1), i1.push_back(k1);
					k1 += 2;
				}
			}
			else {
				//两个负
				if (p1O > 0.0f) {
					glm::vec3 convexPoint1 = lineAndPlaneConvexPoint(a, b, c, p1, p2),
						convexPoint2 = lineAndPlaneConvexPoint(a, b, c, p1, p3);
					Vertex newV1 = Vertex(convexPoint1),
						newV2 = Vertex(convexPoint2);
					v1.push_back(newV1), v1.push_back(newV2);
					i1.push_back(m1[mesh.indices[i]] - 1); i1.push_back(k1++); i1.push_back(k1++);

					v2.push_back(newV1), v2.push_back(newV2);
					i2.push_back(m2[mesh.indices[i + 1]] - 1), i2.push_back(m2[mesh.indices[i + 2]] - 1), i2.push_back(k2);
					i2.push_back(m2[mesh.indices[i + 2]] - 1), i2.push_back(k2 + 1), i2.push_back(k2);
					k2 += 2;
				}
				else if (p2O > 0.0f) {
					glm::vec3 convexPoint1 = lineAndPlaneConvexPoint(a, b, c, p2, p3),
						convexPoint2 = lineAndPlaneConvexPoint(a, b, c, p2, p1);
					Vertex newV1 = Vertex(convexPoint1),
						newV2 = Vertex(convexPoint2);
					v1.push_back(newV1), v1.push_back(newV2);
					i1.push_back(m1[mesh.indices[i + 1]] - 1); i1.push_back(k1++); i1.push_back(k1++);

					v2.push_back(newV1), v2.push_back(newV2);
					i2.push_back(m2[mesh.indices[i + 2]] - 1), i2.push_back(m2[mesh.indices[i]] - 1), i2.push_back(k2);
					i2.push_back(m2[mesh.indices[i]] - 1), i2.push_back(k2 + 1), i2.push_back(k2);
					k2 += 2;
				}
				else if (p3O > 0.0f) {
					glm::vec3 convexPoint1 = lineAndPlaneConvexPoint(a, b, c, p3, p1),
						convexPoint2 = lineAndPlaneConvexPoint(a, b, c, p3, p2);
					Vertex newV1 = Vertex(convexPoint1),
						newV2 = Vertex(convexPoint2);
					v1.push_back(newV1), v1.push_back(newV2);
					i1.push_back(m1[mesh.indices[i + 2]] - 1); i1.push_back(k1++); i1.push_back(k1++);

					v2.push_back(newV1), v2.push_back(newV2);
					i2.push_back(m2[mesh.indices[i]] - 1), i2.push_back(m2[mesh.indices[i + 1]] - 1), i2.push_back(k2);
					i2.push_back(m2[mesh.indices[i + 1]] - 1), i2.push_back(k2 + 1), i2.push_back(k2);
					k2 += 2;
				}
			}
		}
		else if (res == 1) {
			//三角形的一个顶点在面上
			if (p1O == 0.0f) {
				if (p2O < 0) {
					glm::vec3 convexPoint = lineAndPlaneConvexPoint(a, b, c, p2, p3);
					Vertex newV = Vertex(convexPoint);
					v1.push_back(newV);
					v2.push_back(newV);
					i1.push_back(m1[mesh.indices[i]] - 1);
					i1.push_back(k1++);
					i1.push_back(m1[mesh.indices[i + 2]] - 1);
					i2.push_back(m2[mesh.indices[i]] -1);
					i2.push_back(m2[mesh.indices[i + 1]] - 1);
					i2.push_back(k2++);
				}
				else {
					glm::vec3 convexPoint = lineAndPlaneConvexPoint(a, b, c, p2, p3);
					Vertex newV = Vertex(convexPoint);
					v1.push_back(newV);
					v2.push_back(newV);
					i1.push_back(m1[mesh.indices[i]] -1);
					i1.push_back(m1[mesh.indices[i + 1]] - 1);
					i1.push_back(k1++);
					i2.push_back(m2[mesh.indices[i]] -1);
					i2.push_back(k2++);
					i2.push_back(m2[mesh.indices[i + 2]] -1);

				}
			}
			else if (p2O == 0.0f) {
				if (p1O < 0) {
					glm::vec3 convexPoint = lineAndPlaneConvexPoint(a, b, c, p1, p3);
					Vertex newV = Vertex(convexPoint);
					v1.push_back(newV);
					v2.push_back(newV);
					i1.push_back(m1[mesh.indices[i + 1]] -1);
					i1.push_back(m1[mesh.indices[i + 2]] -1);
					i1.push_back(k1++);
					i2.push_back(m2[mesh.indices[i]] -1);
					i2.push_back(m2[mesh.indices[i + 1]] -1);
					i2.push_back(k2++);
				}
				else {
					glm::vec3 convexPoint = lineAndPlaneConvexPoint(a, b, c, p1, p3);
					Vertex newV = Vertex(convexPoint);
					v1.push_back(newV);
					v2.push_back(newV);
					i1.push_back(m1[mesh.indices[i]] -1);
					i1.push_back(m1[mesh.indices[i + 1]] -1);
					i1.push_back(k1++);
					i2.push_back(m2[mesh.indices[i + 1]] -1);
					i2.push_back(m2[mesh.indices[i + 2]] -1);
					i2.push_back(k2++);
				}
			}
			else if (p3O == 0.0f) {
				if (p1O < 0) {
					glm::vec3 convexPoint = lineAndPlaneConvexPoint(a, b, c, p1, p2);
					Vertex newV = Vertex(convexPoint);
					v1.push_back(newV);
					v2.push_back(newV);
					i1.push_back(m1[mesh.indices[i + 1]] -1);
					i1.push_back(m1[mesh.indices[i + 2]] -1);
					i1.push_back(k1++);
					i2.push_back(m2[mesh.indices[i]] -1);
					i2.push_back(k2++);
					i2.push_back(m2[mesh.indices[i + 2]] -1);
				}
				else {
					glm::vec3 convexPoint = lineAndPlaneConvexPoint(a, b, c, p1, p2);
					Vertex newV = Vertex(convexPoint);
					v1.push_back(newV);
					v2.push_back(newV);
					i1.push_back(m1[mesh.indices[i]] -1);
					i1.push_back(k1++);
					i1.push_back(m1[mesh.indices[i + 2]] - 1);
					i2.push_back(m2[mesh.indices[i + 1]] -1);
					i2.push_back(m2[mesh.indices[i + 2]] -1);
					i2.push_back(k2++);
				}
			}
		}
		else if(res == 2){
			//三角形的两个顶点在面上，这种情况不用特殊处理，只需要将原来的索引换成某一子网格当中的索引即可
			if (p1O > 0 || p2O > 0 || p3O > 0) {
				i1.push_back(m1[mesh.indices[i]] - 1);
				i1.push_back(m1[mesh.indices[i + 1]] - 1);
				i1.push_back(m1[mesh.indices[i + 2]] - 1);
			}
			else {
				i2.push_back(m2[mesh.indices[i]] - 1);
				i2.push_back(m2[mesh.indices[i + 1]] - 1);
				i2.push_back(m2[mesh.indices[i + 2]] - 1);
			}
		}
		else {
			i1.push_back(m1[mesh.indices[i]] - 1);
			i1.push_back(m1[mesh.indices[i + 1]] - 1);
			i1.push_back(m1[mesh.indices[i + 2]] - 1);
			i2.push_back(m2[mesh.indices[i]] - 1);
			i2.push_back(m2[mesh.indices[i + 1]] - 1);
			i2.push_back(m2[mesh.indices[i + 2]] - 1);
		}
	}


	std::vector<Mesh> res;
	if(v1.size() > 3) res.push_back(Mesh(v1, i1, t));
	if(v2.size() > 3) res.push_back(Mesh(v2, i2, t));
	return res;
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	VAO.Bind();

	VBO VBO(vertices);
	EBO EBO(indices);

	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();

}

std::vector<Mesh> Mesh::splitMesh(GLuint split, glm::vec3 max, glm::vec3 min)
{
	//该函数将一个mesh随机分解为split个子网格，而且分割后会重新构建分割面以及它们的法向量
	//max：该网格的最大点， min：该网格的最小点
	std::vector<Mesh> tmp1, tmp2;
	bool t1 = false;
	tmp1.push_back(*this);

	if (split > indices.size() / 3 || split < 0)
		split = indices.size() / 3;
	for(int i = 0; i < split; i++){
		glm::vec3 face = getCutFace(min, max);
		if (t1) {
			for (int j = 0; j < tmp1.size(); j++) {
				std::vector<Mesh> splitRes = splitMeshByFace(tmp1[j], face.x, face.y, face.z);
				for (int k = 0; k < splitRes.size(); k++)
					tmp2.push_back(splitRes[k]);
			}
			tmp1.clear();
		}
		else {
			for (int j = 0; j < tmp2.size(); j++) {
				std::vector<Mesh> splitRes = splitMeshByFace(tmp2[j], face.x, face.y, face.z);
				for (int k = 0; k < splitRes.size(); k++)
					tmp1.push_back(splitRes[k]);
			}
			tmp2.clear();
		}
		t1 = !t1;
	}

	//计算法线
	return t1 ? tmp1 : tmp2;
}






void Mesh::Draw(Shader& shader, Camera& camera)
{
	shader.Activate();
	VAO.Bind();

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned i = 0; i < textures.size(); i++) {
		std::string num;
		std::string type = textures[i].type;
		if (type == "texture_diffuse") {
			num = std::to_string(numDiffuse++);
		}
		else if (type == "texture_specular") {
			num = std::to_string(numSpecular++);
		}
		textures[i].texUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "camMatrix");

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}
