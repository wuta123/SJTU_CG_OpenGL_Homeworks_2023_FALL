#pragma once
#include "Mesh.h"
#include "Camera.h"
#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include "StringUtil.h"

using std::min;
using std::max;
using std::vector;
using std::string;
using std::map;

class Model {
public:
	Model(std::string path, bool useSplit = false) {
			loadModel(path, useSplit);
	}
	Model() = default;
	void Draw(Shader &shader, Camera &cam);
	void revisePosition();
	void DrawInnerMesh(Shader& shader, Camera& cam);
	void DrawCollideBox(Shader& shader, Camera& cam);
	void move(float deltaTime);
	void useForce(float deltaTime = 1.0f / 60.0f, glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), GLuint meshNum = 0);
	void hitGround(GLuint meshNum = 0);
	void setVelocity(float x, float y, float z);
	void setVelocity(float x, float y, float z, float v);
	void setAcceleration(float x, float y, float z);
	void setAcceleration(float x, float y, float z, float v);
	void setAngularVelocity(float x, float y, float z);
	void setAngularVelocity(float x, float y, float z, float v);
	void setAngularAcceleration(float x, float y, float z);
	void setAngularAcceleration(float x, float y, float z, float v);
	void setScale(float x, float y, float z);
	bool useGravity = false;
	bool useDecrease = false; //物体的速度和角速度是否会随着时间衰减
	bool useGround = false; //是否会触地回弹
	float vDecreaseCo = 0.99; //速度的衰减系数(每1/60秒速度变为原来的多少)
	float aDecreaseCo = 0.98; //角速度的衰减系数
	float vMin = 0.5; //速度小于这个值会看作0
	float aMin = 0.005; //角速度小于这个值会看作0;
	const float gravityCoefficient = 9.82f;
	float bounciness = 0.90f; //碰撞恢复系数
	float inertia = 225.0f; //转动惯量
	float extents[3]; //碰撞箱在三个尺度上的半长度，分别是x、y、z
	float mass = 50.0f; //模型重量
	std::vector<glm::vec3> defaultGravityPosition;
	std::vector<glm::vec3> gravityPosition; //重心位置（默认为0）
	std::vector<glm::mat4> trans;
	std::vector<glm::mat4> rotate;
	glm::mat4 scale = glm::mat4(1.0f);
	glm::vec3 defaultPos = glm::vec3(0.0f, 0.0f, 0.0f);
	static bool detectCollision(Model& m1, Model& m2);
	static void collisionWithNoShatter(Model& m1, Model& m2);
	void collisionWithShatter(Model& m2);
	float deltaTime = 1.0f / 60.0f;
	bool splited = false;
private:
	const glm::vec4 baseColor = glm::vec4(0.7f, 0.7f, 0.7f, 0.7f);
	std::vector<glm::vec3> velocity; //速度
	std::vector<glm::vec3> acceleration; //加速度
	std::vector<glm::vec3> angularVelocity; //转动角速度
	std::vector<glm::vec3> angularAcceleration;//转动角加速度

	std::vector<Mesh> meshes;
	std::vector<Mesh> innerMeshes;
	std::vector<Mesh> collideBox;
	std::string directory;
	Assimp::Importer importer;
	std::vector<Texture> textures_loaded;
	GLuint meshNum = 0;
	std::vector<float> xMin, xMax, yMin, yMax, zMin, zMax;
	void loadModel(std::string path, bool useSplit);
	void processNode(aiNode* node, const aiScene* scene, bool useSplit);
	void processInnerMesh();
	void processCollideBox();
	void prepareMesh(GLuint num = 1);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);


};

