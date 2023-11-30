#include "Model.h"
#include <exception>

void Model::Draw(Shader& shader, Camera& cam)
{
	//GLuint location = glGetUniformLocation(shader.ID, "baseColor");
	if (splited) {
		shader.Activate();
		for (unsigned i = 0; i < meshNum; i++) {
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(trans[i+1] * rotate[i+1]));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate[i+1]));
			//glUniform4f(location, float(i)/meshes.size(), float(i) / meshes.size(), float(i)/meshes.size(), 0.7f);
			meshes[i].Draw(shader, cam);
		}
	}
	else {
		shader.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(trans[0] * rotate[0]));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate[0]));
		for (unsigned i = 0; i < meshNum; i++) {
			//glUniform4f(location, float(i)/meshes.size(), float(i) / meshes.size(), float(i)/meshes.size(), 0.7f);
			meshes[i].Draw(shader, cam);
		}
	}
}

void Model::DrawInnerMesh(Shader& shader, Camera& cam){
	shader.Activate();
	GLuint location = glGetUniformLocation(shader.ID, "baseColor");
	glUniform4f(location, 0.0f, 0.7f, 0.0f, 0.7f);
	if (splited) {
		shader.Activate();
		for (unsigned i = 0; i < meshNum; i++) {
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(trans[i+1] * rotate[i+1]));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate[i+1]));
			innerMeshes[i].Draw(shader, cam);
		}
	}
	else {
		shader.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(trans[0] * rotate[0]));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate[0]));
		for (unsigned i = 0; i < meshNum; i++) {
			innerMeshes[i].Draw(shader, cam);
		}
	}
	glUniform4f(location, baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
}

void Model::DrawCollideBox(Shader& shader, Camera& cam)
{

	if (splited) {
		/*
		shader.Activate();
		for (unsigned i = 1; i <= meshNum; i++) {
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(trans[i] * rotate[i]));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate[i]));
			collideBox[i].Draw(shader, cam);
		}
		*/
	}
	else {
		GLuint location = glGetUniformLocation(shader.ID, "baseColor");
		glUniform4f(location, 0.0f, 0.7f, 0.0f, 0.7f);
		shader.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(trans[0] * rotate[0]));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate[0]));
		collideBox[0].Draw(shader, cam);
		glUniform4f(location, baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
	}

}

bool Model::detectCollision(Model& m1, Model& m2)
{
	if (m1.splited || m2.splited)
		return false;
	glm::vec3 m1p1 = m1.trans[0] * m1.rotate[0] * m1.scale * glm::vec4(m1.collideBox[0].vertices[4].position, 1.0f);
	glm::vec3 m1p2 = m1.trans[0] * m1.rotate[0] * m1.scale * glm::vec4(m1.collideBox[0].vertices[3].position, 1.0f);

	glm::vec3 m2p1 = m2.trans[0] * m2.rotate[0] * m2.scale * glm::vec4(m2.collideBox[0].vertices[4].position, 1.0f);
	glm::vec3 m2p2 = m2.trans[0] * m2.rotate[0] * m2.scale * glm::vec4(m2.collideBox[0].vertices[3].position, 1.0f);

	glm::vec3 m1Center = (m1p1 + m1p2) * 0.5f;
	glm::vec3 m2Center = (m2p1 + m2p2) * 0.5f;
	glm::vec3 v = m1Center - m2Center;
	glm::vec3 m1x = m1.rotate[0] * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 m1y = m1.rotate[0] * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec3 m1z = m1.rotate[0] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec3 VA[3];
	VA[0] = m1x, VA[1] = m1y, VA[2] = m1z;
	glm::vec3 m2x = m2.rotate[0] * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 m2y = m2.rotate[0] * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec3 m2z = m2.rotate[0] * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
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

	extens1 = m1.scale * glm::vec4(m1.extents[0], m1.extents[1], m1.extents[2], 1.0f);
	extens2 = m2.scale * glm::vec4(m2.extents[0], m2.extents[1], m2.extents[2], 1.0f);

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
	glm::vec3 m1p1 = m1.trans[0] * m1.rotate[0] * m1.scale * glm::vec4(m1.collideBox[0].vertices[4].position, 1.0f);
	glm::vec3 m1p2 = m1.trans[0] * m1.rotate[0] * m1.scale * glm::vec4(m1.collideBox[0].vertices[3].position, 1.0f);

	glm::vec3 m2p1 = m2.trans[0] * m2.rotate[0] * m2.scale * glm::vec4(m2.collideBox[0].vertices[4].position, 1.0f);
	glm::vec3 m2p2 = m2.trans[0] * m2.rotate[0] * m2.scale * glm::vec4(m2.collideBox[0].vertices[3].position, 1.0f);

	glm::vec3 m1Center = (m1p1 + m1p2) * 0.5f;
	glm::vec3 m2Center = (m2p1 + m2p2) * 0.5f;

	glm::vec3 m1Diagonal = m1p2 - m1p1;
	glm::vec3 m2Diagonal = m2p2 - m2p1;

	glm::vec3 m1HalfSize = glm::abs(m1Diagonal) * 0.5f;
	glm::vec3 m2HalfSize = glm::abs(m2Diagonal) * 0.5f;

	glm::vec3 centersDistance = glm::abs(m2Center - m1Center);

	glm::vec3 distanceBetweenCenters = centersDistance - (m1HalfSize + m2HalfSize);

	glm::vec3 direction1 = m2.velocity[0] - m1.velocity[0];
	glm::vec3 direction2 = -direction1;

	glm::vec3 collidePos = m1Center + glm::length(m1HalfSize) * glm::normalize(m2Center - m1Center);

	float simulatedForce = sqrt(m1.mass * m2.mass) * (glm::length(m1.velocity[0]) + 1) * (glm::length(m2.velocity[0]) + 1) * 0.002f;
	//std::cout << glm::length(m1.velocity) + 1 << std::endl;
	//std::cout << glm::length(m2.velocity) + 1 << std::endl;
	m1.useForce(m1.deltaTime, simulatedForce * direction1, collidePos);
	m2.useForce(m2.deltaTime, simulatedForce * direction2, collidePos);
}

