#version 330 core

layout (location = 0) in vec3 pos;
//layout (location = 1) in vec3 in_color;

//out vec3 color;

void main()
{
	gl_Position = vec4(pos, 1.0f);
	//color = in_color;
}