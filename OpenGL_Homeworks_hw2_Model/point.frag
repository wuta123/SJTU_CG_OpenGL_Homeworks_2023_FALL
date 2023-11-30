#version 330 core
out vec4 FragColor;
void main(){
	float dis = distance(gl_PointCoord, vec2(0.5, 0.5));
	if(dis > 0.5)
		discard;
	else
		FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}