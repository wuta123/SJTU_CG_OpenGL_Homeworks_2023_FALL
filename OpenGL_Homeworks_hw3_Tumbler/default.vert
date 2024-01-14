#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aNormal;
// Texture Coordinates
layout (location = 2) in vec3 aColor;
//Normals of one plane
layout (location = 3) in vec2 aTex;


//Outputs the current Position for the Fragment shader
out vec3 crntPos;
//Outputs the normal for the Fragment shader
out vec3 Normal;
// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;
// Outputs the fragment poistion of the light
out vec4 fragPosLight;


// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 camMatrix;
//imports the model matrix from the main function
uniform mat4 model;
//imports the rotation matrix to make changes to the normals
uniform mat4 rotate = mat4(1.0f);
//imports the light matrix
uniform mat4 lightProjection;

void main()
{
	//calculates the current location
	crntPos = vec3(model * vec4(aPos, 1.0f));
		// Assigns the normal from the Vertex Data
	Normal = vec3(rotate * vec4(aNormal, 1.0f));
	// Assigns the colors from the Vertex Data to "color"
	color = aColor;
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;

	fragPosLight = lightProjection * vec4(crntPos, 1.0f);

		// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);
}