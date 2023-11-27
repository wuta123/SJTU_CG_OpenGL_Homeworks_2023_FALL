#include "Model.h"
#include <exception>
Model::Model(std::vector<Mesh> &meshes, Model &parent)
{
	this->meshes = meshes;
	for (int i = 0; i < meshes.size(); i++) {
		for (int j = 0; j < meshes[i].vertices.size(); j++) {
			if (meshes[i].vertices[j].position.x > xMax)
				xMax = meshes[i].vertices[j].position.x;
			if (meshes[i].vertices[j].position.x < xMin)
				xMin = meshes[i].vertices[j].position.x;
			if (meshes[i].vertices[j].position.y > yMax)
				yMax = meshes[i].vertices[j].position.y;
			if (meshes[i].vertices[j].position.y < yMin)
				yMin = meshes[i].vertices[j].position.y;
			if (meshes[i].vertices[j].position.z > zMax)
				zMax = meshes[i].vertices[j].position.z;
			if (meshes[i].vertices[j].position.z < zMin)
				zMin = meshes[i].vertices[j].position.z;
		}
	}
	processInnerMesh();
	processCollideBox();
	this->trans = parent.trans;
	this->scale = parent.scale;
	this->rotate = parent.rotate;
	this->gravityPosition = parent.gravityPosition - parent.defaultGravityPosition + this->defaultGravityPosition ;
	this->bounciness = parent.bounciness;
	this->useDecrease = parent.useDecrease;
	this->useGravity = parent.useGravity;
	this->useGround = parent.useGround;
	this->deltaTime = parent.deltaTime;
}

Model::Model(const Model& parent)
{
	this->useDecrease = parent.useDecrease;
	this->useGravity = parent.useGravity;
	this->useGround = parent.useGround;
	this->vDecreaseCo = parent.vDecreaseCo;
	this->aDecreaseCo = parent.aDecreaseCo;
	this->vMin = parent.vMin;
	this->aMin = parent.aMin;
	this->bounciness = parent.bounciness;
	this->inertia = parent.inertia;
	for (int i = 0; i < 3; i++)
		this->extents[i] = parent.extents[i];
	this->defaultGravityPosition = parent.defaultGravityPosition;
	this->gravityPosition = parent.gravityPosition;
	this->trans = parent.trans;
	this->scale = parent.scale;
	this->rotate = parent.rotate;
	this->defaultPos = parent.defaultPos;
	this->deltaTime = parent.deltaTime;
	this->velocity = parent.velocity;
	this->angularVelocity = parent.angularVelocity;
	this->acceleration = parent.acceleration;
	this->angularAcceleration = parent.angularAcceleration;
	this->mass = parent.mass;
	this->collideBox = parent.collideBox;
	this->directory = parent.directory;
	this->meshes = parent.meshes;
	this->innerMeshes = parent.innerMeshes;
	this->textures_loaded = parent.textures_loaded;
	this->xMin = parent.xMin;
	this->xMax = parent.xMax;
	this->yMax = parent.yMax;
	this->yMin = parent.yMin;
	this->zMax = parent.zMax;
	this->zMin = parent.zMin;

}

void Model::Draw(Shader& shader, Camera& cam)
{
	//GLuint location = glGetUniformLocation(shader.ID, "baseColor");
	shader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(trans * rotate));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(rotate));
	for (unsigned i = 0; i < meshes.size(); i++) {
		//glUniform4f(location, float(i)/meshes.size(), float(i) / meshes.size(), float(i)/meshes.size(), 0.7f);
		meshes[i].Draw(shader, cam);
	}
}

