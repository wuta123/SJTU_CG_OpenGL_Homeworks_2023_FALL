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
#include <vector>

class Camera {
public:
	glm::vec3 Position; //���λ��
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, 1.0f); //�������Ĭ���ǳ����ڲࣨ��Ļ�
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); //�����y��������Ĭ���ǳ����Ϸ�
	glm::mat4 cameraMatrix = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	glm::vec3 defaultPosition = glm::vec3(0.0f, 0.5f, 2.0f);
	glm::vec3 defaultOrientation = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 defaultUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec2 lastClickPosition;
	//���û��������в�����ʱ���������
	bool firstClick = true;

	//��Ļ�Ŀ�͸�
	int width;
	int height;

	//������������ƶ��ٶȺ���ת���ٶ�
	float speed = 0.01f;
	float speedUp = 1.005f;
	float sensitivity = 30.0f;

	//���캯������ʼ��
	Camera(int width, int height, glm::vec3 position);

	//�����������
	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
	//��������������ڶ�����ɫ����
	void Matrix(Shader& shader, const char* uniform);
	glm::vec3 ScreenToRay(float mouseX, float mouseY);
	void revisePosition();
	//������������
	void Inputs(GLFWwindow* window);
	glm::vec3 getPosition();
	glm::vec3 getOrientation();
};


#endif