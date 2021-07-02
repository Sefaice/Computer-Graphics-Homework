#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform int specularPower;


void main()
{
	gl_Position =  projection * view * model * vec4(pos, 1.0);

	// ����������ϵ�м������
	vec3 Normal = mat3(transpose(inverse(model))) * normal;
	vec3 FragPos = vec3(model * vec4(pos, 1.0));
	// ambient ������
	vec3 ambient = ambientStrength * lightColor;
	// diffuse ɢ��/����
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;
	// specular ����
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	aColor = result;
}