#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aNormal;
// Texture Coordinates
layout (location = 2) in vec3 aColor;
//Normals of one plane
layout (location = 3) in vec2 aTex;

// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 camMatrix;
uniform vec3 offset;

void main()
{
	gl_Position = camMatrix * vec4(aPos + offset, 1.0);
}