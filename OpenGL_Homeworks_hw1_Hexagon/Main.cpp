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



bool changeColor = false, movePosition = false, showBarrier = false,
moveSpot = false;

float lastYellowColor = 0.0f; //保存上一次的变色值


//按键状态变量
bool keyCPressed = false;
bool keySPressed = false;
bool keyDPressed = false;
bool keyAPressed = false;

void processInputs(GLFWwindow* window) {
	// 处理 C 键， 变色
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !keyCPressed) {
		changeColor = !changeColor;
		keyCPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
		keyCPressed = false;
	}

	// 处理 S 键， 展示边框
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !keySPressed) {
		showBarrier = !showBarrier;
		keySPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
		keySPressed = false;
	}

	// 处理 D 键， 移动位置
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !keyDPressed) {
		movePosition = !movePosition;
		keyDPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
		keyDPressed = false;
	}

	// 处理 A 键， 发射光斑
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !keyAPressed && showBarrier) {
		moveSpot = !moveSpot;
		keyAPressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
		keyAPressed = false;
	}

	// 处理 ESC 键
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}



int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	bool isFullScreen = true;
	GLFWmonitor* pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Helloword", pMonitor, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();

	GLfloat squeezeRate = 9.0f / 16.0f;
	int edges = 80;
	glViewport(0, 0, 1920, 1080);


	//获得六边形外边框的六个点
	Point2D* outerPoints = Point2D::getAnHexagon(0.5f);
	//六边形内部的三个点
	Point2D innerPoints[3] = {
	Point2D(0.0f, 0.3f),
	Point2D(-3.0 * sq3 / 20.0f, -3.0f / 20.0f),
	Point2D(3.0 * sq3 / 20.0f, -3.0f / 20.0f)
	};

	for (Point2D &point : innerPoints) {
		point.rotateThetaAroundZAxis(30.0f);
	}

	for (int i = 0; i < 6; i++) {
		outerPoints[i].rotateThetaAroundZAxis(-30.0f);
	}

	//Point2D* circlePoints = Point2D::getAnCircle(1.0f, edges);

	Shader shaderProgram("default.vert", "default.frag");

	Shader pointProgram("default.vert", "point.frag");

	//对六边形各个点按照屏幕的比例进行缩放
	float vertices[] = {
	outerPoints[0].x * squeezeRate, outerPoints[0].y, 0.0f,
	outerPoints[1].x * squeezeRate, outerPoints[1].y, 0.0f,
	innerPoints[0].x * squeezeRate, innerPoints[0].y, 0.0f,
	outerPoints[2].x * squeezeRate, outerPoints[2].y, 0.0f,
	innerPoints[1].x * squeezeRate, innerPoints[1].y, 0.0f,
	innerPoints[2].x * squeezeRate, innerPoints[2].y, 0.0f,
	outerPoints[3].x * squeezeRate, outerPoints[3].y, 0.0f,
	outerPoints[4].x * squeezeRate, outerPoints[4].y, 0.0f,
	outerPoints[5].x * squeezeRate, outerPoints[5].y, 0.0f,
	};

	//为绘制六边形和内嵌三角形做准备，一共画了10个三角形
	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3,
		1, 2, 4,
		2, 4, 5,
		2, 5, 3,
		1, 4, 6,
		3, 5, 7,
		6, 4, 8,
		4, 5, 8,
		5, 8, 7
	};



	//float* circleVertices = new float[edges * 3];
	//for (int i = 0; i < edges * 3; i += 3) {
	//	circleVertices[i] = circlePoints[i].x*squeezeRate;
	//	circleVertices[i + 1] = circlePoints[i].y;
	//	circleVertices[i + 2] = 0.0f;
	//}
	//
	//GLuint* circleIndices = new GLuint[edges];
	//for (int i = 0; i < edges; i++) {
	//	circleIndices[i] = i;
	//}

	GLfloat redBase = 0.0f;
	GLfloat greenBase = 0.1f;
	GLfloat blueBase = 0.4f;

	VAO VAO1;
	VAO1.Bind();
	
	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkVBO(VBO1, 0);

	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	//此处为每一个内嵌三角形单独绑定VAO，链接VBO
	GLint triangleSize = sizeof(indices) / sizeof(indices[0]) / 3.0f;
	VAO *triangleVAOS = new VAO[triangleSize];

	for (GLint i = 0; i < triangleSize; i++) {
		triangleVAOS[i].Bind();
		float triVertices[] = { 
			vertices[3*indices[3 * i]], vertices[3 * indices[3 * i]+1],vertices[3 * indices[3 * i]+2],
			vertices[3*indices[3 * i + 1]],  vertices[3*indices[3 * i + 1]+1], vertices[3*indices[3 * i + 1]+2],
			vertices[3*indices[3 * i + 2]], vertices[3*indices[3 * i + 2]+1], vertices[3*indices[3 * i + 2]+2]
		};
		VBO tmp(triVertices, sizeof(triVertices));
		triangleVAOS[i].LinkVBO(tmp, 0);
		triangleVAOS[i].Unbind();
		tmp.Unbind();
	}

