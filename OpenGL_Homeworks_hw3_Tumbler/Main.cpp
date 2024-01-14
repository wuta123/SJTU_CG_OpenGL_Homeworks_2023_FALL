#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Point2D.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Shader.h"
#include <set>
#include "Line.h"
#include <stb/stb_image.h>
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <streambuf>
#include "Camera.h"
#include "Mesh.h"
#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include "Model.h"
#include "Ground.h"


using std::pair;

bool drawFireBall = false;
bool fPressed = false;
bool firstDrawFireBall = true;

bool drawBall = false;
bool bPressed = false;
bool firstDrawBall = true;

bool cPressed = false;
bool drawCollideBox = false;
bool firstClick = true;
bool picked = false;

bool pPressed = false;
bool isPaused = false;

bool isQuit = false;

double mouseX, mouseY;
glm::vec3 world;
vector<pair<int, float>> rayCastIndex;
int pickedIndex = 0;

bool rayCastIndexCmp(pair<int, float> p1, pair<int, float> p2) {
	return p1.second < p2.second;
}

void DrawRayCast(glm::vec3 origin, glm::vec3 direction, Shader& shader, Camera& cam, float distance = 999.9f) {
	vector<Vertex> v;
	vector<GLuint> i;
	vector<Texture> t;

	v.push_back(Vertex(origin));
	v.push_back(Vertex(origin + direction * distance));
	i.push_back(0), i.push_back(1), i.push_back(0);

	Mesh tmp = Mesh(v, i, t);
	glm::vec4 originColor;
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), 0.0f, 1.0f, 0.0f, 1.0f);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotate"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	tmp.Draw(shader, cam);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform1i(glGetUniformLocation(shader.ID, "lightUp"), 1);
}

void printVec3(glm::vec3 vec) {
	std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
}

void handleInput(GLFWwindow *window, Camera &cam, vector<Model> &models, Shader &shader) {
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		cam.revisePosition();
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fPressed) {
		drawFireBall = !drawFireBall;
		fPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		fPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bPressed) {
		drawBall = !drawBall;
		bPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
		bPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pPressed) {
		isPaused = !isPaused;
		pPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
		pPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		for (int i = 0; i < models.size(); i++)
			models[i].revisePosition();
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cPressed) {
		drawCollideBox = !drawCollideBox;
		cPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
		cPressed = false;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		glfwGetCursorPos(window, &mouseX, &mouseY);
			firstClick = false;
			world = cam.ScreenToRay(mouseX, mouseY);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		isQuit = true;
	}
}

