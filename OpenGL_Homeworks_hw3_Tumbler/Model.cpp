#include "Model.h"
#include <exception>

Model::Model(const Model& model)
{
	this->useGravity = model.useGravity;
	this->useDecrease = model.useDecrease;
	this->useGround = model.useGround;
	this->vDecreaseCo = model.vDecreaseCo;
	this->aDecreaseCo = model.aDecreaseCo;
	this->vMin = model.vMin;
	this->aMin = model.aMin;
	this->bounciness = model.bounciness;
	this->inertia = model.inertia;
	for (int i = 0; i < 3; i++)
		this->extents[i] = model.extents[i];
	this->mass = model.mass;
	this->gravityPosition = model.gravityPosition;
	this->model = glm::mat4(1.0f);
	this->rotate = glm::mat4(1.0f);
	this->scale = model.scale;
	this->deltaTime = model.deltaTime;
	for (int i = 0; i < model.meshes.size(); i++)
		this->meshes.push_back(model.meshes[i]);
	this->collideBox = model.collideBox;
	this->directory = model.directory;
	for (int i = 0; i < model.textures_loaded.size(); i++)
		this->textures_loaded.push_back(model.textures_loaded[i]);
	for (int i = 0; i < model.wallTex.size(); i++)
		this->wallTex.push_back(model.wallTex[i]);
	this->meshNum = model.meshNum;
	this->minVec = model.minVec;
	this->maxVec = model.maxVec;
	this->center = model.center;
	this->rotateCenter = model.rotateCenter;
	this->rotateOffset = model.rotateOffset;
	this->gravityOffset = model.gravityOffset;
	this->maxLimit = model.maxLimit;
	this->minLimit = model.minLimit;
	this->collideID = -1;
	this->isFixed = model.isFixed;
	this->fixedVelocity = model.fixedVelocity;
	this->fixedAngularVelocity = model.fixedAngularVelocity;
	this->useTextureChange = model.useTextureChange;
	this->useUpbound = model.useUpbound;
	this->upboundVelocity = model.upboundVelocity;
	this->upboundAngularVelocity = model.upboundAngularVelocity;
	this->useTrailingParticle = model.useTrailingParticle;
	this->useCollisionParticle = model.useCollisionParticle;
	this->useDestroyParticle = model.useDestroyParticle;
	this->isVisible = model.isVisible;
	for (int i = 0; i < model.trailingParticles.size(); i++)
		this->trailingParticles.push_back(model.trailingParticles[i]);
	for (int i = 0; i < model.collisionParticles.size(); i++)
		this->collisionParticles.push_back(model.collisionParticles[i]);
	for (int i = 0; i < model.destroyParticles.size(); i++)
		this->destroyParticles.push_back(model.destroyParticles[i]);
}

void vectorCoefficient(glm::vec3& v1, glm::vec3& v2) {
	v1.x *= v2.x, v1.y *= v2.y, v1.z *= v2.z;
}

glm::vec3 getVectorCoefficient(glm::vec3 v1, glm::vec3 v2) {
	return glm::vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}


void Model::Draw(Shader& shader, Camera& cam)
{
	//GLuint location = glGetUniformLocation(shader.ID, "baseColor");
	if (!isVisible) return;
	updateRotate();
	shader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate));
	for (unsigned i = 0; i < meshNum; i++) {
		meshes[i].Draw(shader, cam);
	}
} 

void Model::DrawCollideBox(Shader& shader, Camera& cam)
{
	if (!isVisible) return;
	updateRotate();
	shader.Activate();
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), collideBoxColor.x, collideBoxColor.y, collideBoxColor.z, collideBoxColor.w);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	collideBox.Draw(shader, cam);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 1);
}

void Model::DrawGravityPosition(Shader &shader, Camera& cam)
{
	if (!isVisible) return;
	std::vector<Vertex> v;
	std::vector<GLuint> i;
	std::vector<Texture> t;
	v.push_back(gravityPosition + glm::vec3(0.0f, 0.1f, 0.0f));
	v.push_back(gravityPosition + glm::vec3(-0.075f, -0.05f, -0.025f * sqrt(3.0f)));
	v.push_back(gravityPosition + glm::vec3(0.075f, -0.05f, -0.025 * sqrt(3.0f)));
	v.push_back(gravityPosition + glm::vec3(0.0f, -0.05f, 0.05 * sqrt(3.0f)));

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

	Mesh tmp(v, i, t);

	shader.Activate();
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), 0.0f, 1.0f, 0.0f, 1.0f);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	tmp.Draw(shader, cam);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 1);
}

void Model::DrawLiftPosition(Shader& shader, Camera& cam)
{
	if (!isVisible) return;
	std::vector<Vertex> v;
	std::vector<GLuint> i;
	std::vector<Texture> t;
	glm::vec3 liftPositionWorld = model * glm::vec4(liftPosition, 1.0f);
	v.push_back(liftPositionWorld + glm::vec3(0.0f, 0.1f, 0.0f));
	v.push_back(liftPositionWorld + glm::vec3(-0.075f, -0.05f, -0.025f * sqrt(3.0f)));
	v.push_back(liftPositionWorld + glm::vec3(0.075f, -0.05f, -0.025 * sqrt(3.0f)));
	v.push_back(liftPositionWorld + glm::vec3(0.0f, -0.05f, 0.05 * sqrt(3.0f)));

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

	Mesh tmp(v, i, t);

	shader.Activate();
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), 0.0f, 1.0f, 0.0f, 1.0f);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	tmp.Draw(shader, cam);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 1);
}

void Model::DrawRotateCenter(Shader& shader, Camera& cam)
{
	if (!isVisible) return;
	std::vector<Vertex> v;
	std::vector<GLuint> i;
	std::vector<Texture> t;
	glm::vec3 rotateCenterWorld = model * glm::vec4(rotateCenter, 1.0f);
	v.push_back(rotateCenterWorld + glm::vec3(0.0f, 0.1f, 0.0f));
	v.push_back(rotateCenterWorld + glm::vec3(-0.075f, -0.05f, -0.025f * sqrt(3.0f)));
	v.push_back(rotateCenterWorld + glm::vec3(0.075f, -0.05f, -0.025 * sqrt(3.0f)));
	v.push_back(rotateCenterWorld + glm::vec3(0.0f, -0.05f, 0.05 * sqrt(3.0f)));

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

	Mesh tmp(v, i, t);

	shader.Activate();
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), 0.0f, 1.0f, 0.0f, 1.0f);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	tmp.Draw(shader, cam);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 1);
}

