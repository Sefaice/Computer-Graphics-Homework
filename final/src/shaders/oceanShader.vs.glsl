#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texcoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;
out vec4 clipSpaceCoords;
out float time;
out float ClipSpacePosZ;
out vec4 FragPosLightSpace[3];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float t;
uniform mat4 lightSpaceMatrix[3];

const float L = 0.2f;
const float W = 2.0f / L;
const float A = 0.05f;
const float S = 0.1f;
const float P = S * W;
const vec3 D = vec3(0.9, 0.0, 0.8);

const float L1 = 0.1f;
const float W1 = 2.0f / L1;
const float A1 = 0.08f;
const float S1 = 0.05f;
const float P1 = S1 * W1;
const vec3 D1 = vec3(1.0, 0.0, 0.0);

void main()
{
	bool HackFlat = true;
	time = t;

	if(!HackFlat)
	{
		float x = pos.x;
		float z = pos.z;
		float y = A * sin(dot(D, vec3(x, 0.0, z) * W + t * P)) + A1 * sin(dot(D1, vec3(x, 0.0, z) * W1 + t * P1));

		gl_Position =  projection * view * model * vec4(x, y, z, 1.0);
		clipSpaceCoords = gl_Position;
		FragPos = vec3(model * vec4(x, y, z, 1.0));

		float dHx = (-W * D.x * A * cos(dot(D, vec3(x, 0.0, z) * W + t * P))) + (-W1 * D1.x * A1 * cos(dot(D1, vec3(x, 0.0, z) * W1 + t * P1)));
		float dHz = (-W * D.z * A * cos(dot(D, vec3(x, 0.0, z) * W + t * P))) + (-W1 * D1.z * A1 * cos(dot(D1, vec3(x, 0.0, z) * W1 + t * P1)));
		vec3 B = vec3(1, 0, dHx);
		vec3 T = vec3(0, dHz, 1);
		//Normal = vec3(-dHx, 1.0, -dHz);
		Normal = mat3(transpose(inverse(model))) * cross(T, B);

		TBN = mat3(B, T, Normal);

		TexCoords = texcoords;
	}
	else
	{
		float x = pos.x;
		float z = pos.z;
		float y = 0;

		gl_Position =  projection * view * model * vec4(x, y, z, 1.0);
		clipSpaceCoords = gl_Position;
		FragPos = vec3(model * vec4(x, y, z, 1.0));
		
		vec3 B = vec3(1, 0, 0);
	    vec3 T = vec3(0, 0, 1);
		Normal = mat3(transpose(inverse(model))) * cross(T, B);

		TBN = mat3(B, T, Normal);

		TexCoords = texcoords;

		for(int i = 0; i < 3; i++)
		{
			FragPosLightSpace[i] = lightSpaceMatrix[i] * model * vec4(pos, 1.0);
		}

		ClipSpacePosZ = gl_Position.z;
	}
}