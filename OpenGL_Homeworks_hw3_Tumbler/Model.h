#pragma once
#include "Mesh.h"
#include "Camera.h"
#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "Ground.h"
#include <map>
#include <iostream>
#include <fstream>
#include "StringUtil.h"
#include "Particle.h"

using std::min;
using std::max;
using std::vector;
using std::string;
using std::map;

class Model {
public:
	Model(std::string path, float x, float y, float z) {
		setScale(x, y, z);
		loadModel(path);
		revisePosition();
	}
	Model(std::string path, float scaleCof = 1.0f) {
		setScale(scaleCof, scaleCof, scaleCof);
		loadModel(path);
		revisePosition();
	}
	Model() = default;
	Model(const Model& model);
	void changeTexture(Texture texture);
	void Draw(Shader &shader, Camera &cam);
	void DrawCollideBox(Shader& shader, Camera& cam);
	void DrawGravityPosition(Shader& shader, Camera& cam);
	void DrawLiftPosition(Shader& shader, Camera& cam);
	void DrawRotateCenter(Shader& shader, Camera& cam);
	void DrawParticle(Shader& shader, Camera& cam);
	glm::vec3 getCenterWorld();
	float getCenterDistance(glm::vec3 point);
	void hitWall(glm::vec3 velocityCoefficient);
	void selfBalance();
	void move(float deltaTime);
	glm::mat4 rotateArbitraryAxis(glm::vec3 v1, glm::vec3 dir, float theta);
	void revisePosition();
	bool rayCast(glm::vec3 origin, glm::vec3 direction);
	void setVelocity(float x, float y, float z);
	void setVelocity(float x, float y, float z, float v);
	void setAcceleration(float x, float y, float z);
	void setAcceleration(float x, float y, float z, float v);
	void setAngularVelocity(float x, float y, float z);
	void setAngularVelocity(float x, float y, float z, float v);
	void setAngularAcceleration(float x, float y, float z);
	void setAngularAcceleration(float x, float y, float z, float v);
	void setScale(float x, float y, float z);
	void setDefaultPosition(float x, float y, float z);
	void setDefaultRotate(glm::vec3 axis, float angle);
	void setMaxLimit(float x, float y, float z);
	void setMinLimit(float x, float y, float z);
	void setPosition(float x, float y, float z);
	void setRotate(glm::vec3 axis, float angle);
	void setRotateOffset(glm::vec3 offset);
	void setGravityOffset(float x, float y, float z);
	void useForce(float deltaTime = 1.0f / 60.0f, glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
	void fixVelocityAndAngularVelocity(float fixVelocity, float fixAngularVelocity);
	void setUpBound(float upboundVelocity, float upboundAngularVelocity);
	void UnfixVelocityAndAngularVelocity();
	void AddTrailingParticle(const ParticleGenerator &p);
	void AddCollisionParticle(const ParticleGenerator& p);
	void AddDestroyParticle(const ParticleGenerator& p);
	void beDestroyed();
	bool useTrailingParticle = false;
	bool useCollisionParticle = false;
	bool useDestroyParticle = false;
	bool useTextureChange = false;
	bool useGravity = false;
	bool useDecrease = false; //物体的速度和角速度是否会随着时间衰减
	bool useGround = false; //是否会触地回弹
	float vDecreaseCo = 0.99; //速度的衰减系数(每1/60秒速度变为原来的多少)
	float aDecreaseCo = 0.99; //角速度的衰减系数
	float vMin = 0.5; //速度小于这个值会看作0
	float aMin = 0.0005; //角速度小于这个值会看作0;
	const float gravityCoefficient = 9.82f;
	float bounciness = 0.90f; //碰撞恢复系数
	float inertia = 225.0f; //转动惯量
	float extents[3]; //碰撞箱在三个尺度上的半长度，分别是x、y、z
	float mass = 50.0f; //模型重量
	GLuint ID = 0; //模型ID；
	int collideID = -1;
	float fixedVelocity = 0.0f, fixedAngularVelocity = 0.0f;
	float upboundVelocity = 0.0f, upboundAngularVelocity = 0.0f;
	bool isFixed = false;
	bool useUpbound = false;
	glm::vec3 gravityPosition = glm::vec3(0.0f, 0.0f, 0.0f); //重心位置（默认为0）
	glm::vec3 center; //几何中心
	glm::vec3 gravityOffset = glm::vec3(0.0f, 0.0f, 0.0f); //重心位置距离几何中心位置的偏移量
	glm::vec3 rotateCenter; //当前的旋转中心，默认为几何中心
	glm::vec3 rotateOffset = glm::vec3(0.0f, 0.0f, 0.0f);//旋转中心到几何中心的便宜量
	glm::mat4 rotate;
	glm::mat4 model;
	glm::mat4 defaultTrans = glm::mat4(1.0f);
	glm::mat4 defaultRotate = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);
	static bool detectCollision(Model& m1, Model& m2);
	static void collisionWithNoShatter(Model& m1, Model& m2);
	void beClicked(glm::vec3 origin, glm::vec3 position, glm::vec3 dir);
	void beReleased();
	glm::vec3 liftPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lastClickVector = glm::vec3(0.0f, 0.0f, 0.0f);
	bool isClicked = false;
	bool isVisible = true;
	float deltaTime = 1.0f / 60.0f;
	vector<Texture> wallTex;
private:
	const glm::vec4 baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 collideBoxColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f); //速度
	glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f); //加速度
	glm::vec3 angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f); //转动角速度
	glm::vec3 angularAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);//转动角加速度
	std::vector<Mesh> meshes;
	Mesh collideBox;
	std::string directory;
	Assimp::Importer importer;
	std::vector<Texture> textures_loaded;
	GLuint meshNum = 0;
	glm::vec3 minVec = glm::vec3(INT32_MAX - 1, INT32_MAX - 1, INT32_MAX - 1);
	glm::vec3 maxVec = glm::vec3(INT32_MIN + 1, INT32_MIN + 1, INT32_MIN + 1);
	glm::vec3 maxLimit = glm::vec3(100.0f, 100.0f, 100.0f);
	glm::vec3 minLimit = glm::vec3(-100.0f, -100.0f, -100.0f);
	std::vector<ParticleGenerator> trailingParticles;
	std::vector<ParticleGenerator> collisionParticles;
	std::vector<ParticleGenerator> destroyParticles;
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	void processCollideBox();
	void updateRotate();
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
};