void Model::DrawParticle(Shader& shader, Camera& cam)
{
	if(useTrailingParticle)
		for (int i = 0; i < trailingParticles.size(); i++)
			trailingParticles[i].Draw(shader, cam);
	if(useCollisionParticle)
		for (int i = 0; i < collisionParticles.size(); i++)
			collisionParticles[i].Draw(shader, cam);
	if(useDestroyParticle && !isVisible)
		for (int i = 0; i < destroyParticles.size(); i++)
			destroyParticles[i].Draw(shader, cam);
}

bool Model::detectCollision(Model& m1, Model& m2)
{
	if (!m1.isVisible || !m2.isVisible)
		return false;
	m1.updateRotate(), m2.updateRotate();
	glm::vec3 m1p1 = m1.model *  glm::vec4(m1.collideBox.vertices[4].position, 1.0f);
	glm::vec3 m1p2 = m1.model *  glm::vec4(m1.collideBox.vertices[3].position, 1.0f);

	glm::vec3 m2p1 = m2.model *  glm::vec4(m2.collideBox.vertices[4].position, 1.0f);
	glm::vec3 m2p2 = m2.model *  glm::vec4(m2.collideBox.vertices[3].position, 1.0f);

	glm::vec3 m1Center = (m1p1 + m1p2) * 0.5f;
	glm::vec3 m2Center = (m2p1 + m2p2) * 0.5f;

	//快速排除不可能发生碰撞的模型
	/*if (glm::length(m1Center - m2Center) > (glm::length(m1p1 - m1p2) + glm::length(m2p1 - m2p2)) * 0.5f) {
		return false;
	}*/

	glm::vec3 v = m1Center - m2Center;
	glm::vec3 m1x = m1.rotate * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 m1y = m1.rotate * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec3 m1z = m1.rotate * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec3 VA[3];
	VA[0] = m1x, VA[1] = m1y, VA[2] = m1z;
	glm::vec3 m2x = m2.rotate * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 m2y = m2.rotate * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec3 m2z = m2.rotate * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec3 VB[3];
	VB[0] = m2x, VB[1] = m2y, VB[2] = m2z;

	glm::vec3 T = glm::vec3(glm::dot(v, m1x), glm::dot(v, m1y), glm::dot(v, m1z));

	float R[3][3], FR[3][3];
	float ra, rb, t;
	glm::vec3 extens1, extens2;
	for (int i = 0; i < 3; i++) {
		for (int k = 0; k < 3; k++) {
			R[i][k] = glm::dot(VA[i], VB[k]);
			FR[i][k] = 1e-6f + std::abs(R[i][k]);
		}
	}

	extens1 = glm::vec4(m1.extents[0], m1.extents[1], m1.extents[2], 1.0f);
	extens2 = glm::vec4(m2.extents[0], m2.extents[1], m2.extents[2], 1.0f);

	for (int i = 0; i < 3; i++) {
		ra = extens1[i];
		rb = extens2[0] * FR[i][0] + extens2[1] * FR[i][1] + extens2[2] * FR[i][2];
		t = std::abs(T[i]);
		if (t > ra + rb) return false;
	}

	for (int i = 0; i < 3; i++) {
		ra = extens2[i];
		rb = extens1[0] * FR[0][i] + extens1[1] * FR[1][i] + extens1[2] * FR[2][i];
		t = std::abs(T[0] * R[0][i] + T[1] * R[1][i] + T[2] * R[2][i]);
		if (t > ra + rb) return false;
	}

	ra = extens1[1] * FR[2][0] + extens1[2] * FR[1][0];
	rb = extens2[1] * FR[0][2] + extens2[2] * FR[0][1];
	t = std::abs(T[2] * R[1][0] - T[1] * R[2][0]);
	if (t > ra + rb) return false;

	ra = extens1[1] * FR[2][1] + extens1[2] * FR[1][1];
	rb = extens2[1] * FR[0][2] + extens2[2] * FR[0][0];
	t = std::abs(T[2] * R[1][1] - T[1] * R[2][1]);
	if (t > ra + rb) return false;

	ra = extens1[1] * FR[2][2] + extens1[2] * FR[1][2];
	rb = extens2[0] * FR[0][1] + extens2[1] * FR[0][0];
	t = std::abs(T[2] * R[1][2] - T[1] * R[2][2]);
	if (t > ra + rb) return false;

	ra = extens1[0] * FR[2][0] + extens1[2] * FR[0][0];
	rb = extens2[1] * FR[1][2] + extens2[2] * FR[1][1];
	t = std::abs(T[0] * R[2][0] - T[2] * R[0][0]);
	if (t > ra + rb) return false;

	ra = extens1[0] * FR[2][1] + extens1[2] * FR[0][1];
	rb = extens2[0] * FR[1][2] + extens2[2] * FR[1][0];
	t = std::abs(T[0] * R[2][1] - T[2] * R[0][1]);
	if (t > ra + rb) return false;

	ra = extens1[0] * FR[2][2] + extens1[2] * FR[0][2];
	rb = extens2[0] * FR[1][1] + extens2[1] * FR[1][0];
	t = std::abs(T[0] * R[2][2] - T[2] * R[0][2]);
	if (t > ra + rb) return false;

	ra = extens1[0] * FR[1][0] + extens1[1] * FR[0][0];
	rb = extens2[1] * FR[2][2] + extens2[2] * FR[2][1];
	t = std::abs(T[1] * R[0][0] - T[0] * R[1][0]);
	if (t > ra + rb) return false;

	ra = extens1[0] * FR[1][1] + extens1[1] * FR[0][1];
	rb = extens2[0] * FR[2][2] + extens2[2] * FR[2][0];
	t = std::abs(T[1] * R[0][1] - T[0] * R[1][1]);
	if (t > ra + rb) return false;

	ra = extens1[0] * FR[1][2] + extens1[1] * FR[0][2];
	rb = extens2[0] * FR[2][1] + extens2[1] * FR[2][0];
	t = std::abs(T[1] * R[0][2] - T[0] * R[1][2]);
	if (t > ra + rb) return false;

	return true;

}