//光斑传递
//原理：通过三角形的两个顶点，可以计算得出一个y = kx + b的边，x每增加1，y则增加k
//		想要实现光斑的传递，只需要计算出每个对应时间下，这个光斑的位移加上原位置，接着调用绘画光点的位置即可。
//		而为了实现光斑传递的统一化（使用的时间一致，每次光斑的移动速度应该和三角形的边的长度成正比
	std::set<std::pair<int, int>> lines;
	for (int i = 0; i < triangleSize; i++) {
		//对每一个三角形，实现光斑传递，但确保在重复边上的光斑只有一个，首先获取indice数组中的所有不重复边
		//比如，已经有边0, 1，就不再加边1, 0.
		int p1 = indices[3 * i], p2 = indices[3 * i + 1], p3 = indices[3 * i + 2];
		if (p1 > p2)
			std::swap(p1, p2);
		if (p2 > p3)
			std::swap(p2, p3);
		if (p1 > p2)
			std::swap(p1, p2);

		//1 2 , 1 3, 2 3
		std::pair<int, int> tmp;
		tmp.first = p1, tmp.second = p2;
		lines.insert(tmp);
		tmp.second = p3;
		lines.insert(tmp);
		tmp.first = p2;
		lines.insert(tmp);
	}
	//计算每一个边的k值和速度。

	int lineNum = lines.size();
	Line* lineList = new Line[lineNum];
	int j = 0;

	for (std::set<std::pair<int, int>>::iterator i = lines.begin(); i != lines.end() && j < lineNum; i++) {
		int p1 = (*i).first, p2 = (*i).second;
		Point2D point1(vertices[p1 * 3], vertices[p1 * 3 + 1]);
		Point2D point2(vertices[p2 * 3], vertices[p2 * 3 + 1]);
		lineList[j++].reset(point1, point2);
	}
	//提前创建用于保存移动光斑位置的数组
	int spotSize = 3 * lineNum;

	float *movingSpot = new float[spotSize] {0};
	float position = 0.0f, speed = 0.001f;//需要测试
	//储存移动的速度以及当前的时间。
	GLfloat colorChange = 0;
	//为一个拟合圆形绑定VAO，链接VBO
	//VAO circleVAO;
	//circleVAO.Bind();
	//VBO circleVBO(circleVertices, edges * 3 * sizeof(float));
	//EBO circleEBO(circleIndices, edges * sizeof(GLuint));
	//
	//circleVAO.LinkVBO(circleVBO, 0);
	//circleVAO.Unbind();
	//circleVBO.Unbind();
	//circleEBO.Unbind();


	while (!glfwWindowShouldClose(window)) {
		if (colorChange >= 2 * PI)
			colorChange = 0;
		processInputs(window);
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_PROGRAM_POINT_SIZE);
		// Tell OpenGL which Shader Program we want to use
		float timeValue = glfwGetTime();

		shaderProgram.Activate();

		

		int vertexOffsetLocation = glGetUniformLocation(shaderProgram.ID, "offset");
		int vertexColorLocation = glGetUniformLocation(shaderProgram.ID, "baseColor");
		float xOffset = sin(timeValue) / 3, yOffset = cos(timeValue) / 3;

		//变换位置，让边框做角速度恒定的圆周运动
		if (movePosition) glUniform2f(vertexOffsetLocation, xOffset, yOffset);
		else glUniform2f(vertexOffsetLocation, 0.0f, 0.0f);

		//为每一个三角形填上颜色
		//每一个三角形的颜色只要微调即可，微调就可以获取渐进色。
		//更换色调只需要更换底色，每次微调的色调保持不变。
		//为了好看，底色更换也可以使用渐进插值的方法！
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (changeColor)
			colorChange += 0.01;
		blueBase = 0.42f + sin(colorChange) / 5.0f;

		for (GLint i = 0; i < triangleSize; i++) {
			triangleVAOS[i].Bind();
			GLfloat red = redBase + (i * 0.035f); // 调整红色分量
			GLfloat green = greenBase + (i * 0.02f);
			std::cout << changeColor << std::endl;
			GLfloat blue = blueBase;
			GLfloat alpha = 1.0f-i*0.5f;  // 透明度设为 1

			glUniform4f(vertexColorLocation, red, green, blue, alpha);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			triangleVAOS[i].Unbind();
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform4f(vertexColorLocation, 255.0f, 255.0f, 255.0f, 1.0f);


		//绘制六边形边框，这个30是indices的长度，其实可以不要硬编码
		if (showBarrier) {
			VAO1.Bind();
			// Bind the VAO so OpenGL knows to use it
			glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
			VAO1.Unbind();


			//三角形顶点光斑，很简单，没有什么需要注释的
			pointProgram.Activate();
			//float pointSizeValue = sin(timeValue)*20; // PointSize太小显示不出来
			//glUniform1f(glGetUniformLocation(pointProgram.ID, "pointSize"), pointSizeValue);
			vertexOffsetLocation = glGetUniformLocation(pointProgram.ID, "offset");
			//变换位置，让光斑做圆周运动
			if (movePosition) glUniform2f(vertexOffsetLocation, xOffset, yOffset);
			else glUniform2f(vertexOffsetLocation, 0.0f, 0.0f);
			VAO1.Bind();
			//glDrawArrays(GL_POINTS ,0 , sizeof(vertices) / sizeof(float) / 3);
			int num_points = sizeof(vertices) / sizeof(float) / 3;

			for (int i = 0; i < num_points; i++) {
				float pointSizeValue = 15.0f + sin(2 * (timeValue + 0.3f * i)) * 10.0f; // 计算每个点的大小
				glUniform1f(glGetUniformLocation(pointProgram.ID, "pointSize"), pointSizeValue);
				glDrawArrays(GL_POINTS, i, 1); // 绘制当前点
			}
			VAO1.Unbind();

		}



		if (moveSpot) {
			//绘制光斑移动，从起点到终点。
			for (int i = 0; i < lineNum; i++) {
				//确定移动的方向
				Point2D movingDirection(lineList[i].endPoint.x - lineList[i].startPoint.x, lineList[i].endPoint.y - lineList[i].startPoint.y);
				//根据移动速度决定单次移动的长度
				movingDirection = movingDirection.normalize();
				Point2D currentLength = movingDirection * lineList[i].length * (position + speed);
				if (currentLength.length() >= lineList[i].length) {
					position = 0;
				}
				movingDirection = movingDirection * lineList[i].length * (position + speed);
				//乘以屏幕的压缩比例
				movingSpot[3 * i] = movingDirection.x + lineList[i].startPoint.x;
				movingSpot[3 * i + 1] = movingDirection.y + lineList[i].startPoint.y;
				movingSpot[3 * i + 2] = 0.0f;
			}
			position += speed;

			VAO movingSpotVAO;
			movingSpotVAO.Bind();
			VBO movingSpotVBO(movingSpot, sizeof(float) * spotSize);
			movingSpotVAO.LinkVBO(movingSpotVBO, 0);

			for (int i = 0; i < lineNum; i++) {
				float pointSizeValue = 15.0f + sin(2 * (timeValue + 0.2f * i)) * 3.0f; // 计算每个传递光斑的大小
				glUniform1f(glGetUniformLocation(pointProgram.ID, "pointSize"), pointSizeValue);
				glDrawArrays(GL_POINTS, i, 1); // 绘制当前点
			}

			movingSpotVAO.Unbind();
			movingSpotVBO.Unbind();

		}
		//错误处理
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << error << std::endl;
		}


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}