void Model::collisionWithShatter(Model& m2)
{
	glm::vec3 m2p1 = m2.trans[0] * m2.rotate[0] * m2.scale * glm::vec4(m2.collideBox[0].vertices[4].position, 1.0f);
	glm::vec3 m2p2 = m2.trans[0] * m2.rotate[0] * m2.scale * glm::vec4(m2.collideBox[0].vertices[3].position, 1.0f);
	glm::vec3 m2Center = (m2p1 + m2p2) * 0.5f;
	glm::vec3 m2Diagonal = m2p2 - m2p1;
	glm::vec3 direction1 = m2.velocity[0] - velocity[0];
	glm::vec3 direction2 = -direction1;
	float simulatedForce = sqrt(mass  * m2.mass) * (glm::length(velocity[0]) + 1) * (glm::length(m2.velocity[0]) + 1) * 0.002f;
	for (GLuint i = 1; i <= meshNum; i++) {
		glm::vec3 m1p1 = trans[0] * rotate[0] * scale * glm::vec4(collideBox[i].vertices[4].position, 1.0f);
		glm::vec3 m1p2 = trans[0] * rotate[0] *scale * glm::vec4(collideBox[i].vertices[3].position, 1.0f);

		glm::vec3 m1Center = (m1p1 + m1p2) * 0.5f;
		glm::vec3 m1Diagonal = m1p2 - m1p1;

		glm::vec3 m1HalfSize = glm::abs(m1Diagonal) * 0.5f;
		glm::vec3 m2HalfSize = glm::abs(m2Diagonal) * 0.5f;

		glm::vec3 centersDistance = glm::abs(m2Center - m1Center);

		glm::vec3 distanceBetweenCenters = centersDistance - (m1HalfSize + m2HalfSize);

		glm::vec3 collidePos = m1Center + glm::length(m1HalfSize) * glm::normalize(m2Center - m1Center);

		//std::cout << glm::length(m1.velocity) + 1 << std::endl;
		//std::cout << glm::length(m2.velocity) + 1 << std::endl;
		trans[i] = trans[0], rotate[i] = rotate[0], velocity[i] = velocity[0], angularVelocity[i] = angularVelocity[0];

		useForce(deltaTime, simulatedForce * direction1, collidePos, i);
	}
	splited = true;
}