void Model::collisionWithNoShatter(Model& m1, Model& m2)
{
	glm::vec3 m1p1 = m1.model * glm::vec4(m1.collideBox.vertices[4].position, 1.0f);
	glm::vec3 m1p2 = m1.model * glm::vec4(m1.collideBox.vertices[3].position, 1.0f);

	glm::vec3 m2p1 = m2.model * glm::vec4(m2.collideBox.vertices[4].position, 1.0f);
	glm::vec3 m2p2 = m2.model * glm::vec4(m2.collideBox.vertices[3].position, 1.0f);

	glm::vec3 m1Center = (m1p1 + m1p2) * 0.5f;
	glm::vec3 m2Center = (m2p1 + m2p2) * 0.5f;

	glm::vec3 m1Diagonal = m1p2 - m1p1;
	glm::vec3 m2Diagonal = m2p2 - m2p1;

	float m1HalfSize = glm::length(glm::abs(m1Diagonal) * 0.5f);
	float m2HalfSize = glm::length(glm::abs(m2Diagonal) * 0.5f);
	glm::vec3 tmp = m2.velocity - m1.velocity;
	if (glm::length(tmp) < 0.01f)
		return;
	glm::vec3 direction1 = glm::normalize(tmp);
	glm::vec3 direction2 = -direction1;


	tmp = m2Center - m1Center;
	glm::vec3 collidePos;
	if (glm::length(tmp) < 0.01f)
		collidePos = (m1Center + m2Center) * 0.5f;
	else
		collidePos = m1Center + (m1HalfSize / (m1HalfSize + m2HalfSize)) * glm::normalize(m2Center - m1Center);

	float simulatedForce = m1.mass * m2.mass * (glm::length(m2.velocity - m1.velocity) + 0.1f);
	//std::cout << glm::length(m1.velocity) + 1 << std::endl;
	//std::cout << glm::length(m2.velocity) + 1 << std::endl;
	m1.useForce(m1.deltaTime, simulatedForce * direction1, collidePos);
	m2.useForce(m2.deltaTime, simulatedForce * direction2, collidePos);

	if (m2.useTextureChange)
		m2.changeTexture(m1.textures_loaded[0]);

	if (m1.useCollisionParticle)
		for (int i = 0; i < m1.collisionParticles.size(); i++)
			m1.collisionParticles[i].unfroze();
	if (m2.useCollisionParticle)
		for (int i = 0; i < m2.collisionParticles.size(); i++)
			m2.collisionParticles[i].unfroze();
}

void Model::processCollideBox() {
	std::vector<GLuint> indices = {
	0, 1, 2,
	1, 2, 3,
	0, 2, 4,
	2, 4, 6,
	0, 1, 5,
	0, 4, 5,
	2, 3, 6,
	3, 6, 7,
	3, 7, 5,
	1, 3, 5,
	4, 6, 7,
	4, 5, 7
	};
	std::vector<Texture> t;

	extents[0] = (maxVec[0] - minVec[0]) * 0.5f;
	extents[1] = (maxVec[1] - minVec[1]) * 0.5f;
	extents[2] = (maxVec[2] - minVec[2]) * 0.5f;


	std::vector<Vertex> box;
	box.push_back(Vertex(glm::vec3(minVec[0], maxVec[1], minVec[2])));
	box.push_back(Vertex(glm::vec3(minVec[0], maxVec[1], maxVec[2])));
	box.push_back(Vertex(glm::vec3(maxVec[0], maxVec[1], minVec[2])));
	box.push_back(Vertex(glm::vec3(maxVec[0], maxVec[1], maxVec[2])));
	box.push_back(Vertex(glm::vec3(minVec[0], minVec[1], minVec[2])));
	box.push_back(Vertex(glm::vec3(minVec[0], minVec[1], maxVec[2])));
	box.push_back(Vertex(glm::vec3(maxVec[0], minVec[1], minVec[2])));
	box.push_back(Vertex(glm::vec3(maxVec[0], minVec[1], maxVec[2])));


	collideBox = Mesh(box, indices, t);
}

void Model::updateRotate()
{
	//更新rotate矩阵，保证法向量正确；
	//原本的原点和基向量
	glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 axiX = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 axiY = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 axiZ = glm::vec3(0.0f, 0.0f, 1.0f);

	//得到当前模型的原点和基向量
	glm::vec3 newOrigin = model * glm::vec4(origin, 1.0f);
	glm::vec3 newAxiX = glm::normalize(glm::vec3(model * glm::vec4(axiX, 1.0f)) - newOrigin);
	glm::vec3 newAxiY = glm::normalize(glm::vec3(model * glm::vec4(axiY, 1.0f)) - newOrigin);
	glm::vec3 newAxiZ = glm::normalize(glm::vec3(model * glm::vec4(axiZ, 1.0f)) - newOrigin);

	glm::vec3 rotateAxisX = glm::cross(axiX, newAxiX);
	glm::vec3 rotateAxisY = glm::cross(axiY, newAxiY);
	glm::vec3 rotateAxisZ = glm::cross(axiZ, newAxiZ);

	rotate = glm::mat4(1.0f); // 初始化为单位矩阵
	rotate[0] = glm::vec4(newAxiX, 0.0f); // 第一列
	rotate[1] = glm::vec4(newAxiY, 0.0f); // 第二列
	rotate[2] = glm::vec4(newAxiZ, 0.0f); // 第三列
}

void Model::loadModel(std::string path)
{
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_ConvertToLeftHanded);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::LOADMODEL: " << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/') + 1);
	processNode(scene->mRootNode, scene);
	maxVec = scale * glm::vec4(maxVec, 1.0f);
	minVec = scale * glm::vec4(minVec, 1.0f);
	//默认，设置重心位置在中心点
	center = glm::vec3((maxVec[0] + minVec[0]) * 0.5f, (maxVec[1] + minVec[1]) * 0.5f, (maxVec[2] + minVec[2]) * 0.5f);
	rotateCenter = center + rotateOffset;
	gravityPosition = model * (glm::vec4(center + gravityOffset, 1.0f));
	processCollideBox();
}

