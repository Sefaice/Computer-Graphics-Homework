#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform bool refl;

void main()
{
    TexCoords = aTexCoords;
	if (refl)
		gl_Position = vec4(aPos.x * 0.3 - 0.7, aPos.y * 0.3 + 0.7, aPos.z, 1.0);
	else
		gl_Position = vec4(aPos.x * 0.3 - 0.7, aPos.y * 0.3 + 0.05, aPos.z, 1.0);
}