void Model::prepareMesh(GLuint num) {
	for (GLuint i = 0; i < num; i++) {
		trans.push_back(glm::mat4(1.0f));
		rotate.push_back(glm::mat4(1.0f));
		velocity.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		acceleration.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		angularVelocity.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		angularAcceleration.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		xMin.push_back(1e8f), yMin.push_back(1e8f), zMin.push_back(1e8f);
		xMax.push_back(-1e8f), yMax.push_back(-1e8f), zMax.push_back(-1e8f);
	}
}


void Model::processInnerMesh()
{
	GLfloat gap = 0.05f;
	for (Mesh mesh : meshes) {
		Mesh tmp(mesh);
		for (Vertex &ver : tmp.vertices) {
			ver.position -= gap * ver.normal;
			ver.normal = -ver.normal;
		}
		innerMeshes.push_back(Mesh(tmp.vertices, tmp.indices, tmp.textures));
	}
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

	extents[0] = (xMax[0] - xMin[0]) / 2.0f;
	extents[1] = (yMax[0] - yMin[0]) / 2.0f;
	extents[2] = (zMax[0] - zMin[0]) / 2.0f;


	for (int i = 0; i <= meshNum; i++) {
		std::vector<Vertex> box;
		box.push_back(Vertex(glm::vec3(xMin[i], yMax[i], zMin[i])));
		box.push_back(Vertex(glm::vec3(xMin[i], yMax[i], zMax[i])));
		box.push_back(Vertex(glm::vec3(xMax[i], yMax[i], zMin[i])));
		box.push_back(Vertex(glm::vec3(xMax[i], yMax[i], zMax[i])));
		box.push_back(Vertex(glm::vec3(xMin[i], yMin[i], zMin[i])));
		box.push_back(Vertex(glm::vec3(xMin[i], yMin[i], zMax[i])));
		box.push_back(Vertex(glm::vec3(xMax[i], yMin[i], zMin[i])));
		box.push_back(Vertex(glm::vec3(xMax[i], yMin[i], zMax[i])));


		collideBox.push_back(Mesh(box, indices, t));
		defaultGravityPosition.push_back(glm::vec3(0.0f, (yMax[i] - yMin[i]) / 2.0f, 0.0f));
		gravityPosition.push_back(defaultGravityPosition[i]);
	}
}

void Model::loadModel(std::string path, bool useSplit)
{
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_ConvertToLeftHanded);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::LOADMODEL: " << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/') + 1);
	prepareMesh();
	processNode(scene->mRootNode, scene, useSplit);
	processCollideBox();
	processInnerMesh();
}