/*
void Model::removeShake()
{
	glm::vec3 origin1 = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 origin2 = glm::vec3(1.0f, 0.0f, 0.0f);

	glm::vec3 translated1 = trans * rotate * glm::vec4(origin1, 1.0f);
	glm::vec3 translated2 = trans * rotate * glm::vec4(origin2, 1.0f);

	glm::vec3 vec = origin2 - origin1;
	glm::vec3 translatedVec = glm::normalize(translated2 - translated1);

	// 旋转轴和角度
	glm::vec3 axis = glm::cross(vec, translatedVec);
	if (glm::length(axis) == 0.0f)
		axis = glm::vec3(0.0f, 1.0f, 0.0f);
	float angle = glm::acos(glm::dot(vec, translatedVec));

	//rotate = glm::rotate(glm::mat4(1.0f), angle, axis);
	rotate = rotateArbitraryAxis(origin1, axis, angle);
	// 更新位移矩阵
	trans = glm::translate(glm::mat4(1.0f), translated1);
}
*/


void Model::processNode(aiNode* node, const aiScene* scene)
{

	for (unsigned i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh newMesh = processMesh(mesh, scene);
		meshNum++;
		meshes.push_back(newMesh);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	bool mNormalsExist = true;
	for (unsigned i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		
		glm::vec3 vector;
		//加载顶点位置的同时更新碰撞箱
		vector.x = mesh->mVertices[i].x;
		if (vector.x > maxVec[0])
			maxVec[0] = vector.x;
		if (vector.x < minVec[0])
			minVec[0] = vector.x;
		vector.y = mesh->mVertices[i].y;
		if (vector.y > maxVec[1])
			maxVec[1] = vector.y;
		if (vector.y < minVec[1])
			minVec[1] = vector.y;
		vector.z = mesh->mVertices[i].z;
		if (vector.z > maxVec[2])
			maxVec[2] = vector.z;
		if (vector.z < minVec[2])
			minVec[2] = vector.z;

		//注意，这一步需要乘scale
		vertex.position = scale * glm::vec4(vector, 1.0f);
		

		if (mesh->mNormals && mNormalsExist) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			if (glm::length(vector) > 0.0f) {
				vertex.normal = glm::normalize(vector);
			}
			else {
				vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
				mNormalsExist = false;
			}
		}
		else {
			vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
			mNormalsExist = false;
		 }

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texUV = vec;
		}

		vertices.push_back(vertex);
	}



	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	}

	
	if (!mNormalsExist) {
		// 按面计算法线
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			// assimp导入特性，每个面都是三角形
			if (face.mNumIndices == 3) {
				// 获取顶点索引
				unsigned int index0 = face.mIndices[0];
				unsigned int index1 = face.mIndices[1];
				unsigned int index2 = face.mIndices[2];

				// 获取顶点位置
				glm::vec3 v0 = vertices[index0].position;
				glm::vec3 v1 = vertices[index1].position;
				glm::vec3 v2 = vertices[index2].position;

				// 计算两个边的向量
				glm::vec3 edge1 = v1 - v0;
				glm::vec3 edge2 = v2 - v0;

				// 计算面的法线（通过两条边的叉乘得到）
				glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

				// 将法线添加到顶点数据中
				vertices[index0].normal += normal;
				vertices[index1].normal += normal;
				vertices[index2].normal += normal;
			}
		}

		// 归一化所有顶点法线
		for (unsigned int i = 0; i < vertices.size(); ++i) {
			vertices[i].normal = glm::normalize(vertices[i].normal);
		}
	}
	
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	unsigned unit = 0;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.C_Str(),(directory+std::string(str.C_Str())).c_str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture texture((directory + std::string(str.C_Str())).c_str(), typeName.c_str(), unit++);
			textures_loaded.push_back(texture);
			textures.push_back(texture);
		}
	}
	return textures;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene)
{
	std::vector<Texture> textures;
	unsigned unit = 0;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString name;
		mat->GetTexture(type, i, &name);
		bool skip = false;
		for (unsigned j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.C_Str(), (directory + std::string(name.C_Str())).c_str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture tex;
			auto filePath = Format("%s/%s", directory.c_str(), name.C_Str());
			const aiTexture *aitexture = scene->GetEmbeddedTexture(name.C_Str());
			if (aitexture != nullptr) {
				tex.type = typeName.c_str(); // Set the type of texture
				tex.path = filePath; // Set the path of the texture (can be useful for debugging)
				tex.unit = unit++;      // Set the texture unit

				// Generate OpenGL texture object
				glGenTextures(1, &tex.ID);
				glBindTexture(GL_TEXTURE_2D, tex.ID);

				// Set texture parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				
				// Load texture data into OpenGL
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, aitexture->mWidth, aitexture->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, aitexture->pcData);

				// Generate mipmaps
				glGenerateMipmap(GL_TEXTURE_2D);

				// Unbind texture
				glBindTexture(GL_TEXTURE_2D, 0);

				textures.push_back(tex);
				textures_loaded.push_back(tex);
			}
		}
	}
	return textures;
}

