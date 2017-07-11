#version 330

in vec3 vPosition;

out vec3 facePos;

uniform mat4 proj, view, model;

void main()
{
	facePos = vPosition;

	gl_Position = proj * view * model * vec4(vPosition.x,vPosition.y,vPosition.z,1.0);	
}