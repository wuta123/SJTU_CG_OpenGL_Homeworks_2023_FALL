#version 330 core
layout (location = 0) in vec3 aPos;
uniform vec2 offset;
uniform float pointSize;

void main() {
	gl_Position = vec4(aPos.x+offset.x, aPos.y+offset.y, aPos.z, 1.0);

    gl_PointSize = pointSize;
 }