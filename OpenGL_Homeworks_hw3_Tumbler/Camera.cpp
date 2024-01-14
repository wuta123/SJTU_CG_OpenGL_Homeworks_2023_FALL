#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	this->height = height;
	this->width = width;
	this->Position = position;
	this->defaultPosition = position;
	this->lastClickPosition = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
	view = glm::lookAt(Position, Position + Orientation, Up);
	proj = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
	cameraMatrix = proj * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	//���������ŵ���ɫ����
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::revisePosition()
{
	//��Ӱ����λ
	Position = defaultPosition;
	Orientation = defaultOrientation;
	Up = defaultUp;
}

glm::vec3 Camera::ScreenToRay(float mouseX, float mouseY) {
	// ��ȡ��Ļ����� NDC ����
	float x = (2.0f * mouseX) / width - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / height;
	float z = 1.0f;

	// ����ͶӰ�������ͼ����������
	glm::mat4 inverseProjectionMatrix = glm::inverse(proj);
	glm::mat4 inverseViewMatrix = glm::inverse(view);

	// ����͸�ӳ���������
	glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

	// ����Ļ����ת��Ϊ�ü��ռ�����
	glm::vec4 rayEye = inverseProjectionMatrix * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	// ���ü��ռ�����ת��Ϊ��������
	glm::vec4 rayWorld = inverseViewMatrix * rayEye;
	glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

	return rayDirection;
}

void Camera::Inputs(GLFWwindow* window)
{
	//�����û�����
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Position += speed * Orientation;
		speed *= speedUp;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Position -= speed * Orientation;
		speed *= speedUp;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//��ˣ�����������ַ���orientation���ڣ�up���ϣ����ַ���������
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
		speed *= speedUp;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//����������߾Ϳ����Զ���ȡ��������
		Position += speed * glm::normalize(glm::cross(Up, Orientation));
		speed *= speedUp;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		//��������ͷ
		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		//��������ͷ
		Position -= speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		//�����ƶ�����ͷ
		if(speed < 0.04f) speed = 0.04f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		speed = 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
		speed = 0.01f;
	}


	//��괦��
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		//�״ε����Ҫ�������λ�õ���Ļ�����룬��ֹ��������
		if (firstClick) {
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			lastClickPosition = glm::vec2(mouseX, mouseY);
			firstClick = false;
		}

		double mouseX, mouseY;

		//��ȡ��굱ǰ��λ��
		glfwGetCursorPos(window, &mouseX, &mouseY);

		//���������뻭�����ĵ���ƶ����룬�����µ���ת�Ƕ�
		float rotX = sensitivity * (float)(mouseY - (lastClickPosition.y)) / height;
		float rotY = sensitivity * (float)(mouseX - (lastClickPosition.x)) / width;

		//��x��������Ϊ�ᣬ��ʱ����תrotX(����X��ʾ�������ĸ�Ϊ��)
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		//��������������ֱ��ת
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
			Orientation = newOrientation;
		}

		//��y��������Ϊ�ᣬ��ʱ��ת��rotY
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
		glfwSetCursorPos(window, lastClickPosition.x, lastClickPosition.y);

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}

}

glm::vec3 Camera::getPosition() {
	return this->Position;
}

glm::vec3 Camera::getOrientation() {
	return this->Orientation;
}