void Model::move(float deltaTime) {
		// 计算速度和角速度对位移和旋转矩阵的影响
		//std::cout << "g: " << gravityPosition.y << std::endl;
		//std::cout << "d: " << glm::vec3(0.0f, 0.0f, 0.0f).y << std::endl;
		// 更新位移和旋转矩阵
	if (isVisible) {
		velocity += acceleration * deltaTime; //上一帧总共计算下来的加速度，作用于这一帧
		angularVelocity += angularAcceleration * deltaTime;

		if (isFixed) {
			if (glm::length(velocity) != fixedVelocity)
				if (glm::length(velocity) > 0)
					velocity = glm::normalize(velocity) * fixedVelocity;
				else
					velocity = glm::vec3(0.0f, 1.0f, 0.0f) * fixedVelocity;
			if (glm::length(angularVelocity) != fixedAngularVelocity)
				if (glm::length(angularVelocity) > 0)
					angularVelocity = glm::normalize(angularVelocity) * fixedAngularVelocity;
				else
					angularVelocity = glm::vec3(0.0f, 1.0f, 0.0f) * fixedAngularVelocity;
		}
		else if (useUpbound) {
			if (glm::length(velocity) > upboundVelocity)
				velocity = glm::normalize(velocity) * upboundVelocity;
			if (glm::length(angularVelocity) > upboundAngularVelocity)
				angularVelocity = glm::normalize(angularVelocity) * upboundAngularVelocity;
		}

		glm::vec3 deltaPos = velocity * deltaTime;
		glm::mat4 deltaTrans = glm::translate(glm::mat4(1.0f), deltaPos);
		// 计算角速度对旋转矩阵的影响
		//glm::vec3 rotationAxis = glm::length(angularVelocity) > 0 ? glm::normalize(angularVelocity) : center;
		//glm::mat4 deltaRotate = glm::rotate(glm::mat4(1.0f), glm::length(angularVelocity) * deltaTime, rotationAxis);
		glm::mat4 deltaRotate = rotateArbitraryAxis(model * glm::vec4(rotateCenter, 1.0f), glm::normalize(angularVelocity), glm::length(angularVelocity) * deltaTime);
		//rotate = deltaRotate * rotate;
		model = deltaTrans * deltaRotate * model;
		// 更新重心位置（位移影响）
		gravityPosition = model * (glm::vec4(center + gravityOffset, 1.0f));


		//float groundLevel = (defaultTrans * defaultRotate * (glm::vec4(center + gravityOffset, 1.0f))).y;
		//bool useDecrease2 = velocity.y > 0 || gravityPosition.y < 2 * 0.5 * gravityCoefficient * deltaTime * deltaTime + groundLevel;
		float rotateRadius = (defaultTrans * defaultRotate * (glm::vec4(rotateCenter, 1.0f))).y;
		glm::vec3 rotateCenterWorld = model * glm::vec4(rotateCenter, 1.0f);
		bool useDecrease2 = velocity.y > 0 || rotateCenterWorld.y < gravityCoefficient* deltaTime* deltaTime + rotateRadius;

		if (useDecrease) {
			if (useGravity && useDecrease2)
				velocity.y *= vDecreaseCo;
			velocity.x *= vDecreaseCo;
			velocity.z *= vDecreaseCo;
			angularVelocity *= aDecreaseCo;
		}

		if (glm::length(velocity) < vMin && useDecrease2 && !isClicked) {
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		if (glm::length(angularVelocity) < aMin && !isClicked) {
			angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		if (useGround) {
			//和墙壁碰撞
			glm::vec3 velocityCoefficient = glm::vec3(1.0f, 1.0f, 1.0f);

			if (rotateCenterWorld.x >= maxLimit.x - rotateRadius || rotateCenterWorld.x <= minLimit.x + rotateRadius) {
				velocityCoefficient.x = -velocityCoefficient.x;
			}

			if (rotateCenterWorld.z >= maxLimit.z - rotateRadius || rotateCenterWorld.z <= minLimit.z + rotateRadius) {
				velocityCoefficient.z = -velocityCoefficient.z;
			}

			if (rotateCenterWorld.y >= maxLimit.y - rotateRadius || rotateCenterWorld.y <= minLimit.y + rotateRadius) {
				velocityCoefficient.y = -velocityCoefficient.y;
			}
			hitWall(velocityCoefficient);
		}

		this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
		this->angularAcceleration = glm::vec3(0.0f, 0.0f, 0.0f); //这一帧结束

		if (useTrailingParticle)
			for (GLuint i = 0; i < trailingParticles.size(); i++)
				trailingParticles[i].move(deltaTime, getCenterWorld(), velocity, 50, rotateRadius);

		if (useCollisionParticle)
			for (GLuint i = 0; i < collisionParticles.size(); i++)
				collisionParticles[i].move(deltaTime, getCenterWorld(), velocity, collisionParticles[i].getAmount() / 3.0f, rotateRadius);

		// 如果有重力，而且不会发生抖动，运用重力
		if (!isClicked)
			useForce(deltaTime, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)); //下一帧的开始，先运用重力
	}
	else {
		float rotateRadius = (defaultTrans * defaultRotate * (glm::vec4(rotateCenter, 1.0f))).y;
		if (useDestroyParticle && !isVisible)
			for (GLuint i = 0; i < destroyParticles.size(); i++)
				destroyParticles[i].move(deltaTime, getCenterWorld(), velocity, 50, rotateRadius);
	}
}

glm::mat4 Model::rotateArbitraryAxis(glm::vec3 v1, glm::vec3 dir, float theta)
{
	if (theta == 0.0f || glm::length(dir) == 0.0f)
		return glm::mat4(1.0f);
	float a = v1.x, b = v1.y, c = v1.z;
	dir = glm::normalize(dir);
	float u = dir.x, v = dir.y, w = dir.z;

	float uu = u * u;
	float uv = u * v;
	float uw = u * w;
	float vv = v * v;
	float vw = v * w;
	float ww = w * w;
	float au = a * u;
	float av = a * v;
	float aw = a * w;
	float bu = b * u;
	float bv = b * v;
	float bw = b * w;
	float cu = c * u;
	float cv = c * v;
	float cw = c * w;

	float cosTheta = cosf(theta), sinTheta = sinf(theta);
	glm::mat4 pOut;
	pOut[0][0] = uu + (vv + ww) * cosTheta;
	pOut[0][1] = uv * (1 - cosTheta) + w * sinTheta;
	pOut[0][2] = uw * (1 - cosTheta) - v * sinTheta;
	pOut[0][3] = 0;

	pOut[1][0] = uv * (1 - cosTheta) - w * sinTheta;
	pOut[1][1] = vv + (uu + ww) * cosTheta;
	pOut[1][2] = vw * (1 - cosTheta) + u * sinTheta;
	pOut[1][3] = 0;

	pOut[2][0] = uw * (1 - cosTheta) + v * sinTheta;
	pOut[2][1] = vw * (1 - cosTheta) - u * sinTheta;
	pOut[2][2] = ww + (uu + vv) * cosTheta;
	pOut[2][3] = 0;

	pOut[3][0] = (a * (vv + ww) - u * (bv + cw)) * (1 - cosTheta) + (bw - cv) * sinTheta;
	pOut[3][1] = (b * (uu + ww) - v * (au + cw)) * (1 - cosTheta) + (cu - aw) * sinTheta;
	pOut[3][2] = (c * (uu + vv) - w * (au + bv)) * (1 - cosTheta) + (av - bu) * sinTheta;
	pOut[3][3] = 1;
	return pOut;
}

void Model::useForce(float deltaTime, glm::vec3 force, glm::vec3 position)
{
	//刚体受力计算函数，包含vec3 force：力矢量， vec3 position: 作用点
	// vec3 gravityPosition：重心位置, 
	// float mass: 质量,
	// float inertia: 转动惯量
	// vec3 acceleration: 加速度
	// vec3 angularAcceleration: 角加速度
	//this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	//this->angularAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 rotateWorld = model * glm::vec4(rotateCenter, 1.0f);
	gravityPosition = model * glm::vec4(center + gravityOffset, 1.0f);

	if (glm::length(force) < 0.01f) {
		if (useGravity) {
			this->acceleration += glm::vec3(0.0f, -gravityCoefficient, 0.0f);
		}
		if (useGravity && useGround && rotateWorld.y <= gravityCoefficient * deltaTime * deltaTime + (defaultTrans * defaultRotate * (glm::vec4(rotateCenter, 1.0f))).y) {
			this->acceleration -= glm::vec3(0.0f, -gravityCoefficient, 0.0f);
			//计算转动中心到重心的矢量
			glm::vec3 rotateToGravity = gravityPosition - rotateWorld;
			//力矩，用叉乘算出
			glm::vec3 torque = glm::cross(rotateToGravity, glm::vec3(0.0f, -mass * gravityCoefficient, 0.0f));
			glm::vec3 tmp = glm::cross(glm::vec3(0.0f, -1.0f, 0.0f), angularVelocity);
			if (glm::length(tmp) > 0) {
				glm::vec3 transVector = glm::length(angularVelocity) * deltaTime * (defaultTrans * defaultRotate * (glm::vec4(rotateCenter, 1.0f))).y * glm::normalize(tmp);
				transVector.y = 0.0f;
				glm::mat4 deltaTrans = glm::translate(glm::mat4(1.0f), transVector);
				model = deltaTrans * model;
			}
			if (rotateWorld.y < gravityPosition.y) {
				angularVelocity *= 0.8f;
			}
			angularVelocity += torque * deltaTime / inertia;
				
		}
		return;
	}
	//重心到力的作用点的距离（这里简化，从作用点指向旋转中心）

	glm::vec3 distance = rotateWorld - position;
	if (glm::length(distance) == 0 || glm::length(glm::abs(glm::normalize(distance)) - glm::abs(glm::normalize(force))) < 0.1f) {
		//假如力到重心的偏移量小于0.1f，直接当做质点受力处理
		//设置角加速度为0
		//this->angularAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
		this->acceleration += force / mass;
		return;
	}
	// 力矩
	glm::vec3 torque = glm::cross(distance, force);

	//角加速度
	// 将力分成产生加速度和产生力的两部分
	// 加速度方向是力在质心处的投影
	glm::vec3 acceleration = glm::dot(force, distance) / glm::dot(distance, distance) * glm::normalize(distance);
	// 扭矩方向是力垂直于距离的分量
	glm::vec3 torqueDirection = force - acceleration;
	glm::vec3 angularAcceleration = glm::cross(torqueDirection, distance);

	// 更新模型的加速度和角加速度
	this->acceleration += acceleration / mass;
	this->angularAcceleration += angularAcceleration / inertia;
}

void Model::fixVelocityAndAngularVelocity(float fixVelocity, float fixAngularVelocity)
{
	fixedVelocity = fixVelocity;
	fixedAngularVelocity = fixAngularVelocity;
	isFixed = true;
}

void Model::setUpBound(float upboundVelocity, float upboundAngularVelocity)
{
	this->upboundVelocity = upboundVelocity;
	this->upboundAngularVelocity = upboundAngularVelocity;
}

void Model::UnfixVelocityAndAngularVelocity()
{
	isFixed = false;
}

void Model::AddTrailingParticle(const ParticleGenerator &p)
{
	trailingParticles.push_back(p);
}

void Model::AddCollisionParticle(const ParticleGenerator& p)
{
	collisionParticles.push_back(p);
}

void Model::AddDestroyParticle(const ParticleGenerator& p)
{
	destroyParticles.push_back(p);
}

void Model::beDestroyed()
{
	if (useDestroyParticle && destroyParticles.size() > 0) {
		for (int i = 0; i < destroyParticles.size(); i++)
			destroyParticles[i].unfroze();
	}	
	this->isVisible = false;
}

void Model::hitWall(glm::vec3 velocityCoefficient)
{

	if (velocityCoefficient.x >0 && velocityCoefficient.y > 0 && velocityCoefficient.z > 0)
		return;

	float rotateRadius = (defaultTrans * defaultRotate * (glm::vec4(rotateCenter, 1.0f))).y;

	if (useCollisionParticle) {
		for (GLuint i = 0; i < collisionParticles.size(); i++) {
			collisionParticles[i].unfroze();
		}
	}

	glm::vec3 rotateWorld = model * glm::vec4(rotateCenter, 1.0f);
	if (useTextureChange) {
		if (rotateWorld.x > maxLimit.x - rotateRadius) {
			changeTexture(wallTex[2]);
		}
		else if (rotateWorld.x <= minLimit.x + rotateRadius) {
			changeTexture(wallTex[3]);
		}
		else if (rotateWorld.y > maxLimit.y - rotateRadius) {
			changeTexture(wallTex[0]);
		}
		else if (rotateWorld.y <= minLimit.y + rotateRadius) {
			changeTexture(wallTex[1]);
		}
		else if (rotateWorld.z > maxLimit.z - rotateRadius) {
			changeTexture(wallTex[5]);
		}
		else if (rotateWorld.z <= minLimit.z + rotateRadius) {
			changeTexture(wallTex[4]);
		}
	}

	glm::vec3 in1 = glm::vec3(maxLimit.x - rotateRadius, maxLimit.y - rotateRadius, maxLimit.z - rotateRadius);
	glm::vec3 in2 = glm::vec3(minLimit.x + rotateRadius, minLimit.y + rotateRadius, minLimit.z + rotateRadius);

	glm::vec3 matrixCoefficient = (glm::vec3(1.0f, 1.0f, 1.0f) - velocityCoefficient) * 0.5f;
	vectorCoefficient(rotateWorld, matrixCoefficient), vectorCoefficient(in1, matrixCoefficient), vectorCoefficient(in2, matrixCoefficient);
	glm::vec3 in1ToWorld = in1 - rotateWorld, in2ToWorld = in2 - rotateWorld;
	glm::vec3 moveVec;
	moveVec.x = abs(in1ToWorld.x) < abs(in2ToWorld.x) ? in1ToWorld.x : in2ToWorld.x;
	moveVec.y = abs(in1ToWorld.y) < abs(in2ToWorld.y) ? in1ToWorld.y : in2ToWorld.y;
	moveVec.z = abs(in1ToWorld.z) < abs(in2ToWorld.z) ? in1ToWorld.z : in2ToWorld.z;
	glm::mat4 deltaModel = glm::translate(glm::mat4(1.0f), moveVec);

	//撞墙，角速度变向
	if (velocityCoefficient.x < 0 || velocityCoefficient.z < 0)
		angularVelocity = -angularVelocity;

	//下落和地板碰撞，应用弹性系数
	if (velocity.y < 0 && velocityCoefficient.y < 0) { 
		velocityCoefficient.y = 1.0f;
		bool b1 = rotateWorld.y > 0.5 * gravityCoefficient * deltaTime * deltaTime + rotateRadius;
		if (b1 || glm::length(velocity) > 3.5f) { //大于1m的坠落
			//和地板碰撞了	此时不对速度做处理
			velocity.y = -velocity.y * bounciness;
		}
		else if (glm::length(velocity) > 3.5f) {
			//为了减少近地面的抽搐抖动
			velocity.y = -velocity.y * 0.7f * bounciness;
		}
		else {
			velocity.y = -0.5f * velocity.y * bounciness;
		}
	}

	vectorCoefficient(velocity, velocityCoefficient);

	model = deltaModel * model;
}

glm::vec3 Model::getCenterWorld()
{
	return model * glm::vec4(center, 1.0f);
}

float Model::getCenterDistance(glm::vec3 point)
{
	glm::vec3 centerWorld = getCenterWorld();
	return glm::length(centerWorld - point);
}

void Model::selfBalance()
{
	
}


void Model::setVelocity(float x, float y, float z)
{
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}

void Model::setVelocity(float x, float y, float z, float v)
{
	velocity.x = x*v;
	velocity.y = y*v;
	velocity.z = z*v;
}

void Model::setAcceleration(float x, float y, float z)
{
	acceleration.x = x;
	acceleration.y = y;
	acceleration.z = z;
}

void Model::setAcceleration(float x, float y, float z, float v)
{
	acceleration.x = x*v;
	acceleration.y = y*v;
	acceleration.z = z*v;
}

void Model::setAngularVelocity(float x, float y, float z)
{
	angularVelocity.x = x;
	angularVelocity.y = y;
	angularVelocity.z = z;
}

void Model::setAngularVelocity(float x, float y, float z, float v)
{
	angularVelocity.x = x*v;
	angularVelocity.y = y*v;
	angularVelocity.z = z*v;
}

void Model::setAngularAcceleration(float x, float y, float z)
{
	angularAcceleration.x = x;
	angularAcceleration.y = y;
	angularAcceleration.z = z;
}

void Model::setAngularAcceleration(float x, float y, float z, float v)
{
	angularAcceleration.x = x*v;
	angularAcceleration.y = y*v;
	angularAcceleration.z = z*v;
}

void Model::setScale(float x, float y, float z)
{
	this->scale = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void Model::setPosition(float x, float y, float z) {
	this->model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	gravityPosition =	model * glm::vec4(center + gravityOffset, 1.0f);
}

void Model::setRotate(glm::vec3 axis, float angle)
{
	this->rotate = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
	//gravityPosition = trans * rotate * glm::vec4(center + gravityOffset, 1.0f);
}

void Model::setRotateOffset(glm::vec3 offset)
{
	rotateOffset = offset;
	if (!isClicked) rotateCenter = center + offset;
}

void Model::setDefaultPosition(float x, float y, float z)
{
	this->defaultTrans = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));;
}

