#include "Particle.h"

ParticleGenerator::ParticleGenerator(GLuint amount)
{
	this->amount = amount;
	std::vector<Vertex> v;
	std::vector<GLuint> i;
	std::vector<Texture> t;
	v.push_back(glm::vec3(0.0f, 0.1f, 0.0f));
	v.push_back(glm::vec3(-0.075f, -0.05f, -0.025f * sqrt(3.0f)));
	v.push_back(glm::vec3(0.075f, -0.05f, -0.025 * sqrt(3.0f)));
	v.push_back(glm::vec3(0.0f, -0.05f, 0.05 * sqrt(3.0f)));

	i.push_back(0);
	i.push_back(1);
	i.push_back(2);
	i.push_back(0);
	i.push_back(2);
	i.push_back(3);
	i.push_back(0);
	i.push_back(3);
	i.push_back(1);
	i.push_back(1);
	i.push_back(2);
	i.push_back(3);

	this-> particleMesh = Mesh(v, i, t);
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

ParticleGenerator::ParticleGenerator(const ParticleGenerator& p)
{
	this->amount = p.amount;
	this->bIncreaseAmount = p.bIncreaseAmount;
	this->rIncreaseAmount = p.rIncreaseAmount;
	this->gIncreaseAmount = p.gIncreaseAmount;
	this->baseColor = p.baseColor;
	this->lifeTime = p.lifeTime;
	this->pt = p.pt;
	std::vector<Vertex> v;
	std::vector<GLuint> i;
	std::vector<Texture> t;
	v.push_back(glm::vec3(0.0f, 0.1f, 0.0f));
	v.push_back(glm::vec3(-0.075f, -0.05f, -0.025f * sqrt(3.0f)));
	v.push_back(glm::vec3(0.075f, -0.05f, -0.025 * sqrt(3.0f)));
	v.push_back(glm::vec3(0.0f, -0.05f, 0.05 * sqrt(3.0f)));

	i.push_back(0);
	i.push_back(1);
	i.push_back(2);
	i.push_back(0);
	i.push_back(2);
	i.push_back(3);
	i.push_back(0);
	i.push_back(3);
	i.push_back(1);
	i.push_back(1);
	i.push_back(2);
	i.push_back(3);

	this->particleMesh = Mesh(v, i, t);
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

void ParticleGenerator::move(float deltaTime, glm::vec3 objectCenter, glm::vec3 objectVelocity, GLuint newParticleAmount, float radius) {
	for (GLuint i = 0; i < newParticleAmount; i++) {
		int unusedParticle = this->getFirstUnusedParticle();
		if (this->particles[unusedParticle].frozen)
			break;
		this->respawnParticle(this->particles[unusedParticle], objectCenter, objectVelocity, radius);
	}
	for (GLuint i = 0; i < this->amount; i++) {
		Particle& p = this->particles[i];
		//if (glm::dot(p.velocity, objectVelocity) < 0)
			//p.lifetime = 0.0f;
		if (p.lifetime > 0.0f) {		
			p.lifetime -= deltaTime;
			p.position += p.velocity * deltaTime;
			p.color.r += deltaTime * rIncreaseAmount;
			p.color.r = min(p.color.r, 1.0f);
			p.color.r = max(p.color.r, 0.0f);
			p.color.g += deltaTime * gIncreaseAmount;
			p.color.g = max(p.color.g, 0.0f);
			p.color.g = min(p.color.g, 1.0f);
			p.color.b += deltaTime * bIncreaseAmount;
			p.color.b = max(p.color.b, 0.0f);
			p.color.b = min(p.color.b, 1.0f);
		}
	}
}

void ParticleGenerator::Draw(Shader& shader, Camera& cam) {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	shader.Activate();
	//glDisable(GL_DEPTH_TEST);
	for (Particle &p : this->particles) {
		if (p.lifetime > 0.0f) {
			glUniform3f(glGetUniformLocation(shader.ID, "offset"),  p.position.x, p.position.y, p.position.z);
			glUniform4f(glGetUniformLocation(shader.ID, "color"), p.color.r, p.color.g, p.color.b, p.color.a);
			particleMesh.Draw(shader, cam);
		}
	}
	//glEnable(GL_DEPTH_TEST);
}

GLuint ParticleGenerator::getFirstUnusedParticle() {
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].lifetime <= 0.0f && !this->particles[i].frozen) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].lifetime <= 0.0f && !this->particles[i].frozen) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}
void ParticleGenerator::respawnParticle(Particle& particle, glm::vec3 objectCenter, glm::vec3 objectVelocity, float radius) {
	if (pt == TRAILING) {
		particle.position = objectCenter + glm::normalize(glm::vec3(((rand() % 100) - 50) / 100.0f, ((rand() % 100) - 50) / 100.0f, ((rand() % 100) - 50) / 100.0f)) * (radius + 0.05f);
		glm::vec3 convexPoint = objectCenter - objectVelocity * 0.1f * lifeTime;
		particle.color = baseColor + glm::vec4(((rand() % 10) / 200.0f), ((rand() % 10) / 200.0f), ((rand() % 10) / 200.0f), 1.0f);
		for (int i = 0; i < 4; i++) {
			particle.color[i] = max(particle.color[i], 0.0f);
			particle.color[i] = min(particle.color[i], 1.0f);
		}
		particle.lifetime = lifeTime;
		//particle.velocity = objectVelocity * 0.5f;
		//particle.velocity = glm::length(objectVelocity) * 0.1f * glm::length(particle.position - convexPoint) / glm::length(objectCenter - convexPoint) * glm::normalize((convexPoint - particle.position));
		particle.velocity = glm::length(objectVelocity) * 0.1f * glm::normalize((convexPoint - particle.position));
	}
	else if (pt == SPREAD) {
		if (particle.frozen) //选到的是已经出现过的粒子。
			return;
		particle.position = objectCenter - glm::normalize(objectVelocity) * (radius);
		particle.color = baseColor;
		particle.lifetime = lifeTime;
		glm::vec3 randLocation = glm::normalize(glm::vec3(rand() % 1000 - 500.0f, rand() % 1000 - 500.0f, rand() % 1000 - 500.0f));
		glm::vec3 convex = randLocation * (radius + 0.3f) + objectCenter;
		particle.velocity = (glm::normalize(convex - particle.position) + glm::normalize(objectVelocity)) * 20.0f;
		particle.frozen = true;
	}
	else if (pt == EVAPORATE) {
		particle.position = objectCenter + glm::normalize(glm::vec3(((rand() % 100) - 50) / 100.0f, ((rand() % 100) - 50) / 100.0f, ((rand() % 100) - 50) / 100.0f)) * (radius + 0.05f);
		
		glm::vec3 convexPoint = particle.position + max(glm::length(objectVelocity), 10.0f) * 0.1f * lifeTime * glm::vec3(0.0f, 1.0f, 0.0f);
		particle.color = baseColor + glm::vec4(((rand() % 10) / 200.0f), ((rand() % 10) / 200.0f), ((rand() % 10) / 200.0f), 1.0f);
		for (int i = 0; i < 4; i++) {
			particle.color[i] = max(particle.color[i], 0.0f);
			particle.color[i] = min(particle.color[i], 1.0f);
		}
		particle.lifetime = lifeTime;
		//particle.velocity = objectVelocity * 0.5f;
		//particle.velocity = glm::length(objectVelocity) * 0.1f * glm::length(particle.position - convexPoint) / glm::length(objectCenter - convexPoint) * glm::normalize((convexPoint - particle.position));
		particle.velocity = max(glm::length(objectVelocity), 10.0f) * 0.1f * glm::normalize((convexPoint - particle.position));
		particle.frozen = true;
	}
}

void ParticleGenerator::setRGBIncrease(float r, float g, float b) {
	this->rIncreaseAmount = r;
	this->gIncreaseAmount = g;
	this->bIncreaseAmount = b;
}

void ParticleGenerator::setBaseColor(float r, float g, float b, float a) {
	this->baseColor = glm::vec4(r, g, b, a);
}

void ParticleGenerator::setLifeTime(float time) {
	this->lifeTime = time;
}

void ParticleGenerator::setPT(particleType t) {
	this->pt = t;
}

void ParticleGenerator::unfroze() {
	for (int i = 0; i < this->amount; i++)
		particles[i].frozen = false;
}

GLuint ParticleGenerator::getAmount()
{
	return this->amount;
}
