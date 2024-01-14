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
	//将相机矩阵放到着色器内
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::revisePosition()
{
	//摄影机归位
	Position = defaultPosition;
	Orientation = defaultOrientation;
	Up = defaultUp;
}

glm::vec3 Camera::ScreenToRay(float mouseX, float mouseY) {
	// 获取屏幕坐标的 NDC 坐标
	float x = (2.0f * mouseX) / width - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / height;
	float z = 1.0f;

	// 创建投影矩阵和视图矩阵的逆矩阵
	glm::mat4 inverseProjectionMatrix = glm::inverse(proj);
	glm::mat4 inverseViewMatrix = glm::inverse(view);

	// 计算透视除法的坐标
	glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

	// 将屏幕坐标转换为裁剪空间坐标
	glm::vec4 rayEye = inverseProjectionMatrix * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	// 将裁剪空间坐标转换为世界坐标
	glm::vec4 rayWorld = inverseViewMatrix * rayEye;
	glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

	return rayDirection;
}

void Camera::Inputs(GLFWwindow* window)
{
	//处理用户输入
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Position += speed * Orientation;
		speed *= speedUp;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Position -= speed * Orientation;
		speed *= speedUp;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//叉乘，叉乘遵守右手法则，orientation向内，up向上，右手法则叉乘向右
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
		speed *= speedUp;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//交换叉乘两边就可以自动获取反向向量
		Position += speed * glm::normalize(glm::cross(Up, Orientation));
		speed *= speedUp;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		//上移摄像头
		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		//下移摄像头
		Position -= speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		//快速移动摄像头
		if(speed < 0.04f) speed = 0.04f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		speed = 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
		speed = 0.01f;
	}


	//鼠标处理
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		//首次点击需要重置鼠标位置到屏幕正中央，防止画面跳动
		if (firstClick) {
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			lastClickPosition = glm::vec2(mouseX, mouseY);
			firstClick = false;
		}

		double mouseX, mouseY;

		//获取鼠标当前的位置
		glfwGetCursorPos(window, &mouseX, &mouseY);

		//根据鼠标距离画面中心点的移动距离，计算新的旋转角度
		float rotX = sensitivity * (float)(mouseY - (lastClickPosition.y)) / height;
		float rotY = sensitivity * (float)(mouseX - (lastClickPosition.x)) / width;

		//以x轴正方向为轴，逆时针旋转rotX(这里X表示的是以哪个为轴)
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		//这里避免了相机垂直翻转
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
			Orientation = newOrientation;
		}

		//以y轴正方向为轴，逆时针转动rotY
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
