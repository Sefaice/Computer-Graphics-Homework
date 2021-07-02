#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
// for gen shadows
out vec2 TexCoords;
out vec4 FragPosLightSpace[3];
// use for chossing cascade
out float ClipSpacePosZ;
// DEBUG
// --------------
out vec4 ViewSpacePos;
// --------------

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// for gen shadows
uniform mat4 lightSpaceMatrix[3];

void main()
{
	gl_Position =  projection * view * model * vec4(pos, 1.0);
	// 在世界坐标系中计算光照
	Normal = mat3(transpose(inverse(model))) * normal;
	FragPos = vec3(model * vec4(pos, 1.0));
	ClipSpacePosZ = gl_Position.z;
	// DEBUG
	// -----------------
	ViewSpacePos = view * model * vec4(pos, 1.0);
	// -----------------

	// 光源位置坐标计算
	for(int i = 0; i < 3; i++)
	{
		FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
	}
	TexCoords = texCoords;
}