void Model::setDefaultRotate(glm::vec3 axis, float angle)
{
	this->defaultRotate = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}

void Model::setMaxLimit(float x, float y, float z)
{
	this->maxLimit = glm::vec3(x, y, z);
}

void Model::setMinLimit(float x, float y, float z)
{
	this->minLimit = glm::vec3(x, y, z);
}

void Model::changeTexture(Texture texture)
{
	//在本程序中，每个物体只有一个texture，不考虑多个网格组成的面。
	this->textures_loaded.clear();
	textures_loaded.push_back(texture);
	for (int i = 0; i < this->meshes.size(); i++) {
		/*this->meshes[i].textures.clear();
		this->meshes[i].textures.push_back(texture);*/
		this->meshes[i] = Mesh(this->meshes[i].vertices, this->meshes[i].indices, this->textures_loaded);
	}
}

void Model::setGravityOffset(float x, float y, float z)
{
	this->gravityOffset = glm::vec3(x, y, z);
}


void Model::revisePosition()
{
	this->gravityPosition = defaultTrans * defaultRotate * glm::vec4(center + gravityOffset, 1.0f);
	this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	this->angularAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	this->model = defaultTrans * defaultRotate;
	this->rotate = defaultRotate;
	this->rotateCenter = center + rotateOffset;
}


