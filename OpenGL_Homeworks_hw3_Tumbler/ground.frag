#version 330 core

out vec4 FragColor;
in float camDistance;

void main()
{
	float fadeCoefficient = 100.0f / camDistance;
	FragColor = vec4(0.8f, 0.8f, 0.8f, 0.8f) * (fadeCoefficient > 1.0f ? 1.0f : fadeCoefficient) ;
}