void Model::processNode(aiNode* node, const aiScene* scene, bool useSplit)
{

	for (unsigned i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh newMesh = processMesh(mesh, scene);
		if (useSplit) {
			std::vector<Mesh> newMeshes = newMesh.splitMesh(9 - rand() % 5, glm::vec3(xMax[0], yMax[0], zMax[0]), glm::vec3(xMin[0], yMin[0], zMin[0]));
			GLuint base = meshNum + 1, k = 0;
			meshNum += newMeshes.size();
			prepareMesh(newMeshes.size());
			for (int i = base; i <= meshNum; i++, k++) {
				for (Vertex vector : newMeshes[k].vertices) {
					if (vector.position.x > xMax[i])
						xMax[i] = vector.position.x;
					if (vector.position.x < xMin[i])
						xMin[i] = vector.position.x;
					vector.position.y = mesh->mVertices[i].y;
					if (vector.position.y > yMax[i])
						yMax[i] = vector.position.y;
					if (vector.position.y < yMin[i])
						yMin[i] = vector.position.y;
					vector.position.z = mesh->mVertices[i].z;
					if (vector.position.z > zMax[i])
						zMax[i] = vector.position.z;
					if (vector.position.z < zMin[i])
						zMin[i] = vector.position.z;
				}
				meshes.push_back(newMeshes[k]);
			}
		}
		else {
			prepareMesh();
			meshNum++;
			meshes.push_back(newMesh);
		}
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, useSplit);
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
		if (vector.x > xMax[0])
			xMax[0] = vector.x;
		if (vector.x < xMin[0])
			xMin[0] = vector.x;
		vector.y = mesh->mVertices[i].y;
		if (vector.y > yMax[0])
			yMax[0] = vector.y;
		if (vector.y < yMin[0])
			yMin[0] = vector.y;
		vector.z = mesh->mVertices[i].z;
		if (vector.z > zMax[0])
			zMax[0] = vector.z;
		if (vector.z < zMin[0])
			zMin[0] = vector.z;

		if (vector.x > xMax[meshNum])
			xMax[meshNum] = vector.x;
		if (vector.x < xMin[meshNum])
			xMin[meshNum] = vector.x;
		vector.y = mesh->mVertices[i].y;
		if (vector.y > yMax[meshNum])
			yMax[meshNum] = vector.y;
		if (vector.y < yMin[meshNum])
			yMin[meshNum] = vector.y;
		vector.z = mesh->mVertices[i].z;
		if (vector.z > zMax[meshNum])
			zMax[meshNum] = vector.z;
		if (vector.z < zMin[meshNum])
			zMin[meshNum] = vector.z;

		vertex.position = vector;
		
		/*if (mesh->mNormals) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = glm::normalize(vector);
		}
		else {*/
			vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
			mNormalsExist = false;
		/* }*/
		vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);

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
		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = LoadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular", scene);
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

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene)
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
	//std::cout << "d: " << defaultGravityPosition.y << std::endl;
	GLuint lowerBound = splited ? 1 : 0, upperBound = splited ? meshNum : 0;

	for (GLuint i = lowerBound; i <= upperBound; i++) {
		glm::vec3 deltaPos = velocity[i] * deltaTime;
		glm::mat4 deltaTrans = glm::translate(glm::mat4(1.0f), deltaPos);

		// 计算角速度对旋转矩阵的影响
		glm::vec3 rotationAxis = glm::length(angularVelocity[i]) > 0 ? glm::normalize(angularVelocity[i]) : glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 deltaRotate = glm::rotate(glm::mat4(1.0f), glm::length(angularVelocity[i]) * deltaTime, rotationAxis);

		// 更新位移和旋转矩阵
		trans[i] = deltaTrans * trans[i];
		rotate[i] = deltaRotate * rotate[i];

		if (useDecrease) {
			velocity[i] *= vDecreaseCo;
			angularVelocity[i] *= aDecreaseCo;
		}

		if (glm::length(velocity[i]) < vMin) {
			velocity[i] = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		if (glm::length(angularVelocity[i]) < aMin) {
			angularVelocity[i] = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		velocity[i] += acceleration[i] * deltaTime;
		angularVelocity[i] += angularAcceleration[i] * deltaTime;

		// 更新重心位置（位移影响）
		gravityPosition[i] = glm::vec3(trans[i] * glm::vec4(defaultGravityPosition[i], 1.0f));

		// 如果有重力，而且不会发生抖动，运用重力
		useForce(deltaTime, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), i);

		//和地板碰撞
		if (gravityPosition[i].y <= defaultGravityPosition[i].y && useGround) {
			hitGround(i);
		}
	}
}

void Model::useForce(float deltaTime, glm::vec3 force, glm::vec3 position, GLuint meshNum)
{
	//刚体受力计算函数，包含vec3 force：力矢量， vec3 position: 作用点
	// vec3 gravityPosition：重心位置, 
	// float mass: 质量,
	// float inertia: 转动惯量
	// vec3 acceleration: 加速度
	// vec3 angularAcceleration: 角加速度
	this->acceleration[meshNum] = glm::vec3(0.0f, 0.0f, 0.0f);
	this->angularAcceleration[meshNum] = glm::vec3(0.0f, 0.0f, 0.0f);
	if (glm::length(force) == 0) {
		if (useGravity && gravityPosition[meshNum].y > 0.5 * gravityCoefficient * deltaTime * deltaTime + defaultGravityPosition[meshNum].y) {
			this->acceleration[meshNum] = glm::vec3(0.0f, -gravityCoefficient, 0.0f);
		}
		else {
			return;
		}
	}
	//重心到力的作用点的距离（从作用点指向重心）
	glm::vec3 distance = gravityPosition[meshNum] - position;

	if (glm::length(distance) < 0.01) {
		//假如到重心的距离小于0.01，当做直接作用在重心上提供加速度的力
		//设置角加速度为0
		this->angularAcceleration[meshNum] = glm::vec3(0.0f, 0.0f, 0.0f);
		this->acceleration[meshNum] += force / mass;
		return;
	}
	// 力矩
	glm::vec3 torque = glm::cross(distance, force);
	// 加速度
	glm::vec3 accelerationComponent = force / mass;
	//角加速度
	glm::vec3 angularAccelerationComponent = torque / inertia;
	// 将力分成产生加速度和产生力的两部分
	// 加速度方向是力在质心处的投影
	glm::vec3 acceleration = glm::dot(force, distance) / glm::dot(distance, distance) * distance;
	// 扭矩方向是力垂直于距离的分量
	glm::vec3 torqueDirection = force - acceleration;
	glm::vec3 angularAcceleration = glm::cross(torqueDirection, distance) / inertia;

	// 更新模型的加速度和角加速度
	this->acceleration[meshNum] += acceleration;
	this->angularAcceleration[meshNum] = angularAcceleration;
}


