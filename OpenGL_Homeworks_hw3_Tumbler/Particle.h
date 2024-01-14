#pragma once
#include "Mesh.h"
#include "Camera.h"

using std::vector;
using std::max;
using std::min;

struct Particle
{
	glm::vec3 position, velocity;
	glm::vec4 color;
	bool frozen = false;
	float lifetime;
	Particle() {
		position = glm::vec3(0.0f);
		velocity = glm::vec3(0.0f);
		color = glm::vec4(0.0f);
		lifetime = 0.0f;
	}

	Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float life) {
		this->position = position;
		this->velocity = velocity;
		this->color = color;
		this->lifetime = life;
	}

	Particle(const Particle& p) {
		this->position = p.position;
		this->velocity = p.velocity;
		this->color = p.color;
		this->lifetime = p.lifetime;
	}
};

class ParticleGenerator {
public:
	static enum particleType
	{//表示这种粒子的产生方式有几种类型，比较常见的是拖尾（TRAILING），发散（SPREAD），蒸发（EVAPORATE）
		TRAILING, SPREAD, EVAPORATE
	};
	ParticleGenerator(GLuint amount = 10000);
	ParticleGenerator(const ParticleGenerator& p);
	void move(float deltaTime, glm::vec3 objectCenter, glm::vec3 objectVelocity, GLuint newParticleAmount, float radius);
	void Draw(Shader& shader, Camera& cam);
	void setRGBIncrease(float r, float g, float b);
	void setBaseColor(float r, float g, float b, float a = 1.0f);
	void setLifeTime(float time);
	void setPT(particleType t);
	void unfroze();
	GLuint getAmount();
private:
	GLuint lastUsedParticle = 0;
	particleType pt = TRAILING;
	vector<Particle> particles;
	GLuint amount;
	float lifeTime = 0.0f;
	glm::vec4 baseColor = glm::vec4(1.0f);
	GLuint getFirstUnusedParticle();
	void respawnParticle(Particle& particle, glm::vec3 objectCenter, glm::vec3 objectVelocity, float radius);
	Mesh particleMesh;
	float rIncreaseAmount = 0.0f, gIncreaseAmount = 0.0f, bIncreaseAmount = 0.0f;
};

