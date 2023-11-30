#pragma once
#ifndef Point2D_CLASS_H
#define Point2D_CLASS_H

const GLfloat sq3 = sqrt(3);
const double PI = acos(-1.0);
class Point2D {

public:

	float x;
	float y;

	Point2D() {
		this->x = 0.0f, this->y = 0.0f;
	}

	Point2D(float x, float y) {
		this->x = x, this->y = y;
	}

	Point2D(const Point2D& otherPoint) {
		this->x = otherPoint.x;
		this->y = otherPoint.y;
	}

	bool operator ==(Point2D& otherPoint) {
		return this->x == otherPoint.x && this->y == otherPoint.y;
	}

	bool operator ==(const bool& other) {
		if (other) {
			return this->x != 0.0f && this->y != 0.0f;
		}
		else {
			return this->x == 0.0f && this->y == 0.0f;
		}
	}

	Point2D operator*(float k) {
		return Point2D(this->x * k, this->y * k);
	}

	Point2D normalize() {
		float base = pow(pow(this->x, 2) + pow(this->y, 2), 0.5);
		return Point2D(this->x / base, this->y / base);
	}

	void rotateThetaAroundZAxis(float theta) {
		//逆时针旋转theta个角度
		theta = 3.14159265359f * theta / 360.f;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);
		float x = this->x, y = this->y;
		// 应用旋转矩阵
		this->x = x * cosTheta - y * sinTheta;
		this->y = x * sinTheta + y * cosTheta;

	}

	static Point2D rotateThetaAroundZAxis(Point2D& vector, float theta) {
		//将当前向量绕Z轴逆时针旋转thelta个角度
		theta = 3.14159265359f * theta / 360.f;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		float x = vector.x, y = vector.y;
		// 应用旋转矩阵
		vector.x = x * cosTheta - y * sinTheta;
		vector.y = x * sinTheta + y * cosTheta;

		return vector;
	}

	static Point2D* getAnHexagon(float length) {
		//获得一个正六边形，正六边形的中心在原点。length为六边形的中点到任意一个顶点的距离，返回的点顺序为：
		length = abs(length);
		Point2D* points = new Point2D[6];
		points[0].x = 0.0f, points[0].y = length;
		points[1].x = -sq3 * 0.5f * length, points[1].y = 0.5f * length;
		points[2].x = -points[1].x, points[2].y = points[1].y;
		points[3].x = points[1].x, points[3].y = -points[1].y;
		points[4].x = -points[3].x, points[4].y = points[3].y;
		points[5].x = 0.0f, points[5].y = -length;
		return points;
	}

	static Point2D* getAnCircle(float radius, int edges) {
		//返回一个点集，用对应边数的多边形来模仿一个圆
		Point2D* points = new Point2D[edges];
		for (int i = 0; i < edges; i++) {
			points[i].x = radius * cos(2 * PI * i / edges);
			points[i].y = radius * sin(2 * PI * i / edges);
		}
		return points;
	}

	float length() {
		return pow(pow(this->x, 2) + pow(this->y, 2), 0.5);
	}
};

#endif // !Point2D
