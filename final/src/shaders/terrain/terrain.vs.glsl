#version 330 core
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;

out vec3 Positions;
out float height;
out vec3 Normals;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 oceanPlane;
uniform mat4 lightSpaceMatrix;

void main()
{
	vec4 worldPos =  model * vec4(v_position, 1.0);

    Positions = vec3(worldPos);
	height = v_position.y;
	Normals = mat3(transpose(inverse(model))) * v_normal;
    TexCoords = v_texcoord;

	gl_ClipDistance[0] = dot(oceanPlane, worldPos);

	gl_Position = projection * view * worldPos;

	FragPosLightSpace = lightSpaceMatrix * worldPos;
}
