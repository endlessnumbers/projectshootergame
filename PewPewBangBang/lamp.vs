#version 430
in vec4 position;

out vec3 vPosition;

void main()
{
	vPosition = position.xyz;
}
