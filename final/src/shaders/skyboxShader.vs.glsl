#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
	// The resulting normalized device coordinates will then always have a z value equal to 1.0: the maximum depth value
    gl_Position = pos.xyww;
}