bool rayTriangleIntersect(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 origin, glm::vec3 dir, glm::vec3& intersectPosition) {
	glm::vec3 S = origin - v0;
	glm::vec3 E1 = v1 - v0;
	glm::vec3 E2 = v2 - v0;
	glm::vec3 S1 = glm::cross(dir, E2);
	glm::vec3 S2 = glm::cross(S, E1);

	float S1E1 = glm::dot(S1, E1);
	float t = glm::dot(S2, E2) / S1E1;
	float b1 = glm::dot(S1, S) / S1E1;
	float b2 = glm::dot(S2, dir) / S1E1;

	if (t >= 0.f && b1 >= 0.f && b2 >= 0.f && (1 - b1 - b2) >= 0.f) {
		intersectPosition = origin + t * dir;
		return true;
	}

	return false;
}

bool Model::rayCast(glm::vec3 origin, glm::vec3 direction)
{
	bool re = false;
	//假如已经是被点击状态，不用继续计算，只需要将其移动到以摄像机为球面的圆上的对应位置即可。
	if (isClicked) {
		glm::vec3 clickPosition = origin + direction * glm::length(lastClickVector);
		beClicked(origin, clickPosition, direction);
		return true;
	}

	//特判0， 射线原点在长方体的包围盒之内
	//长方体的碰撞盒此时需要通过trans * rotate * 碰撞盒坐标算出，一次计算八个点很麻烦，反向思考
	//只要射线原点左乘 rotate逆 * trans逆，再和原本的正交碰撞盒进行判断即可。
	glm::vec3 inverseOrigin = glm::inverse(model) * glm::vec4(origin, 1.0f);
	//一个点在正交长方体内的条件为: xMin <= x <= yMax，其余的一一对应
	re = (minVec[0] <= origin.x && minVec[1] <= origin.y && minVec[2] <= origin.z) && (origin.x <= maxVec[0] && origin.y <= maxVec[1] && origin.z <= maxVec[2]);
	if (re) {
		beClicked(origin, origin, direction);
		return true;
	}
	//特判1， 包围盒中心到射线的距离长于该长方体包围盒的体对角线，如果这样，不可能相交，直接退出
	glm::vec3 p1 = model * glm::vec4(collideBox.vertices[4].position, 1.0f);
	glm::vec3 p2 = model * glm::vec4(collideBox.vertices[3].position, 1.0f);
	glm::vec3 center = (p1 + p2) * 0.5f;
	//体对角线长度
	float maxLength = glm::length(p1 - p2) * 0.5f;
	glm::vec3 originToCenter = center - origin;
	//计算获得投影长投影点
	float d1 = glm::dot(originToCenter, direction) / glm::length(direction);
	glm::vec3 p2Proj = origin + d1 * direction;
	//获得投影点到长方体中心点之间的距离
	float distance = glm::distance(p2Proj, center);
	if (distance > maxLength)
		return false;
	//普通判断，根据碰撞箱顶点坐标来判定射线是否和箱子相交
	//找到距离射线原点的路径最短点

	GLuint index[4];
	index[0] = 0;
	float minDistance = glm::distance(origin, glm::vec3(model * glm::vec4(collideBox.vertices[0].position, 1.0f)));
	for (GLuint i = 1; i < 8; i++) {
		float tmpDis = glm::distance(origin, glm::vec3(model * glm::vec4(collideBox.vertices[i].position, 1.0f)));
		if (tmpDis < minDistance) {
			minDistance = tmpDis;
			index[0] = i;
		}
	}

	glm::vec3 position0 = collideBox.vertices[index[0]].position;
	for (int i = 1; i < 4; i++) {
		index[i] = index[0];//初始化
	}
	//获取从最短点index0开始向外延伸的三点index，注意，在原本的碰撞箱中，和这个点邻接的三个点在坐标三维上仅仅只有一个维度不同。
	for (int i = 0; i < 8; i++) {
		if (i == index[0]) continue;
		glm::vec3 tmpPosition = collideBox.vertices[i].position;
		int isClose = (tmpPosition.x == position0.x) + (tmpPosition.y == position0.y) + (tmpPosition.z == position0.z);
		if (isClose != 2) continue;
		if (index[1] == index[0]) {
			index[1] = i;
		}
		else if (index[2] == index[0]) {
			index[2] = i;
		}
		else if (index[3] == index[0]) {
			index[3] = i;
			break;
		}
	}


	//Moller-Trumbore
	glm::vec3 points[4];
	glm::vec3 intersectPoint;
	for (int i = 0; i < 4; i++) {
		points[i] = model * glm::vec4(collideBox.vertices[index[i]].position, 1.0f);
	}

	if (re = rayTriangleIntersect(points[0], points[1], points[2], origin, direction, intersectPoint)) {}
	else 	if (re = rayTriangleIntersect(points[0], points[1], points[3], origin, direction, intersectPoint)) {}
	else 	if (re = rayTriangleIntersect(points[0], points[2], points[3], origin, direction, intersectPoint)) {}
	else 	if (re = rayTriangleIntersect(points[2] + points[1] - points[0], points[1], points[2], origin, direction, intersectPoint)) {}
	else 	if (re = rayTriangleIntersect(points[3] + points[1] - points[0], points[1], points[3], origin, direction, intersectPoint)) {}
	else 	if (re = rayTriangleIntersect(points[2] + points[3] - points[0], points[2], points[3], origin, direction, intersectPoint)) {}

	if (re) {
		beClicked(origin, intersectPoint, direction);
	}

	return re;
}