void Model::hitGround(GLuint meshNum)
{
	/*std::cout << "g: " << gravityPosition.y << std::endl;
	std::cout <<"v: " << velocity.y << std::endl;
	*/
	//防止穿地板
	if (gravityPosition[meshNum].y < 0.25f + defaultGravityPosition[meshNum].y) {
		//假如模型的重心位置为负，将其往上提
		trans[meshNum] = glm::translate(trans[meshNum], glm::vec3(0.0f, -gravityPosition[meshNum].y + defaultGravityPosition[meshNum].y, 0.0f));
		gravityPosition[meshNum].y = defaultGravityPosition[meshNum].y;
	}

	bool b1 = gravityPosition[meshNum].y > 0.5 * gravityCoefficient * deltaTime * deltaTime + defaultGravityPosition[meshNum].y;
	if ( b1 || glm::length(velocity[meshNum]) > 3.5f) { //大于1m的坠落
		//和地板碰撞了	近似完全弹性碰撞
		velocity[meshNum].y = -velocity[meshNum].y * bounciness;
	}
	else if(glm::length(velocity[meshNum]) > 3.5f){
		//为了减少近地面的抽搐抖动
		velocity[meshNum].y = -velocity[meshNum].y * 0.7f * bounciness;
	}
	else {
		velocity[meshNum].y = -0.5f * velocity[meshNum].y;
	}
}


void Model::setVelocity(float x, float y, float z)
{
	velocity[0].x = x;
	velocity[0].y = y;
	velocity[0].z = z;
}

void Model::setVelocity(float x, float y, float z, float v)
{
	velocity[0].x = x*v;
	velocity[0].y = y*v;
	velocity[0].z = z*v;
}

void Model::setAcceleration(float x, float y, float z)
{
	acceleration[0].x = x;
	acceleration[0].y = y;
	acceleration[0].z = z;
}

void Model::setAcceleration(float x, float y, float z, float v)
{
	acceleration[0].x = x*v;
	acceleration[0].y = y*v;
	acceleration[0].z = z*v;
}

void Model::setAngularVelocity(float x, float y, float z)
{
	angularVelocity[0].x = x;
	angularVelocity[0].y = y;
	angularVelocity[0].z = z;
}

void Model::setAngularVelocity(float x, float y, float z, float v)
{
	angularVelocity[0].x = x*v;
	angularVelocity[0].y = y*v;
	angularVelocity[0].z = z*v;
}

void Model::setAngularAcceleration(float x, float y, float z)
{
	angularAcceleration[0].x = x;
	angularAcceleration[0].y = y;
	angularAcceleration[0].z = z;
}

void Model::setAngularAcceleration(float x, float y, float z, float v)
{
	angularAcceleration[0].x = x*v;
	angularAcceleration[0].y = y*v;
	angularAcceleration[0].z = z*v;
}

void Model::setScale(float x, float y, float z)
{
	this->scale = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
	this->defaultGravityPosition[0] = scale * glm::vec4(0.0f, (yMax[0] - yMin[0]) / 2.0f, 0.0f, 1.0f);
}


void Model::revisePosition()
{
	this->splited = false;
	this->rotate[0] = glm::mat4(1.0f);
	this->gravityPosition = defaultGravityPosition;
	this->velocity[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	this->angularVelocity[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	this->acceleration[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	this->angularAcceleration[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	this->trans[0] = glm::translate(glm::mat4(1.0f), defaultPos);
}