int main() {
	glfwInit();
	//Tell GLFW what version of OpenGL we are using
	//In this case OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Tell GLFW we are using the CORE profile, which means we only have
	//the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Get current Monitor if we need to create a fullscreen window
	bool isFullScreen = true;
	GLuint windowWidth = 1920, windowHeight = 1080;
	GLfloat squeezeRate = (float)windowHeight / (float)windowWidth;
	GLFWmonitor* pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
	//Create a GLFW window with 1920x1080 pixels, naming it "Helloword"
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Helloword", pMonitor, NULL);
	//Error check if the window fails to create
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//设置窗口始终前端显示
	//glfwSetWindowAttrib(window, GLFW_FLOATING, GLFW_TRUE);
	//Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD, it will configures OpenGL
	gladLoadGL();

	//Specify the view port of OpenGL in the Window
	//Viewport goes from x = 0, y = 0, to x = 1920, y = 1080
	glViewport(0, 0, windowWidth, windowHeight);

	Shader shaderProgram("default.vert", "default.frag");
	// Store mesh data in vectors for the mesh
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 18.0f, 0.0f);
	glm::vec4 ballColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//glm::vec3 lightPos = glm::vec3(0.0f, -0.2f, -0.5f);

	vector<Model> models;
	models.push_back(Model("./model/tumbler.obj", 15.0f));
	models[0].useGravity = true;
	models[0].useDecrease = true;
	models[0].useGround = true;
	models[0].mass = 50;
	models[0].inertia = 0.25 * models[0].mass * models[0].extents[0] * models[0].extents[1];
	models[0].setGravityOffset(0.0f, -0.7 * models[0].extents[1], 0.0f);
	models[0].setRotateOffset(glm::vec3(0.0f, models[0].extents[1] * (-0.4f), 0.0f));
	models[0].bounciness = 0.1f;
	models[0].setMaxLimit(10.0f, 20.0f, 10.0f);
	models[0].setMinLimit(-10.0f, 0.0f, -10.0f);
	models[0].useUpbound = true;
	models[0].setUpBound(20.0f, 40.0f);
	for (int i = 1; i < 5; i++) {
		models.push_back(models[0]);
	}
	models[1].setDefaultPosition(-6.0f, 0.7482f, 0.0f), models[2].setDefaultPosition(-3.0f, 0.7482f, 0.0f), models[3].setDefaultPosition(3.0f, 0.7482f, 0.0f), models[4].setDefaultPosition(6.0f, 0.7482f, 0.0f);
	for (int i = 1; i < models.size(); i++) {
		models[i].ID = i;
		models[i].revisePosition();
	}

	models[0].setDefaultPosition(0.0f, 0.7482f, 0.0f);
	models[0].setPosition(0.0f, 10.0f, 0.0f);

	for (int i = 0; i < models.size(); i++) {
		rayCastIndex.push_back(pair<int, float>(i, 0.0f));
	}
	Texture tex1("./texture/ceiling_gray.jpg", "texture_diffuse", 0);
	Texture tex2("./texture/ground_wooden.jpg", "texture_diffuse", 0);
	Texture tex3("./texture/wall_green.jpg", "texture_diffuse", 0);
	Texture tex4("./texture/wall_orange.jpg", "texture_diffuse", 0);
	Texture tex5("./texture/wall_violet.jpg", "texture_diffuse", 0);
	Texture texBall("./texture/ball_white.jpg", "texture_diffuse", 0);
	vector<Model> balls;
	balls.push_back(Model("./ball/ball.obj", 0.01f));
	balls[0].changeTexture(texBall);
	balls[0].mass = 10;
	balls[0].useGround = true;
	balls[0].useGravity = true;
	balls[0].setMaxLimit(10.0f, 20.0f, 10.0f);
	balls[0].setMinLimit(-10.0f, 0.0f, -10.0f);
	balls[0].ID = 5;
	balls[0].useTextureChange = true;
	balls[0].wallTex = { tex1, tex2, tex4, tex3, texBall, tex5};
	balls[0].useUpbound = true;
	balls[0].setUpBound(20.0f, 20.0f);
	balls[0].useDestroyParticle = true;
	balls[0].isVisible = false;
	
	ParticleGenerator DestroyParticle(600);
	DestroyParticle.setPT(ParticleGenerator::EVAPORATE);
	DestroyParticle.setBaseColor(0.95f, 0.85f, 0.85f);
	DestroyParticle.setRGBIncrease(-1.8f, -3.0f, -8.5f);
	DestroyParticle.setLifeTime(1.20);
	balls[0].AddDestroyParticle(DestroyParticle);
	//balls[0].fixVelocityAndAngularVelocity(10.0f, 10.0f);
	GLuint ballNum = 20;
	for (int i = 1; i < ballNum; i++) {
		balls.push_back(balls[0]);
	}
	balls[0].ID = models.size();
	for (int i = 1; i < ballNum; i++) {
		balls[i].ID = balls[i - 1].ID + 1;
	}


	Shader particleShader("particle.vert", "particle.frag");
	Model fireBall(balls[0]);
	fireBall.useTextureChange = false;
	fireBall.useUpbound = false;
	fireBall.useDestroyParticle = false;
	fireBall.isVisible = true;
	fireBall.fixVelocityAndAngularVelocity(20.0f, 0.0f);
	fireBall.ID = models.size() + balls.size();
	fireBall.useTrailingParticle = true;
	fireBall.useCollisionParticle = true;
	ParticleGenerator fireParticle = ParticleGenerator(3000);
	fireParticle.setBaseColor(0.95f, 0.85f, 0.85f);
	fireParticle.setRGBIncrease(-1.8f, -3.0f, -8.5f);
	fireParticle.setLifeTime(0.40);
	fireBall.AddTrailingParticle(fireParticle);
	ParticleGenerator collisionParticle = ParticleGenerator(50);
	collisionParticle.setPT(ParticleGenerator::SPREAD);
	collisionParticle.setBaseColor(0.95f, 0.85f, 0.65f);
	collisionParticle.setRGBIncrease(-1.8f, -1.7f, -2.2f);
	collisionParticle.setLifeTime(0.105);
	fireBall.AddCollisionParticle(collisionParticle);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightUp"), 1);

	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 10.0f, -40.0f));
	vector<Ground> walls;
	for(int i = 0; i < 5 ; i++)
		walls.push_back(Ground(2.0f, 2.0f, 10.0f, 10.0f));

	walls[0].setTex(tex2);
	walls[1].setTex(tex1);
	walls[1].setPosiAndRotation(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), -180.0f); //
	walls[2].setTex(tex3);
	walls[2].setPosiAndRotation(glm::vec3(-10.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), -90.0f);
	walls[3].setTex(tex4);
	walls[3].setPosiAndRotation(glm::vec3(10.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 90.0f); //
	walls[4].setTex(tex5);
	walls[4].setPosiAndRotation(glm::vec3(0.0f, 10.0f, 10.0f), glm::vec3(1.0f, 0.0f, 0.0f), -90.0f);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	// Main while loop

	GLfloat lastTime = glfwGetTime();
	GLfloat frameGap = 1.0f / 60.0f;

	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_FRAMEBUFFER_SRGB);

	Shader shadowMapProgram("shadow.vert", "shadow.frag");
	Shader shadowCubeMapProgram("shadowCubeMap.vert", "shadowCubeMap.frag", "shadowCubeMap.geom");

	unsigned shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);

	unsigned shadowMapWidth = 2048, shadowMapHeight = 2048;
	unsigned shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);
	
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float farPlane = 100.0f;
	// Matrices needed for the light's perspective
	glm::mat4 orthgonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, farPlane);
	glm::mat4 perspectiveProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightProjection = perspectiveProjection * lightView;

	shadowMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowMapProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

	// Framebuffer for Cubemap Shadow Map
	unsigned int pointShadowMapFBO;
	glGenFramebuffers(1, &pointShadowMapFBO);

	// Texture for Cubemap Shadow Map FBO
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Matrices needed for the light's perspective on all faces of the cubemap
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);
	glm::mat4 shadowTransforms[] =
	{
	shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
	shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
	shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
	shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
	shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
	shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};
	// Export all matrices to shader
	shadowCubeMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[0]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[0]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[1]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[1]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[2]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[2]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[3]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[3]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[4]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[4]));
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapProgram.ID, "shadowMatrices[5]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[5]));
	glUniform3f(glGetUniformLocation(shadowCubeMapProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(glGetUniformLocation(shadowCubeMapProgram.ID, "farPlane"), farPlane);

	Shader bolbProgram("selfLight.vert", "selfLight.frag");

	Model bolb("./model/bolb.obj", 5.0f);
	bolb.useGravity = false;
	bolb.useDecrease = false;
	bolb.useGround = false;
	bolb.changeTexture(tex1);
	bolb.setGravityOffset(0.0f, -0.7 * models[0].extents[1], 0.0f);
	bolb.bounciness = 0.1f;
	bolb.setMaxLimit(10.0f, 20.0f, 10.0f);
	bolb.setMinLimit(-10.0f, 0.0f, -10.0f);
	bolb.setDefaultPosition(0.0f, 19.99f, 0.0f);
	bolb.setDefaultRotate(glm::vec3(0.0f, 0.0f, 1.0f), 180);
	bolb.revisePosition();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		/*spotlight and direct light*/
		/*glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < walls.size(); i++)
			walls[i].Draw(shadowMapProgram, camera);
		for (int i = 0; i < models.size(); i++)
			models[i].Draw(shadowMapProgram, camera);
		if (drawBall)
			for (int i = 0; i < balls.size(); i++)
				balls[i].Draw(shadowMapProgram, camera);
		if (drawFireBall)
			fireBall.Draw(shadowMapProgram, camera);
		*/
		glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < walls.size(); i++)
			walls[i].Draw(shadowCubeMapProgram, camera);
		for (int i = 0; i < models.size(); i++)
			models[i].Draw(shadowCubeMapProgram, camera);
		if (drawBall)
			for (int i = 0; i < balls.size(); i++)
				balls[i].Draw(shadowCubeMapProgram, camera);
		if (drawFireBall)
			fireBall.Draw(shadowCubeMapProgram, camera);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowWidth, windowHeight);

		GLfloat currentTime = glfwGetTime();
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		handleInput(window, camera, models, shaderProgram);
		/*
		// Simple timer
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60)
		{
			rotation += 0.5f;
			prevTime = crntTime;
		}*/

		camera.updateMatrix(45.0f, 0.01f, 1000.0f);
		//处理相机输入
		camera.Inputs(window);

		shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "farPlane"), farPlane);
		// Bind the Shadow Map
		/*glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowMap"), 2);*/
		// Bind the Cubemap Shadow Map
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowCubeMap"), 2);
		
		bolb.Draw(bolbProgram, camera);

		for (int i = 0; i < walls.size(); i++)
			walls[i].Draw(shaderProgram, camera);

		for (int i = 0; i < models.size(); i++)
			models[i].Draw(shaderProgram, camera);

		if (drawCollideBox) {
			for (int i = 0; i < models.size(); i++)
				models[i].DrawCollideBox(shaderProgram, camera);
			for (int i = 0; i < models.size(); i++)
				models[i].DrawGravityPosition(shaderProgram, camera);
			//for (int i = 0; i < models.size(); i++)
			//	models[i].DrawLiftPosition(shaderProgram, camera);
			for (int i = 0; i < models.size(); i++)
				models[i].DrawRotateCenter(shaderProgram, camera);
		} 

		if(!firstClick)
			DrawRayCast(camera.Position + 0.1f * camera.Orientation, world, shaderProgram, camera);

		if (!firstClick && !isPaused) {
			if (!picked) {
				//按照模型中心距离摄像机的位置先排一下序号，越近的越先检测。
				//1.更新距离
				for (int i = 0; i < rayCastIndex.size(); i++) {
					rayCastIndex[i].second = models[rayCastIndex[i].first].getCenterDistance(camera.Position);
				}
				sort(rayCastIndex.begin(), rayCastIndex.end(), rayCastIndexCmp);
				for (int i = 0; i < models.size(); i++) {
					if (models[rayCastIndex[i].first].rayCast(camera.Position, world)) {
						picked = true;
						pickedIndex = rayCastIndex[i].first;
						break;
					}
				}
			}
			else
				models[pickedIndex].rayCast(camera.Position, world);
		}
		else if(!isPaused){
			models[pickedIndex].beReleased();
			picked = false;
		}

		if (drawFireBall) {
			if (firstDrawFireBall) {
				shaderProgram.Activate();
				glm::vec3 ballPos = camera.Position + camera.Orientation * 4.0f;
				//ball.trans = glm::translate(glm::mat4(1.0f), ballPos);
				fireBall.model = glm::translate(glm::mat4(1.0f), ballPos);
				fireBall.setAngularVelocity(0.0f, 0.0f, 0.0f); //角速度清除
				fireBall.rotate = glm::mat4(1.0f);
				fireBall.setVelocity(camera.Orientation.x, camera.Orientation.y, camera.Orientation.z, 10.0f);
				//ball.setVelocity(0.0f, 0.0f, 0.0f);
				firstDrawFireBall = false;
				fireBall.isVisible = true;
			}
			fireBall.Draw(bolbProgram, camera);

			if (drawCollideBox) {
				fireBall.DrawCollideBox(shaderProgram, camera);
			}
			particleShader.Activate();
			
			fireBall.DrawParticle(particleShader, camera);


			if (fireBall.collideID == -1) {
				for (int i = 0; i < models.size(); i++) {
					bool fireBallCollided = Model::detectCollision(models[i], fireBall);
					if (fireBallCollided) {
						fireBall.collideID = models[i].ID;
						Model::collisionWithNoShatter(models[i], fireBall);
						break;
					}
				}
				if (fireBall.collideID == -1) {
					for (int i = 0; i < balls.size(); i++) {
						bool fireBallCollided = Model::detectCollision(balls[i], fireBall);
						if (fireBallCollided) {
							Model::collisionWithNoShatter(balls[i], fireBall);
							balls[i].beDestroyed();
							break;
						}
					}
				}
			}
			else {
				bool fireBallCollided;
				if (fireBall.collideID < models.size())
					fireBallCollided = Model::detectCollision(models[fireBall.collideID], fireBall);
				else
					fireBallCollided = Model::detectCollision(balls[fireBall.collideID - models.size()], fireBall);
				if (!fireBallCollided)
					fireBall.collideID = -1;
			}

			if (!isPaused && currentTime - lastTime >= frameGap) {
				fireBall.move(frameGap);
			}
		}
		else {
			fireBall.isVisible = false;
			firstDrawFireBall = true;
		}

		if (drawBall) {
			if (firstDrawBall) {
				shaderProgram.Activate();
				for (int i = 0; i < balls.size(); i++) {
					glm::vec3 ballPos = glm::vec3(rand() % 20 - 10.0f, rand() % 20, rand() % 20 - 10.0f);
					//ball.trans = glm::translate(glm::mat4(1.0f), ballPos);
					balls[i].model = glm::translate(glm::mat4(1.0f), ballPos);
					balls[i].setAngularVelocity(0.0f, 0.0f, 0.0f); //角速度清除
					balls[i].rotate = glm::mat4(1.0f);
					if (glm::length(ballPos) == 0.0f)
						ballPos = glm::vec3(1.0f, 0.0f, 0.0f);
					else
						ballPos = glm::normalize(ballPos);
					balls[i].setVelocity(ballPos.x, ballPos.y, ballPos.z, 10.0f);
					balls[i].isVisible = true;
				}
				firstDrawBall = false;
			}

			for(int i = 0; i < balls.size(); i++)
				balls[i].Draw(shaderProgram, camera);

			if (drawCollideBox) {
				for (int i = 0; i < balls.size(); i++)
					balls[i].DrawCollideBox(shaderProgram, camera);
			}

			for (int i = 0; i < balls.size(); i++)
				balls[i].DrawParticle(particleShader, camera);

			for (int i = 0; i < balls.size(); i++) {
				if (balls[i].collideID == -1) {
					for (int j = 0; j < models.size(); j++) {
						bool ballCollided = Model::detectCollision(models[j], balls[i]);
						if (ballCollided) {
							balls[i].collideID = models[j].ID;
							Model::collisionWithNoShatter(models[j], balls[i]);
							break;
						}
					}
				}
				else {
					bool ballCollided = Model::detectCollision(models[balls[i].collideID], balls[i]);
					if (!ballCollided)
						balls[i].collideID = -1;
				}
			}

			if (!isPaused && currentTime - lastTime >= frameGap) {
				for (int i = 0; i < balls.size(); i++)
					balls[i].move(frameGap);
			}
		}
		else {
			for (int i = 0; i < balls.size(); i++)
				balls[i].isVisible = false;
			firstDrawBall = true;
		}


		if (!isPaused && currentTime - lastTime >= frameGap) {
			for (int i = 0; i < models.size(); i++)
				models[i].move(frameGap);
			lastTime = currentTime;
		}

		
		
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();

		if (isQuit)
			glfwSetWindowShouldClose(window, 1);
	}



	// Delete all the objects we've created
	shaderProgram.Delete();
	shadowMapProgram.Delete();
	shadowCubeMapProgram.Delete();
	glfwDestroyWindow(window);
	//Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}


