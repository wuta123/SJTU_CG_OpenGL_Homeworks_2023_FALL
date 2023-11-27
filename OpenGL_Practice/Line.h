#pragma once
#ifndef LINE_CLASS_H
#define LINE_CLASS_H
#include "Point2D.h"
#include <cmath>

class Line {
public:
	Point2D startPoint, endPoint;
	float length, k;

	Line() {
		length = 0; k = 0;
	}
	Line(Point2D &p1, Point2D &p2) {
		startPoint.x = p1.x, startPoint.y = p1.y;
		endPoint.x = p2.x, endPoint.y = p2.y;

		length = pow(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2), 0.5);
		k = (p1.y - p2.y) / (p1.x - p2.x);
	}

	Line(Line& l) {
		startPoint = l.startPoint;
		endPoint = l.endPoint;
		length = l.length;
		k = l.k;
	}

	void reset(Point2D &p1, Point2D &p2) {
		startPoint.x = p1.x, startPoint.y = p1.y;
		endPoint.x = p2.x, endPoint.y = p2.y;

		length = pow(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2), 0.5);
		k = (p1.y - p2.y) / (p1.x - p2.x);
	}


};

#endif // !LINE_CLASS_H
