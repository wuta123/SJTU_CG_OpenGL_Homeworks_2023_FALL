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

bool lighted = false;
bool LPressed = false;
bool drawInner = false;
bool IPressed = false;
bool drawOutside = true;
bool OPressed = false;
bool drawBall = false;
bool bPressed = false;
bool firstDrawBall = true;
bool cPressed = false;
bool drawCollideBox = false;
bool collided = false;
bool useShatter = false;
bool gPressed = false;
void handleInput(Shader &shaderProgram, GLFWwindow *window, Camera &cam, Model &model) {
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !LPressed) {
		lighted = !lighted;
		glUniform1f(glGetUniformLocation(shaderProgram.ID, "lightUp"), lighted);
		if (lighted) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		LPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
		LPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !OPressed) {
		drawOutside = !drawOutside;
		OPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) {
		OPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !IPressed) {
		drawInner = !drawInner;
		IPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) {
		IPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		cam.revisePosition();
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bPressed) {
		drawBall = !drawBall;
		bPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
		bPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		model.revisePosition();
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cPressed) {
		drawCollideBox = !drawCollideBox;
		cPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
		cPressed = false;
	}

	if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gPressed) {
		useShatter = !useShatter;
		gPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
		gPressed = false;
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
	bool isFullScreen = false;
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
	//Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD, it will configures OpenGL
	gladLoadGL();

	//Specify the view port of OpenGL in the Window
	//Viewport goes from x = 0, y = 0, to x = 1920, y = 1080
	glViewport(0, 0, windowWidth, windowHeight);

	std::vector <Texture> tex;
	Shader shaderProgram("default.vert", "default.frag");
	Shader ballShader("ball.vert", "ball.frag");
	Shader groundShader("ground.vert", "ground.frag");
	// Store mesh data in vectors for the mesh
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

	glm::vec4 ballColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 modelModel = glm::mat4(1.0f);
	modelModel = glm::translate(modelModel, modelPos);
	Model model("./model/model.obj", true);
	model.useGravity = true;
	model.useDecrease = true;
	model.useGround = true;
	model.trans[0] = modelModel;
	model.defaultPos = modelPos;
	model.mass = 50;
	//model.setScale(20.0f, 20.0f, 20.0f);
	Model ball("./ball/ball.obj");
	ball.setScale(0.02f, 0.02f, 0.02f);

	ballShader.Activate();
	glUniform4f(glGetUniformLocation(ballShader.ID, "lightColor"), ballColor.x, ballColor.y, ballColor.z, ballColor.w);
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "baseColor"), 0.7f, 0.7f, 0.7f, 0.7f);
	groundShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(groundShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelModel));
	/*// Variables that help the rotation of the pyramid
	float rotation = 0.0f;
	double prevTime = glfwGetTime();
	*/
	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.5f, 2.0f));
	Ground ground;
	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	// Main while loop

	GLfloat lastTime = glfwGetTime();
	GLfloat frameGap = 1.0f / 60.0f;
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentTime = glfwGetTime();
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		handleInput(shaderProgram, window, camera, model);

		/*
		// Simple timer
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60)
		{
			rotation += 0.5f;
			prevTime = crntTime;
		}*/

		camera.updateMatrix(45.0f, 0.1f, 500.0f);
		//处理相机输入
		camera.Inputs(window);

		if (drawBall) {
			if (firstDrawBall) {
				ballShader.Activate();
				glm::vec3 ballPos = camera.Position * 50.0f + camera.Orientation * 400.0f;
				ball.trans[0] = glm::translate(ball.scale, ballPos);
				ball.setAngularVelocity(0.0f, 0.0f, 0.0f); //角速度清除
				ball.rotate[0] = glm::mat4(1.0f);
				ball.setVelocity(camera.Orientation.x, camera.Orientation.y, camera.Orientation.z, 40.0f);
				//glUniformMatrix4fv(glGetUniformLocation(ballShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(ball.trans));
				firstDrawBall = false;
			}
			if(useShatter){
				ball.mass = 5000;
				glUniform4f(glGetUniformLocation(ballShader.ID, "lightColor"), 0.8f, 0.0f, 0.0f, 1.0f);
			}
			else {
				ball.mass = 100;
				glUniform4f(glGetUniformLocation(ballShader.ID, "lightColor"), 0.8f, 0.8f, 0.8f, 0.8f);
			}
			ball.Draw(ballShader, camera);
			if (drawCollideBox) {
				if (lighted) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					ball.DrawCollideBox(ballShader, camera);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				else {
					ball.DrawCollideBox(ballShader, camera);
				}
			}
			if (!collided) {
					collided = Model::detectCollision(model, ball);
					if (collided) {
						if (useShatter) {
							model.collisionWithShatter(ball);
						}
						else Model::collisionWithNoShatter(model, ball);
					}
			}
			 if (currentTime - lastTime >= frameGap) {
				ball.move(frameGap);
			}
		}
		else {
			firstDrawBall = true;
			collided = false;
		}

		groundShader.Activate();
		glUniform3f(glGetUniformLocation(groundShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);

		if (lighted) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			ground.Draw(groundShader, camera);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
			ground.Draw(groundShader, camera);

		if(drawOutside)
				model.Draw(shaderProgram, camera);
		if(drawInner)
				model.DrawInnerMesh(shaderProgram, camera);
		if (drawCollideBox) {
			if (lighted) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					model.DrawCollideBox(shaderProgram, camera);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else {
					model.DrawCollideBox(shaderProgram, camera);
			}
		}
		if (currentTime - lastTime >= frameGap) {
				model.move(frameGap);
				lastTime = currentTime;
		}
	

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	ballShader.Delete();
	shaderProgram.Delete();
	//Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}


