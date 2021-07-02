#version 330 core

layout (location = 0) in vec2 pos;
//layout (location = 1) in vec3 in_color;

//out vec3 color;

uniform mat4 projection;
uniform int pointSize;

void main()
{
	gl_PointSize = pointSize;
	gl_Position = projection * vec4(pos, 0.0, 1.0);
	//color = in_color;
}