void Model::DrawInnerMesh(Shader& shader, Camera& cam){
	GLuint location = glGetUniformLocation(shader.ID, "baseColor");
	glUniform4f(location, 0.0f, 0.7f, 0.0f, 0.7f);
	for (unsigned i = 0; i < innerMeshes.size(); i++) {
		innerMeshes[i].Draw(shader, cam);
	}
	glUniform4f(location, baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
}

void Model::DrawCollideBox(Shader& shader, Camera& cam)
{
	GLuint location = glGetUniformLocation(shader.ID, "baseColor");
	glUniform4f(location, 0.0f, 0.7f, 0.0f, 0.7f);
	collideBox.Draw(shader, cam);
	glUniform4f(location, baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
}

bool Model::detectCollision(Model& m1, Model& m2)
{
	glm::vec3 m1p1 = m1.trans * m1.rotate * m1.scale * glm::vec4(m1.collideBox.vertices[4].position, 1.0f);
	glm::vec3 m1p2 = m1.trans * m1.rotate * m1.scale * glm::vec4(m1.collideBox.vertices[3].position, 1.0f);

	glm::vec3 m2p1 = m2.trans * m2.rotate * m2.scale * glm::vec4(m2.collideBox.vertices[4].position, 1.0f);
	glm::vec3 m2p2 = m2.trans * m2.rotate * m2.scale * glm::vec4(m2.collideBox.vertices[3].position, 1.0f);

	glm::vec3 m1Center = (m1p1 + m1p2) * 0.5f;
	glm::vec3 m2Center = (m2p1 + m2p2) * 0.5f;
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
	glm::vec3 m1p1 = m1.trans * m1.rotate * m1.scale * glm::vec4(m1.collideBox.vertices[4].position, 1.0f);
	glm::vec3 m1p2 = m1.trans * m1.rotate * m1.scale * glm::vec4(m1.collideBox.vertices[3].position, 1.0f);

	glm::vec3 m2p1 = m2.trans * m2.rotate * m2.scale * glm::vec4(m2.collideBox.vertices[4].position, 1.0f);
	glm::vec3 m2p2 = m2.trans * m2.rotate * m2.scale * glm::vec4(m2.collideBox.vertices[3].position, 1.0f);

	glm::vec3 m1Center = (m1p1 + m1p2) * 0.5f;
	glm::vec3 m2Center = (m2p1 + m2p2) * 0.5f;

	glm::vec3 m1Diagonal = m1p2 - m1p1;
	glm::vec3 m2Diagonal = m2p2 - m2p1;

	glm::vec3 m1HalfSize = glm::abs(m1Diagonal) * 0.5f;
	glm::vec3 m2HalfSize = glm::abs(m2Diagonal) * 0.5f;

	glm::vec3 centersDistance = glm::abs(m2Center - m1Center);

	glm::vec3 distanceBetweenCenters = centersDistance - (m1HalfSize + m2HalfSize);

	glm::vec3 direction1 = m2.velocity - m1.velocity;
	glm::vec3 direction2 = -direction1;

	glm::vec3 collidePos = m1Center + glm::length(m1HalfSize) * glm::normalize(m2Center - m1Center);

	float simulatedForce = m1.mass * m2.mass * (glm::length(m1.velocity) + 1) * (glm::length(m2.velocity) + 1) * 0.0002f;
	//std::cout << glm::length(m1.velocity) + 1 << std::endl;
	//std::cout << glm::length(m2.velocity) + 1 << std::endl;
	m1.useForce(m1.deltaTime, simulatedForce * direction1, collidePos);
	m2.useForce(m2.deltaTime, simulatedForce * direction2, collidePos);
}

void Model::collisionWithShatter(Model& m1, Model& m2, GLuint m1b)
{
	//碰撞函数，只有m1会破碎，m2只用作受力计算
	GLuint s1 = m1.meshes.size();
	std::vector<Model> subModel;
	for (GLuint i = 0; i < s1; i++) {
		std::vector<Mesh> res = m1.meshes[i].splitMesh(m1b + rand() % 5 - 2);
		GLuint tmp = res.size();
		for (GLuint j = 0; j < tmp;) {
			GLuint next = j + rand() % 6;
			std::vector<Mesh> subRes;
			if (next >= tmp)
				next = tmp;
			for (GLuint k = j; k < next; k++) {
				subRes.push_back(res[k]);
			}
			Model newSubModel(subRes, m1);
			newSubModel.mass = m1.mass / GLfloat(m1b);
			collisionWithNoShatter(newSubModel, m2);
			newSubModel.velocity /= glm::length(newSubModel.gravityPosition - m2.gravityPosition);
			subModel.push_back(newSubModel);
			j = next;
		}
	}

}

void Model::loadModel(std::string path)
{
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FixInfacingNormals | aiProcess_ConvertToLeftHanded);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::LOADMODEL: " << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/')+1);

	processNode(scene->mRootNode, scene);
	
	processInnerMesh();
}


void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}

	processCollideBox();
}

void Model::processInnerMesh()
{
	GLfloat gap = 0.05f;
	for (Mesh mesh : meshes) {
		Mesh tmp(mesh);
		for (Vertex &ver : tmp.vertices) {
			ver.position -= gap * ver.normal;
		}
		innerMeshes.push_back(Mesh(tmp.vertices, tmp.indices, tmp.textures));
	}
}

