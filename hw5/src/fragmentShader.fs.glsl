#version 330 core

out vec4 fcolor;

in vec3 NormalColor;

void main()
{
	fcolor = vec4(NormalColor, 1.0);
}