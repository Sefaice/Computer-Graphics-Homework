#version 330 core

out vec4 fcolor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;
in vec4 clipSpaceCoords;
in float time;
in float ClipSpacePosZ;
in vec4 FragPosLightSpace[3];

uniform sampler2D OceanTexture;
uniform sampler2D OceanNormalTexture;
uniform samplerCube Skybox;
uniform sampler2D ReflectionTexture;
uniform sampler2D RefractionTexture;
uniform sampler2D OceanDuDvTexture;

uniform vec3 lightColor;
uniform vec3 viewPos;

// CSM
const int CascadeNum = 3;
uniform sampler2D depthMap[CascadeNum];
uniform mat4 lightSpaceMatrix[CascadeNum];
uniform float cascadeEndClipSpace[CascadeNum];


// refl : refr
float schlick(float cosine, float ri)
{
	float r0 = (1 - ri) / (1 + ri);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow(1 - cosine, 5);
}

float calcShadow(int index)
{
	float shadow = 0;

	vec3 ndcCoords = FragPosLightSpace[index].xyz / FragPosLightSpace[index].w;
	ndcCoords = (ndcCoords + 1.0) / 2;

	float closestDepth = 0;
	closestDepth = texture(depthMap[index], ndcCoords.xy).r;

	float currentDepth = ndcCoords.z;
	if(currentDepth > 1.0)
		shadow = 0;
	else
	{
		float bias = 0.005;
		shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;  
		vec2 texelSize = 1.0 / textureSize(depthMap[index], 0);
		for(int x = -3; x <= 3; ++x)
		{
			for(int y = -3; y <= 3; ++y)
			{
				float pcfDepth = texture(depthMap[index], ndcCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}
		}
		shadow /= 49.0;
	}

	return shadow;
}

void main()
{
	//vec3 oceanColor = texture(OceanTexture, TexCoords).rgb;
	vec3 oceanColor = vec3(0.33, 0.55, 0.69);

	vec2 distorationTexCoords = vec2(TexCoords.x + 0.01 * time, TexCoords.y + 0.01 * time);

	vec3 tNormal = texture(OceanNormalTexture, distorationTexCoords).rgb;
	tNormal = normalize(TBN * tNormal);
	// sin fnc normal
	//vec3 norm = normalize(Normal);
	// bump map normal
	vec3 norm = tNormal;
	// both
	//vec3 norm = normalize(normalize(Normal) + tNormal);

	// ambient
	vec3 ambient = 0.5 * lightColor;
	// diffuse
	vec3 lightDir = normalize(vec3(30, 10, -20) - FragPos); // hack to point light to gain "波光粼粼" effect
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = 0.6 * diff * lightColor;
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(norm, halfwayDir), 0.0), 100.0);
	vec3 specular = spec * lightColor;

	// reflect
	vec3 inVec = -viewDir;
	vec3 reflVec = reflect(inVec, norm);
//	vec4 ReflClipSpaceCoords = vec4(clipSpaceCoords.xyz + normalize(reflVec), clipSpaceCoords.w);
//	vec3 ReflNDC = ReflClipSpaceCoords.xyz / ReflClipSpaceCoords.w;
//	ReflNDC = ReflNDC / 2.0 + 0.5;
//	vec2 reflTexCoords = vec2(ReflNDC.x, -ReflNDC.y);
	vec2 ndc = (clipSpaceCoords.xy / clipSpaceCoords.w) / 2.0 + 0.5;
	vec2 reflTexCoords = vec2(ndc.x, -ndc.y);
	reflTexCoords.x = clamp(reflTexCoords.x, 0.001, 0.999);
	reflTexCoords.y = clamp(reflTexCoords.y, -0.999, -0.001);
	vec2 distoration = (texture(OceanDuDvTexture, distorationTexCoords).rg * 2.0 - 1.0) * 0.05;
	reflTexCoords += distoration;
	vec3 reflColor = texture(ReflectionTexture, reflTexCoords).rgb;

	// refract
	vec3 refrVec = refract(reflVec, norm, 1.0 / 1.33);
//	vec4 RefrClipSpaceCoords = vec4(clipSpaceCoords.xyz + normalize(reflVec), clipSpaceCoords.w);
//	vec3 RefrNDC = RefrClipSpaceCoords.xyz / RefrClipSpaceCoords.w;
//	RefrNDC = RefrNDC / 2.0 + 0.5;
//  vec2 refrTexCoords = vec2(RefrNDC.x, RefrNDC.y);
	vec2 refrTexCoords = vec2(ndc.x, ndc.y);
	refrTexCoords += distoration;
	refrTexCoords = clamp(refrTexCoords, 0.001, 0.999);
	vec3 refrColor = texture(RefractionTexture, refrTexCoords).rgb;

	float fresnel = schlick(max(dot(-inVec, vec3(0, 1, 0)), 0), 1.0 / 1.33);
	// video turitual fresnel
	fresnel = max(dot(-inVec, vec3(0.0, 1.0, 0.0)), 0.0);
	fresnel = 1.0 - pow(fresnel, 2.0);
	vec3 result = mix(reflColor, refrColor, 1.0 - fresnel);
	result = mix(result, vec3(0, 0.3, 0.5), 0.1) * 0.7 + (diffuse + specular) * 0.5;

	// calculate shadow
	float bias = 0.0005;  
   // 计算阴影
	float shadow;
	vec3 debugColor;
	for(int i = 0; i < CascadeNum; i++)
	{
		if (ClipSpacePosZ <= cascadeEndClipSpace[i])
		{
			debugColor[i] = 0.1;

			shadow = calcShadow(i);
			// blend
			if(i > 0 && abs(ClipSpacePosZ - cascadeEndClipSpace[i-1]) < 1.0)
			{
				float factor = abs(ClipSpacePosZ - cascadeEndClipSpace[i-1]) / 1.0;
				//factor = factor / 2.0 + 0.5;
				shadow = shadow * factor + calcShadow(i-1) * (1.0 - factor);
				//shadow = calcShadow(i-1, norm, lightDir);
			}
			else if(i < CascadeNum-1 && abs(ClipSpacePosZ - cascadeEndClipSpace[i+1]) < 1.0)
			{
				float factor = abs(ClipSpacePosZ - cascadeEndClipSpace[i+1]) / 1.0;
				//factor = factor / 2.0 + 0.5;
				shadow = shadow * (1.0 - factor) + calcShadow(i+1) * factor;
				//shadow = calcShadow(i+1, norm, lightDir);
			}
			
			break;
		}
	}

	fcolor = vec4((1.0 - shadow / 2.5) * result, 1.0);
}