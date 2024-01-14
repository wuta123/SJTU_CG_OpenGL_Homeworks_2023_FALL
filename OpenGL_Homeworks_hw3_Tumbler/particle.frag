#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

uniform vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec3 camPos;

void main()
{
		FragColor = color;
}