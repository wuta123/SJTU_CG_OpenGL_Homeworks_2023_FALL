#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>


#include "Shader.h"

class Camera {
public:
	glm::vec3 Position; //相机位置
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f); //相机朝向，默认是朝向内侧（屏幕里）
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); //相机的y轴正方向，默认是朝向上方
	glm::mat4 cameraMatrix = glm::mat4(1.0f);

	const glm::vec3 defaultPosition = glm::vec3(0.0f, 0.5f, 2.0f);
	const glm::vec3 defaultOrientation = glm::vec3(0.0f, 0.0f, -1.0f);
	const glm::vec3 defaultUp = glm::vec3(0.0f, 1.0f, 0.0f);
	//在用户用鼠标进行操作的时候进行消抖
	bool firstClick = true;

	//屏幕的宽和高
	int width;
	int height;

	//定义了相机的移动速度和旋转的速度
	float speed = 0.01f;
	float speedUp = 1.005f;
	float sensitivity = 30.0f;

	//构造函数，初始化
	Camera(int width, int height, glm::vec3 position);

	//更新相机矩阵
	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
	//将相机矩阵作用于顶点着色器上
	void Matrix(Shader& shader, const char* uniform);

	void revisePosition();
	//处理输入和输出
	void Inputs(GLFWwindow* window);
	glm::vec3 getPosition();
	glm::vec3 getOrientation();
};


#endif