void Model::beClicked(glm::vec3 origin, glm::vec3 position, glm::vec3 dir) {
	if (position.y < gravityPosition.y && !isClicked) {
		//在未被提起状态时，点击模型重心以下的位置给予动量，并直接放松。
		dir = glm::normalize(dir);;
		dir.y = 0.0f;
		this->velocity +=  dir;
		beReleased();
	}
	else {
		//点击部位高于重心，提起模型
		//首先将模型进入被提起状态
		if (position.y <= gravityPosition.y)
			return;
		if (!isClicked) {
			collideBoxColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			isClicked = true;
			//lastClickPosition = position;
			lastClickVector = position - origin;
			liftPosition = glm::inverse(model) * glm::vec4(position, 1.0f); //坐标系转换，保存在原正交坐标系当中的提起点位置
			angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
			//将旋转中心放到提起点
			rotateCenter = liftPosition;
			//rotateCenter = trans * rotate * glm::vec4(liftPosition, 1.0f);
		}

		//lastClickPosition = position;
		//lastClickVector = position - origin;
		//将其速度记为0
		velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		//更新提起点的世界坐标
		glm::vec3 liftPositionWorld =   model *  glm::vec4(liftPosition, 1.0f);
		//更新重心位置
		gravityPosition = model * (glm::vec4(center + gravityOffset, 1.0f));
		//2. 计算由于提起对模型产生的力矩
		//r为距离矢量，F为向下的重力
		glm::vec3 deltaDis = position - liftPositionWorld;
		glm::mat4 deltaTrans = glm::translate(glm::mat4(1.0f), deltaDis);
		//acceleration += deltaDis / deltaTime;
		// 
		//1. 计算提起点到重心的距离矢量
		glm::vec3 liftToGravityCenter = gravityPosition - liftPositionWorld;
		glm::vec3 torque = glm::cross(liftToGravityCenter, glm::vec3(0.0f, -mass * gravityCoefficient, 0.0f));
		//此时模型视为质点，视其转动惯量为mr*r
		glm::vec3 deltaAngularAcc = torque / (mass * glm::length(liftToGravityCenter) * glm::length(liftToGravityCenter));//计算出绕着提起点在一个deltaTime时间之内的角加速度
		if (gravityPosition.y > liftPositionWorld.y)
			deltaAngularAcc *= 10;
		angularAcceleration += deltaAngularAcc;

		model = deltaTrans * model;
	}
}

void Model::beReleased() {
	if (isClicked) {
		angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
		velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		collideBoxColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		rotateCenter = center + rotateOffset;
		isClicked = false;
		//清除因为不同的旋转顶点会对两个矩阵造成的不良影响。
		//removeShake();
	}
}