void Model::processCollideBox() {
	std::vector<Vertex> box;
	box.push_back(Vertex(glm::vec3(xMin, yMax, zMin)));
	box.push_back(Vertex(glm::vec3(xMin, yMax, zMax)));
	box.push_back(Vertex(glm::vec3(xMax, yMax, zMin)));
	box.push_back(Vertex(glm::vec3(xMax, yMax, zMax)));
	box.push_back(Vertex(glm::vec3(xMin, yMin, zMin)));
	box.push_back(Vertex(glm::vec3(xMin, yMin, zMax)));
	box.push_back(Vertex(glm::vec3(xMax, yMin, zMin)));
	box.push_back(Vertex(glm::vec3(xMax, yMin, zMax)));

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
	collideBox = Mesh(box, indices, t);
	extents[0] = (xMax - xMin) / 2.0f;
	extents[1] = (yMax - yMin) / 2.0f;
	extents[2] = (zMax - zMin) / 2.0f;

	defaultGravityPosition = glm::vec3(0.0f, extents[1], 0.0f);
	gravityPosition = defaultGravityPosition;
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
		if (vector.x > xMax)
			xMax = vector.x;
		if (vector.x < xMin)
			xMin = vector.x;
		vector.y = mesh->mVertices[i].y;
		if (vector.y > yMax)
			yMax = vector.y;
		if (vector.y < yMin)
			yMin = vector.y;
		vector.z = mesh->mVertices[i].z;
		if (vector.z > zMax)
			zMax = vector.z;
		if (vector.z < zMin)
			zMin = vector.z;

		vertex.position = vector;

		if (mesh->mNormals) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = glm::normalize(vector);
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
	glm::vec3 deltaPos = velocity * deltaTime;	
	glm::mat4 deltaTrans = glm::translate(glm::mat4(1.0f), deltaPos);

	// 计算角速度对旋转矩阵的影响
	glm::vec3 rotationAxis = glm::length(angularVelocity) > 0 ? glm::normalize(angularVelocity) : glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 deltaRotate = glm::rotate(glm::mat4(1.0f), glm::length(angularVelocity) * deltaTime, rotationAxis);

	// 更新位移和旋转矩阵
	trans = deltaTrans * trans;
	rotate = deltaRotate * rotate;

	if (useDecrease) {
		velocity *= vDecreaseCo;
		angularVelocity *= aDecreaseCo;
	}

	if (glm::length(velocity) < vMin) {
		velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	if (glm::length(angularVelocity) < aMin) {
		angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	velocity += acceleration * deltaTime;
	angularVelocity += angularAcceleration * deltaTime;

	// 更新重心位置（位移影响）
	gravityPosition =  glm::vec3(trans * glm::vec4(defaultGravityPosition, 1.0f));

	// 如果有重力，而且不会发生抖动，运用重力
	useForce(deltaTime);

	//和地板碰撞
	if (gravityPosition.y <= defaultGravityPosition.y && useGround) {
		hitGround();
	}
}

void Model::useForce(float deltaTime, glm::vec3 force, glm::vec3 position)
{
	//刚体受力计算函数，包含vec3 force：力矢量， vec3 position: 作用点
	// vec3 gravityPosition：重心位置, 
	// float mass: 质量,
	// float inertia: 转动惯量
	// vec3 acceleration: 加速度
	// vec3 angularAcceleration: 角加速度
	this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	this->angularAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	if (glm::length(force) == 0) {
		if (useGravity && gravityPosition.y > 0.5 * gravityCoefficient * deltaTime * deltaTime + defaultGravityPosition.y) {
			this->acceleration = glm::vec3(0.0f, -gravityCoefficient, 0.0f);
		}
		else {
			return;
		}
	}
	//重心到力的作用点的距离（从作用点指向重心）
	glm::vec3 distance = gravityPosition - position;

	if (glm::length(distance) < 0.01) {
		//假如到重心的距离小于0.01，当做直接作用在重心上提供加速度的力
		//设置角加速度为0
		this->angularAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
		this->acceleration += force / mass;
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
	this->acceleration += acceleration;
	this->angularAcceleration = angularAcceleration;
}

void Model::hitGround()
{
	/*std::cout << "g: " << gravityPosition.y << std::endl;
	std::cout <<"v: " << velocity.y << std::endl;
	*/
	//防止穿地板
	if (gravityPosition.y < 0.25f + defaultGravityPosition.y) {
		//假如模型的重心位置为负，将其往上提
		trans = glm::translate(trans, glm::vec3(0.0f, -gravityPosition.y + defaultGravityPosition.y, 0.0f));
		gravityPosition.y = defaultGravityPosition.y;
	}

	bool b1 = gravityPosition.y > 0.5 * gravityCoefficient * deltaTime * deltaTime + defaultGravityPosition.y;
	if ( b1 || glm::length(velocity) > 3.5f) { //大于1m的坠落
		//和地板碰撞了	近似完全弹性碰撞
		velocity.y = -velocity.y * bounciness;
	}
	else if(glm::length(velocity) > 3.5f){
		//为了减少近地面的抽搐抖动
		velocity.y = -velocity.y * 0.7f * bounciness;
	}
	else {
		velocity.y = -0.5f * velocity.y;
	}
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
	this->defaultGravityPosition = scale * glm::vec4(0.0f, (yMax - yMin) / 2.0f, 0.0f, 1.0f);
}


void Model::revisePosition()
{
	this->rotate = glm::mat4(1.0f);
	this->gravityPosition = defaultGravityPosition;
	this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	this->angularAcceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	this->trans = glm::translate(glm::mat4(1.0f), defaultPos);
}

