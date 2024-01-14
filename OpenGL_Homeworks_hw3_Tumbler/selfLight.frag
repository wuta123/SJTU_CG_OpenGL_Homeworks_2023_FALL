#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;

// Imports the normal from the Vertex Shader
in vec3 Normal;

// Imports the color from the Vertex Shader
in vec3 color;

// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

void main()
{
		FragColor = vec4(1.0f, 1.0f, 1.0f, 0.0f);
}